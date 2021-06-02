/*
 * classifier.c
 *
 *  Created on: 2. 6. 2021
 *      Author: Petr Dvorak
 */

#include "main.h"
#include "application.h"
#include "defs.h"
#include "error_handler.h"
#include "classifier.h"

extern volatile struct rpmsg_endpoint rp_endpoint;

RETURN_STATUS class_send_enable(void)
{
	volatile msg_t message;

	message.header.cmd = MSG_CLASS001_MSG;			// classifier enable
	message.header.length = 0;
	OPENAMP_send((struct rpmsg_endpoint *)&rp_endpoint, (const void *)&message, sizeof(msg_header_t) + message.header.length);

	return RETURN_OK;
}
