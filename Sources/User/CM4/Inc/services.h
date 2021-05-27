/*
 * services.h
 *
 *  Created on: Dec 28, 2014
 *      Author: Petr Dvorak
 */

#ifndef INCLUDES_SERVICES_H_
#define INCLUDES_SERVICES_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "scheduler.h"

#define MILISEKUND 	/1000

// periody jsou v milisekundach, neboli zakladni periode SysTick casovace
#define MSG_SERVICE_PERIOD							(SCHEDULERPERIOD * 1000 MILISEKUND)
#define MSG_TIMEOUT_SERVICE_PERIOD					(SCHEDULERPERIOD * 5000 MILISEKUND)
#define RECEIVE_SERVICE_PERIOD						(SCHEDULERPERIOD * 10 MILISEKUND)
#define CMD_DECODE_SERVICE_PERIOD					(SCHEDULERPERIOD * 0 MILISEKUND)
#define MIKROBUS_DECODE_SERVICE_PERIOD				(SCHEDULERPERIOD * 0 MILISEKUND)
#define RESET_SERVICE_PERIOD						(SCHEDULERPERIOD * 1000 MILISEKUND)

#define FREE			0
#define STARTED			127
#define REQUESTED		64


// sluzby
void Msg_service(void);
void Msg_timeout_service(void);
void Receive_service(void);
void Cmd_decode_service(void);
void Mikrobus_uart_decode_service(void);
void Reset_service(void);
void Datacq_service(void);
void Progressbar_service(void);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDES_SERVICES_H_ */
