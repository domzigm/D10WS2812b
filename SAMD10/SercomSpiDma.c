/*
 * SercomSpiDma.c
 *
 * Created: 06.02.2017 12:52:05
 *  Author: M43734
 */ 

#include "config.h"
#include "atmel_start_pins.h"
#include <string.h>
#include <utils.h>
#include <hpl_gclk_base.h>
#include <hpl_pm_base.h>
#include <hri_dmac_d10.h>
 
COMPILER_ALIGNED(32)
volatile struct RGB LEDS[LED_ARR_SIZE];
volatile uint8_t dmaTransferStatus = 0;

COMPILER_ALIGNED(16) DmacDescriptor data_descriptor;
COMPILER_ALIGNED(16) DmacDescriptor temp_descriptor;
#if RESET_DMA == 1
COMPILER_ALIGNED(16) DmacDescriptor reset_descriptor;
static uint8_t BITRESET[4] = {0u};
#endif
 
static void setup_transfer_descriptor_tx(DmacDescriptor *descriptor)
{
	memset((void*)descriptor, 0, sizeof(DmacDescriptor));

	descriptor->BTCTRL.bit.VALID    = 1u;
	descriptor->BTCTRL.bit.BEATSIZE = DMAC_BTCTRL_BEATSIZE_BYTE;
	descriptor->BTCTRL.bit.SRCINC	 = 1u;
	descriptor->BTCNT.reg     = sizeof(LEDS) / sizeof(uint8_t);
	descriptor->SRCADDR.reg   = ((uint32_t)&LEDS) + sizeof(LEDS);
	descriptor->DSTADDR.reg   = (uint32_t)&SERCOM0->SPI.DATA.reg;
#if RESET_DMA == 1
	descriptor->DESCADDR.reg  = (uint32_t)&reset_descriptor;
#endif
}

static void setup_reset_descriptor_tx(DmacDescriptor *descriptor)
{
#if RESET_DMA == 1
	memset((void*)descriptor, 0, sizeof(DmacDescriptor));

	descriptor->BTCTRL.bit.VALID    = 1u;
	descriptor->BTCTRL.bit.BEATSIZE = DMAC_BTCTRL_BEATSIZE_BYTE;
	descriptor->BTCNT.reg   = 150u;
	descriptor->SRCADDR.reg = ((uint32_t)&BITRESET) + 1;
	descriptor->DSTADDR.reg = (uint32_t)&SERCOM0->SPI.DATA.reg;
#endif
}

static void DMA_INIT()
{
	hri_pm_set_AHBMASK_reg(PM, PM_AHBMASK_DMAC);
	hri_pm_set_APBBMASK_reg(PM, PM_APBBMASK_DMAC);

	// Reset DMAC
	hri_dmac_clear_CTRL_DMAENABLE_bit(DMAC);
	hri_dmac_clear_CTRL_CRCENABLE_bit(DMAC);
	hri_dmac_set_CHCTRLA_SWRST_bit(DMAC);

	hri_dmac_write_CTRL_reg(DMAC, (DMAC_CTRL_LVLEN0 | DMAC_CTRL_LVLEN1 | DMAC_CTRL_LVLEN2 | DMAC_CTRL_LVLEN3));
	hri_dmac_write_DBGCTRL_DBGRUN_bit(DMAC, 1);
	hri_dmac_write_BASEADDR_reg(DMAC, (uint32_t)&data_descriptor);
	hri_dmac_write_WRBADDR_reg(DMAC, (uint32_t)&temp_descriptor);

	hri_dmac_write_CHID_reg(DMAC, 0);
	hri_dmac_write_CHCTRLB_reg(DMAC,
		DMAC_CHCTRLB_TRIGACT_BEAT |
		DMAC_CHCTRLB_TRIGSRC(2));

	NVIC_ClearPendingIRQ(DMAC_IRQn);
	NVIC_EnableIRQ(DMAC_IRQn);
	hri_dmac_set_CTRL_LVLEN0_bit(DMAC);
	hri_dmac_set_CTRL_DMAENABLE_bit(DMAC);
}

