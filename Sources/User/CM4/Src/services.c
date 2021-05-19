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
#include "amp_messaging_cm4.h"
#include "commands.h"
#include "dataacq.h"

/* private prototypes */

extern volatile struct rpmsg_endpoint rp_endpoint;
extern volatile msg_t rx_message;

void Msg_service(void)
{
	volatile msg_t message;

	message.header.cmd = MSG_LED_TOGGLE;
	message.header.length = 0;
	OPENAMP_send((struct rpmsg_endpoint *)&rp_endpoint, (const void *)&message, sizeof(msg_header_t) + message.header.length);
}

void Msg_timeout_service(void)
{
	amp_set_status(MESSAGING_TIMEDOUT);
}

void Receive_service(void)
{
	static volatile uint16_t len = 0;
	len = receive_message();

	// zprava o pripravene komunikaci
	if (len != 0)
	{
		switch (rx_message.header.cmd)
		{
			case MSG_COMM_BIND:
				if(Scheduler_Add_Task(Msg_service, 0, MSG_SERVICE_PERIOD) == SCH_MAX_TASKS)
				{
					// chyba pri zalozeni service
				}
			break;

			case MSG_COMM_ALIVE:
				amp_set_status(MESSAGING_ALIVE);
				amp_set_timeout();
			break;

			case MSG_UART_MSG:
				uart1_send_message((uint8_t *)&rx_message.data, rx_message.header.length);
			break;

			default:
			break;
		}
	}
}

// sluzba pro dekodovani prikazu
void Cmd_decode_service(void)
{
//	volatile uint8_t data[2];
//	data[0] = '\r';
//	data[1] = '\n';
//	uart1_send_message((uint8_t *)&data, 2);

	commands_process();

	usart_set_receive_mode();
}

// resetujici sluzba
void Reset_service(void)
{
	NVIC_SystemReset();
}

// sluzba pro vysilani dat
void Datacq_service(void)
{
	datacq_send_data();
}
