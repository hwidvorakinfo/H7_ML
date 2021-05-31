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
	//datacq_send_data();

	// ukonci sber adc kanalu
	if (dacq_number_of_adc_channels() != 0)
	{
		adc_dma_unconfig();
	}

	// ukonci sber serial kanalu
	dacq_set_serial_setup(SERIAL_ACQ_DISABLED);

	// zastav synchronizacni casovac
	adc_sync_timer_deinit();

	// zrus sluzbu progress baru
	dacq_cancel_progressbar();

	// vypis zaverecnou zpravu
	dacq_logging_finished_message();

	// zhasni LEDD
	HAL_GPIO_WritePin(LEDD_GPIO_PORT, LEDD_PIN, GPIO_PIN_SET);
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
