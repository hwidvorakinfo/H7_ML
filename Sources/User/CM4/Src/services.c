/*
 * services.c
 *
 *  Created on: Dec 28, 2014
 *      Author: Petr Dvorak
 */

#include "services.h"
#include "application.h"
#include "errorcodes.h"
#include "openamp.h"
#include "main.h"
#include "defs.h"
#include "msg_types.h"
#include "uart_hal_cm4.h"

/* private prototypes */

extern volatile struct rpmsg_endpoint rp_endpoint;
extern volatile msg_t rx_message;

void Msg_service(void)
{
	volatile msg_t message;

	message.cmd = MSG_LED_TOGGLE;
	message.length = 0;
	OPENAMP_send(&rp_endpoint, (const void *)&message, sizeof(message));
}

void Receive_service(void)
{
	msg_t message;
	static volatile uint16_t len = 0;
	len = receive_message();

	// zprava o pripravene komunikaci
	if (len != 0)
	{
		switch (rx_message.cmd)
		{
			case MSG_COMM_BIND:
				if(Scheduler_Add_Task(Msg_service, 0, MSG_SERVICE_PERIOD) == SCH_MAX_TASKS)
				{
					// chyba pri zalozeni service
					return RETURN_ERROR;
				}
			break;

			case MSG_UART_MSG:
				uart1_send_message((uint8_t *)&rx_message.data, rx_message.length);
			break;

			default:
			break;
		}
	}
}

