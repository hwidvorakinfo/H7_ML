/*
 * msg_types.h
 *
 *  Created on: 10. 5. 2021
 *      Author: Petr Dvorak
 */

#ifndef COMMON_INC_MSG_TYPES_H_
#define COMMON_INC_MSG_TYPES_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx_hal.h"
#include "defs.h"

#define MSG_PAYLOAD_LENGTH				256

// typy zprav podle CMD
#define MSG_COMM_BIND					0xA000
#define MSG_COMM_ALIVE					0xA001
#define MSG_LED_TOGGLE					0xB000
#define MSG_UART_MSG					0xC000
#define MSG_CLASS001_MSG				0xD000			// classifier enable

// status openamp messaging systemu
typedef enum {
	MESSAGING_INIT = 0,
	MESSAGING_ALIVE,
	MESSAGING_TIMEDOUT,
} AMP_MESSAGING_STATUS;

typedef struct {
 	uint16_t id;
 	uint16_t cmd;
 	uint16_t length;
 } msg_header_t;

typedef struct {
	msg_header_t header;
	uint8_t data[MSG_PAYLOAD_LENGTH];
} msg_t;

#ifdef __cplusplus
}
#endif

#endif /* COMMON_INC_MSG_TYPES_H_ */
