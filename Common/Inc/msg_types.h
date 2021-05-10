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

#define MSG_COMM_BIND					0xA000
#define MSG_LED_TOGGLE					0xB000
#define MSG_UART_MSG					0xC000


typedef struct {
	uint16_t id;
	uint16_t cmd;
	uint8_t data[MSG_PAYLOAD_LENGTH];
	uint16_t length;
} msg_t;

#ifdef __cplusplus
}
#endif

#endif /* COMMON_INC_MSG_TYPES_H_ */
