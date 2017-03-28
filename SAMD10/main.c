/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include "config.h"
#include <stdio.h>
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>

void setupSercomUsart();
void setupSercomSpiDma();
void DMA_TRANSACTION();
uint8_t uart_getchar(const void* const USART);
void uart_putchar(const void* const USART, uint8_t data);

enum COMMANDS
{
	CMD_NULL = 0,
	CMD_GET_LEDCOUNT,
	CMD_SET_COLOR,
	CMD_SET_COLOR_SINGLE,
	CMD_GET_COLOR,
	CMD_GET_COLOR_SINGLE,
	CMD_UPDATE,
	CMD_READY = 0x55,
	CMD_ERROR = 0xFF
};

static uint8_t cmd = CMD_NULL;
static uint8_t uartBuffer[NUM_LEDS * 3];

void expandByte(uint8_t byte, volatile struct uint24_t* buf)
{
	uint32_t tmp = 0;
	
	for(uint8_t i=0; i<8; i++) {
		
		tmp <<= 3;

		if(byte & 0x80u) {
			tmp |= 0x06u;
		}
		else {
			tmp |= 0x04u;
		}
		
		byte <<= 1;
	}

	buf->data = ((tmp & 0x0000FF) << 16) |
				((tmp & 0x00FF00)      ) |
				((tmp & 0xFF0000) >> 16);
}

static void expandUint32(uint32_t blob, volatile struct RGB* buf)
{
	/* IF GRB order is not maintained, DMA will output wrong data?! */
	expandByte((blob & 0x00FF00) >>  8, &buf->G);
	expandByte((blob & 0xFF0000) >> 16, &buf->R);
	expandByte((blob & 0x0000FF)      , &buf->B);
}

void clearLeds()
{
	expandByte(0x00u, &LEDS[1].G);
	LEDS[1].R = LEDS[1].G;
	LEDS[1].B = LEDS[1].G;

	for(uint32_t i=2; i<=NUM_LEDS; i++) {
		LEDS[i] = LEDS[1];
	}
}

void SERCOM2_Handler(void)
{
	hri_sercomusart_write_INTEN_RXC_bit(SERCOM2, false);
}

int main(void)
{
	init_mcu();
	clearLeds();

	setupSercomUsart();
	setupSercomSpiDma();

	gpio_set_pin_direction(PA05, GPIO_DIRECTION_OUT);
	gpio_set_pin_level(PA05, true);

	uint8_t updateRequest = 1;
	NVIC_EnableIRQ(SERCOM2_IRQn);

	while(1)
	{
		if(dmaTransferStatus == 0 && updateRequest == 1)
		{
			updateRequest = 0;
			for(uint32_t i=0; i< NUM_LEDS; i++) {
				expandByte(uartBuffer[(i * 3) + 0], &LEDS[i + 1].R);
				expandByte(uartBuffer[(i * 3) + 1], &LEDS[i + 1].G);
				expandByte(uartBuffer[(i * 3) + 2], &LEDS[i + 1].B);
			}
			
			DMA_TRANSACTION();
		}
/*
		hri_sercomusart_write_INTEN_RXC_bit(SERCOM2, true);
		while(!hri_sercomusart_get_interrupt_RXC_bit(SERCOM2)) {
			__WFI();
		}
*/
		cmd = uart_getchar(SERCOM2);

		if(CMD_NULL == cmd)
		{
			uart_putchar(SERCOM2, CMD_READY);
		}
		else if(CMD_GET_LEDCOUNT == cmd)
		{
			uart_putchar(SERCOM2, NUM_LEDS);
			uart_putchar(SERCOM2, CMD_READY);
		}
		else if(CMD_GET_COLOR == cmd)
		{
			for(uint32_t i = 0; i<NUM_LEDS*3; i++)
			{
				uart_putchar(SERCOM2, uartBuffer[i]);
			}
			uart_putchar(SERCOM2, CMD_READY);
		}
		else if(CMD_SET_COLOR == cmd)
		{
			for(uint32_t i = 0; i<NUM_LEDS*3; i++)
			{
				uartBuffer[i] = uart_getchar(SERCOM2);
			}
			uart_putchar(SERCOM2, CMD_READY);
		}
		else if(CMD_GET_COLOR_SINGLE == cmd)
		{
			uint8_t pos = uart_getchar(SERCOM2);
			if(pos < NUM_LEDS)
			{
				for(uint8_t i=0; i<3; i++)
				{
					uart_putchar(SERCOM2, uartBuffer[ (pos * 3) + i ]);
				}
				uart_putchar(SERCOM2, CMD_READY);
			}
			else
			{
				uart_putchar(SERCOM2, CMD_ERROR);
			}
		}
		else if(CMD_SET_COLOR_SINGLE == cmd)
		{
			uint8_t pos = uart_getchar(SERCOM2);
			if(pos < NUM_LEDS)
			{
				for(uint32_t i = 0; i<NUM_LEDS*3; i++)
				{
					uartBuffer[ (pos * 3) + i ] = uart_getchar(SERCOM2);
				}
				uart_putchar(SERCOM2, CMD_READY);
			}
			else
			{
				uart_putchar(SERCOM2, CMD_ERROR);
			}
		}
		else if(CMD_UPDATE == cmd)
		{
			updateRequest = 1;
			uart_putchar(SERCOM2, CMD_READY);
		}
		else {
			uart_putchar(SERCOM2, CMD_ERROR);
		}
	}
}
