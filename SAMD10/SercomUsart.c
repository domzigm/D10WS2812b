/*
 * SercomUsart.c
 *
 * Created: 06.02.2017 13:04:02
 *  Author: M43734
 */ 

#include "config.h"
#include "atmel_start_pins.h"
#include <stdio.h>
#include <hpl_gclk_base.h>
#include <hpl_pm_base.h>
#include <peripheral_clk_config.h>

#define CONF_SERCOM_USART_BAUD_RATE      (65536 - ((65536 * 16.0f * CONF_UART_BAUD) / CONF_CPU_FREQUENCY))

#define SCOM SERCOM2

void SERCOM2_USART_INIT()
{
	hri_pm_set_APBCMASK_reg(PM, PM_APBCMASK_SERCOM2);

	hri_gclk_write_CLKCTRL_reg(GCLK,
	GCLK_CLKCTRL_ID_SERCOM2_CORE |
	GCLK_CLKCTRL_GEN_GCLK1 |
	GCLK_CLKCTRL_CLKEN);
	hri_gclk_wait_for_sync(GCLK);

	hri_sercomusart_set_CTRLA_SWRST_bit(SCOM);
	hri_sercomusart_wait_for_sync(SCOM, SERCOM_USART_SYNCBUSY_SWRST);

	hri_sercomusart_write_CTRLA_reg(SCOM,
		SERCOM_USART_CTRLA_RUNSTDBY |
		SERCOM_USART_CTRLA_MODE_USART_INT_CLK |
		SERCOM_USART_CTRLA_TXPO(1) |
		SERCOM_USART_CTRLA_RXPO(3) |
		SERCOM_USART_CTRLA_CPOL |
		SERCOM_USART_CTRLA_DORD);
	hri_sercomusart_write_CTRLB_reg(SCOM,
		SERCOM_USART_CTRLB_TXEN |
		SERCOM_USART_CTRLB_RXEN);
	hri_sercomusart_wait_for_sync(SCOM, SERCOM_USART_SYNCBUSY_CTRLB);
	hri_sercomusart_write_BAUD_reg(SCOM, (uint16_t) CONF_SERCOM_USART_BAUD_RATE);

	gpio_set_pin_function(PA10, PINMUX_PA10D_SERCOM2_PAD2);
	gpio_set_pin_function(PA11, PINMUX_PA11D_SERCOM2_PAD3);

	/* Enable the UART */
	hri_sercomusart_set_CTRLA_ENABLE_bit(SCOM);
}

uint8_t uart_getchar(const void* const USART)
{
	/* Wait to read a character from EDBG CDC UART*/
	while(!hri_sercomusart_get_interrupt_RXC_bit(USART));
	return hri_sercomusart_read_DATA_reg(USART);
}

void uart_putchar(const void* const USART, uint8_t data)
{
	/* Check if UART is ready to send a byte */
	while(!hri_sercomusart_get_interrupt_DRE_bit(USART));
	hri_sercomusart_write_DATA_reg(USART, data);
}

#if 0
int32_t stdio_io_read(uint8_t* const buf, uint16_t length)
{
	int32_t offset = 0;

	if(hri_sercomusart_get_CTRLA_ENABLE_bit(SCOM)) {
		do {
			buf[offset] = uart_getchar(SCOM);
		} while(++offset < length);
	}

	return offset;
}

int32_t stdio_io_write(const uint8_t* const buf, uint16_t length)
{
	int32_t offset = 0;
	 
	if(hri_sercomusart_get_CTRLA_ENABLE_bit(SCOM)) {
		do {
			uart_putchar(SCOM, buf[offset]);
		} while(++offset < length);
	}

	return offset;
}

int __attribute__((weak)) _read(int file, char *ptr, int len)
{
	int n = 0;

	if (file != 0) {
		return -1;
	}

	n = stdio_io_read((uint8_t *)ptr, len);
	if (n < 0) {
		return -1;
	}

	return n;
}

int __attribute__((weak)) _write(int file, char *ptr, int len)
{
	int n = 0;

	if ((file != 1) && (file != 2) && (file != 3)) {
		return -1;
	}

	n = stdio_io_write((const uint8_t *)ptr, len);
	if (n < 0) {
		return -1;
	}

	return n;
}
#endif

void setupSercomUsart()
{
	SERCOM2_USART_INIT();
	/*
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);
	*/
}