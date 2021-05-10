/*
 * services.h
 *
 *  Created on: Dec 28, 2014
 *      Author: Petr Dvorak
 */

#ifndef INCLUDES_SERVICES_H_
#define INCLUDES_SERVICES_H_

#include "scheduler.h"

#define MILISEKUND 	/1000

// periody jsou v milisekundach, neboli zakladni periode SysTick casovace
#define MSG_SERVICE_PERIOD							(SCHEDULERPERIOD * 1000 MILISEKUND)
#define RECEIVE_SERVICE_PERIOD						(SCHEDULERPERIOD * 10 MILISEKUND)

#define FREE			0
#define STARTED			127
#define REQUESTED		64


// sluzby
void Msg_service(void);
void Receive_service(void);


#endif /* INCLUDES_SERVICES_H_ */