void DMA_TRANSACTION()
{
	dmaTransferStatus = 1;
	hri_dmac_write_CHID_reg(DMAC, 0);
	hri_dmac_set_CHINTEN_TCMPL_bit(DMAC);
	hri_dmac_set_CHINTEN_TERR_bit(DMAC);
	hri_dmac_set_CHINTEN_SUSP_bit(DMAC);
	hri_dmac_set_CHCTRLA_ENABLE_bit(DMAC);
}

void DMAC_Handler(void)
{
	uint32_t status = 0;
	do 
	{
		status = hri_dmac_read_INTSTATUS_reg(DMAC);
		uint8_t channel = hri_dmac_read_INTPEND_ID_bf(DMAC);
		hri_dmac_write_CHID_reg(DMAC, channel);

		if (hri_dmac_get_CHINTFLAG_TERR_bit(DMAC))
		{
			hri_dmac_clear_CHINTFLAG_TERR_bit(DMAC);
		}
		else if (hri_dmac_get_CHINTFLAG_TCMPL_bit(DMAC))
		{
			hri_dmac_clear_CHINTFLAG_TCMPL_bit(DMAC);
			dmaTransferStatus = 0;
		}
#if 0
		else if (hri_dmac_get_CHINTFLAG_SUSP_bit(DMAC))
		{
			//hri_dmac_clear_CHINTFLAG_SUSP_bit(DMAC);
		}
#endif
	} while (status != 0);
}

#if 0
/* Only use if DMA is disabled */
void SERCOM0_SPI_WRITE(uint8_t* data, uint16_t length)
{
	for (uint16_t i = 0; i < length; i++) {
		while(!hri_sercomspi_get_interrupt_DRE_bit(SERCOM0));
		hri_sercomspi_write_DATA_reg(SERCOM0, data[i]);
		while(!hri_sercomspi_get_interrupt_TXC_bit(SERCOM0));
	}
}
#endif

void SERCOM0_SPI_INIT()
{
	hri_pm_set_APBCMASK_reg(PM, PM_APBCMASK_SERCOM0);

	hri_gclk_write_CLKCTRL_reg(GCLK,
		GCLK_CLKCTRL_ID_SERCOM0_CORE |
		GCLK_CLKCTRL_GEN_GCLK1 |
		GCLK_CLKCTRL_CLKEN);
	hri_gclk_wait_for_sync(GCLK);

	hri_sercomspi_set_CTRLA_SWRST_bit(SERCOM0);
	hri_sercomspi_wait_for_sync(SERCOM0, SERCOM_SPI_SYNCBUSY_SWRST);

	hri_sercomspi_write_CTRLA_reg(SERCOM0,
		//SERCOM_SPI_CTRLA_DORD |
		SERCOM_SPI_CTRLA_DOPO(0u) |
		SERCOM_SPI_CTRLA_DIPO(3u) |
		SERCOM_SPI_CTRLA_MODE(SERCOM_SPI_CTRLA_MODE_SPI_MASTER_Val));

	hri_sercomspi_write_BAUD_reg(SERCOM0, 9u);

	gpio_set_pin_direction(PA04, GPIO_DIRECTION_OUT);
	gpio_set_pin_level(PA04, false);
	gpio_set_pin_function(PA04, PINMUX_PA04D_SERCOM0_PAD0);

	hri_sercomspi_set_CTRLA_ENABLE_bit(SERCOM0);
	hri_sercomspi_wait_for_sync(SERCOM0, SERCOM_SPI_SYNCBUSY_ENABLE);
}

void setupSercomSpiDma()
{
	SERCOM0_SPI_INIT();

#if RESET_DMA == 1
	setup_reset_descriptor_tx(&reset_descriptor);
#endif
	setup_transfer_descriptor_tx(&data_descriptor);

	DMA_INIT();
}