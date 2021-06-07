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
#include "dataacq.h"
#include "model_metadata.h"

extern volatile struct rpmsg_endpoint rp_endpoint;

RETURN_STATUS class_send_enable(void)
{
	// zavolej classifier s odkazem na data
	dacq_call_classifier(EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, 0);

	return RETURN_OK;
}
