/*
 * config.h
 *
 * Created: 06.02.2017 12:53:02
 *  Author: M43734
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdint.h>

#define NUM_LEDS 38

#define RESET_DMA 0
#if RESET_DMA == 0
#define LED_ARR_SIZE (1 + NUM_LEDS + 2)
#else
#define LED_ARR_SIZE (1 + 1)
#endif

//#define CONF_UART_BAUD 57600
//#define CONF_UART_BAUD 115200
#define CONF_UART_BAUD 128000

#pragma pack(1)
struct uint24_t {
	uint32_t data : 24;
};

struct RGB {
	struct uint24_t G;
	struct uint24_t R;
	struct uint24_t B;
};
#pragma pack()

extern volatile struct RGB LEDS[LED_ARR_SIZE];
extern volatile uint8_t dmaTransferStatus;

void clearLeds();
void expandByte(uint8_t byte, volatile struct uint24_t* buf);

#endif /* CONFIG_H_ */
