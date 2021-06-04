/*
 * amp_messaging_cm7.c
 *
 *  Created on: 10. 5. 2021
 *      Author: Petr Dvorak
 */

#include "openamp.h"
#include "cmsis_os.h"
#include "msg_types.h"
#include "beasth7.h"
#include "string.h"
#include "stdarg.h"
#include "classifier.h"
#include "amp_messaging_cm7.h"

static volatile msg_t tx_message;
static volatile msg_t rx_message[AMP_RX_MESSAGE_BUFFER_LEN];
//static volatile uint16_t received_data;
struct rpmsg_endpoint rp_endpoint;
static volatile AMP_MESSAGING_STATUS amp_messaging_status = MESSAGING_INIT;
static volatile uint16_t num_of_received_messages = 0;

extern osSemaphoreId osSemaphore_ChannelCreation;

static int rpmsg_recv_callback(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv)
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

int16_t amp_receive_message(void)
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

void amp_send_alive_message(void)
{
	tx_message.header.cmd = MSG_COMM_ALIVE;
	tx_message.header.length = 0;
	OPENAMP_send(&rp_endpoint, (const void *)&tx_message, sizeof(msg_header_t) + tx_message.header.length);

}

void service_destroy_cb(struct rpmsg_endpoint *ept)
{
  /* this function is called while remote endpoint as been destroyed, the
   * service is no more available
   */
}

void new_service_cb(struct rpmsg_device *rdev, const char *name, uint32_t dest)
{
  /* create a endpoint for rmpsg communication */
  OPENAMP_create_endpoint(&rp_endpoint, name, dest, rpmsg_recv_callback, service_destroy_cb);
  osSemaphoreRelease(osSemaphore_ChannelCreation);
}

void amp_set_status(AMP_MESSAGING_STATUS status)
{
	amp_messaging_status = status;
}

AMP_MESSAGING_STATUS amp_get_status(void)
{
	return amp_messaging_status;
}

void vprint(const char *fmt, va_list argp)
{
    char string[MSG_PAYLOAD_LENGTH];

    if (amp_get_status() == MESSAGING_ALIVE)
    {
    	// pokud bezi system posilani zprav

		if(0 < vsprintf(string, fmt, argp)) // build string
		{
			//HAL_UART_Transmit(&Uart1Handle, (uint8_t*)string, strlen(string), 0xffffff); 		// send message via UART

			tx_message.header.cmd = MSG_UART_MSG;
			memcpy((void *)&tx_message.data, string, strlen((const char *)string));
			tx_message.header.length = strlen((const char *)string);

			OPENAMP_send(&rp_endpoint, (const void *)&tx_message, sizeof(msg_header_t) + tx_message.header.length);
		}
    }
}

int16_t amp_send_message(uint16_t cmd, void *data, uint16_t length)
{
	int16_t ret;

	tx_message.header.cmd = cmd;
	if (data != NULL)
	{
		memcpy((void *)&tx_message.data, data, length);
	}
	tx_message.header.length = length;
	ret = OPENAMP_send(&rp_endpoint, (const void *)&tx_message, sizeof(msg_header_t) + tx_message.header.length);
	osDelay(1);

	return ret;
}

RETURN_STATUS amp_message_decode(uint8_t index)
{
	switch (rx_message[index].header.cmd)
	{
		case MSG_LED_TOGGLE:
			HAL_GPIO_TogglePin(LEDA_GPIO_PORT, LEDA_PIN);
		break;

		case MSG_CLASS001_MSG:
			classifier_set_state(ENABLED);
		break;

		default:
		break;
	}

	rx_message[index].header.cmd = 0;							// zneplatni zpravu

	return RETURN_OK;
}
