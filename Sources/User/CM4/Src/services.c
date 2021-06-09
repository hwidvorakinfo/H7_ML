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
#include "adc_hal_cm4.h"
#include "amp_messaging_cm4.h"
#include "commands.h"
#include "dataacq.h"
#include "mikrobus.h"
#include "beasth7.h"
#include "classifier.h"

/* private prototypes */

static uint8_t msg_task_id = 0;

extern volatile struct rpmsg_endpoint rp_endpoint;
extern volatile msg_t rx_message[AMP_RX_MESSAGE_BUFFER_LEN];

// sluzba pro posilani periodickych zprav do CM7
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
	static volatile uint16_t number_of_messages_received = 0;
	volatile uint8_t index = 0;

	do
	{
		number_of_messages_received = amp_receive_message();

		switch (rx_message[index].header.cmd)
		{
			case MSG_COMM_BIND:
				if (msg_task_id == 0)
				{
					msg_task_id = Scheduler_Add_Task(Msg_service, 0, MSG_SERVICE_PERIOD);
					if(msg_task_id == SCH_MAX_TASKS)
					{
						// chyba pri zalozeni service
					}
				}
			break;

			case MSG_COMM_ALIVE:
				amp_set_status(MESSAGING_ALIVE);
				amp_set_timeout();
			break;

			case MSG_UART_MSG:
				uart1_send_message((uint8_t *)rx_message[index].data, rx_message[index].header.length);
			break;

			case MSG_CLASS002_MSG:
				class_received_finished((uint8_t *)rx_message[index].data, rx_message[index].header.length);
			break;

			default:
			break;
		}
		rx_message[index].header.cmd = 0;							// zneplatni zpravu
		index++;													// zvys index dalsi zpravy ke zpracovani

	}while (number_of_messages_received > 0);

}

// sluzba pro dekodovani prikazu
void Cmd_decode_service(void)
{
//	volatile uint8_t data[2];
//	data[0] = '\r';
//	data[1] = '\n';
//	uart1_send_message((uint8_t *)&data, 2);

	commands_process();

	usart_set_receive_mode((UART_HandleTypeDef *)&Uart1Handle);
}

// sluzba pro dekodovani prikazu z mikrobus
void Mikrobus_uart_decode_service(void)
{
	//commands_process();

	usart_set_receive_mode(&Uart2Handle);
}

// resetujici sluzba
void Reset_service(void)
{
	NVIC_SystemReset();
}

// sluzba pro vysilani dat
void Datacq_service(void)
{
	dacq_stop_logging();

	if (class_get_state() == AUTO_STOPPED)
	{
		// vypis zaverecnou zpravu
		dacq_logging_finished_message();
	}
	else
	{
		// uspesne ulozeno, nastav stav AUTO_STORED
		class_set_state(AUTO_STORED);
	}
}

// sluzba pro vypis progress baru
void Progressbar_service(void)
{
	static uint8_t state = 0;
	uint8_t text[] = "\r[          ]";
	uint8_t i;
#define POS			2

	for (i = 0; i < PROGRESSBARLENGTH; i++)
	{
		if (i <= state)
		{
			text[POS+i] = '|';
		}
	}

	uart1_send_message((uint8_t *)text, strlen((const char *)text));

	state++;
	if (state >= PROGRESSBARLENGTH)
	{
		state = 0;
	}
#undef POS
}

void Autoclass_service(void)
{
	class_automat();
}
