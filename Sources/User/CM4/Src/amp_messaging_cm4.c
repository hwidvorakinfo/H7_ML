/*
 * amp_messaging_cm4.c
 *
 *  Created on: 17. 5. 2021
 *      Author: Petr Dvorak
 */

#include "openamp.h"
#include "cmsis_os.h"
#include "msg_types.h"
#include "beasth7.h"
#include "string.h"
#include "stdarg.h"
#include "scheduler.h"
#include "services.h"

static volatile AMP_MESSAGING_STATUS amp_messaging_status = MESSAGING_INIT;
static volatile uint8_t alive_service = 0;

void amp_set_status(AMP_MESSAGING_STATUS status)
{
	amp_messaging_status = status;
}

void amp_set_timeout(void)
{
	// zrus starou sluzbu, pokud existuje
	if (alive_service != 0)
	{
		Scheduler_Delete_Task(alive_service);
	}

	// zaloz novou sluzbu
	alive_service = Scheduler_Add_Task(Msg_timeout_service, MSG_TIMEOUT_SERVICE_PERIOD, 0);
	if (alive_service == SCH_MAX_TASKS)
	{
		// chyba pri zalozeni service
	}
}

void amp_cancel_timeout(void)
{
	alive_service = 0;
}
