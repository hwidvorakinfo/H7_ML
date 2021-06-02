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
#include "amp_messaging_cm4.h"

static volatile AMP_MESSAGING_STATUS amp_messaging_status = MESSAGING_INIT;
static volatile uint8_t alive_service = 0;
volatile msg_t tx_message;
volatile msg_t rx_message[AMP_RX_MESSAGE_BUFFER_LEN];
//static volatile int16_t received_data;
static volatile uint16_t num_of_received_messages = 0;

int rpmsg_recv_callback(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv)
{
	if (num_of_received_messages < AMP_RX_MESSAGE_BUFFER_LEN)
	{
		// je jeste misto v bufferu
		memcpy((void *)&rx_message[num_of_received_messages], data, len);
		num_of_received_messages++;
	}
	else
	{
		// neni misto v bufferu, zpravu zahod
	}

	return 0;
}

// vyzvedni zpravu z bufferu zprav
// vrat pocet zprav v bufferu
unsigned int amp_receive_message(void)
{
	if (num_of_received_messages == 0)
	{
		OPENAMP_check_for_message();
		return 0;
	}
	else
	{
		return num_of_received_messages--;
	}
}

void amp_set_status(AMP_MESSAGING_STATUS status)
{
	amp_messaging_status = status;
}

AMP_MESSAGING_STATUS amp_get_status(void)
{
	return amp_messaging_status;
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
