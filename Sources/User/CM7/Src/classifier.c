/*
 * classifier.c
 *
 *  Created on: 2. 6. 2021
 *      Author: Petr Dvorak
 */

#include "openamp.h"
#include "cmsis_os.h"
#include "msg_types.h"
#include "beasth7.h"
#include "string.h"
#include "classifier.h"

static volatile classifier_t classifier;

// inicializace spousteni a vlastnosti classifieru
RETURN_STATUS classifier_config(void)
{
	classifier_set_state(STOPPED);

	return RETURN_OK;
}

RETURN_STATUS classifier_set_state(classifier_state_t state)
{
	classifier.state = state;

	return RETURN_OK;
}

classifier_state_t classifier_get_state(void)
{
	return classifier.state;
}
