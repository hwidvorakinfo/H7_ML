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
#include "services.h"
#include "uart_hal_cm4.h"

typedef struct {
    const char *label;
    float value;
} ei_impulse_result_classification_t;

typedef struct {
    int sampling;
    int dsp;
    int classification;
    int anomaly;
} ei_impulse_result_timing_t;

typedef struct {
#if EI_CLASSIFIER_OBJECT_DETECTION == 1
    ei_impulse_result_bounding_box_t bounding_boxes[EI_CLASSIFIER_OBJECT_DETECTION_COUNT];
#else
    ei_impulse_result_classification_t classification[EI_CLASSIFIER_LABEL_COUNT];
#endif
    float anomaly;
    ei_impulse_result_timing_t timing;
} ei_impulse_result_t;

static classifier_automat_t auto_classifier;

extern volatile struct rpmsg_endpoint rp_endpoint;

RETURN_STATUS class_init(void)
{
	// inicializace automatiky
	auto_classifier.enabled = FALSE;
	auto_classifier.state = AUTO_STOPPED;
	auto_classifier.taskid = 0;

	return RETURN_OK;
}

RETURN_STATUS class_start(void)
{
	// odstartuj automat, pokud je to mozne
	if (dacq_acquisition_ready() == RETURN_OK)
	{
		auto_classifier.enabled = TRUE;
		auto_classifier.state = AUTO_STARTED;

		// zalozeni tasku pro obsluhu automatickeho classifieru
		//uint32_t period = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE * dacq_get_freq() + AUTO_CLASSIFIER_LEADTIME;
		auto_classifier.taskid = Scheduler_Add_Task(Autoclass_service, 0, AUTO_CLASSIFIER_LEADTIME);
		if (auto_classifier.taskid == SCH_MAX_TASKS)
		{
			// chyba pri zalozeni service
			return RETURN_ERROR;
		}
		return RETURN_OK;
	}
	else
	{
		// malo dat
		return RETURN_ERROR;
	}
}

RETURN_STATUS class_stop(void)
{
	if (auto_classifier.taskid != 0)
	{
		Scheduler_Delete_Task(auto_classifier.taskid);
		auto_classifier.taskid = 0;
	}

	auto_classifier.enabled = FALSE;
	auto_classifier.state = AUTO_STOPPED;

	return RETURN_OK;
}

RETURN_STATUS class_automat(void)
{
	switch (auto_classifier.state)
	{
		case AUTO_STOPPED:

		break;

		case AUTO_STARTED:
			// pripraveno na start funkci DELALL
			if (dacq_delall() == RETURN_OK)
			{
				auto_classifier.state = AUTO_ERASED;
			}
		break;

		case AUTO_ERASED:
			// pripraveno na start funkci START
			if (dacq_start_acq() == RETURN_OK)
			{
				auto_classifier.state = AUTO_WAITING;
			}
		break;

		case AUTO_STORED:
			// pripraveno na predani funkci CLASSEN
			class_send_enable();
			auto_classifier.state = AUTO_WAITING;
		break;

		case AUTO_WAITING:
			// prazdny stav bez akce, cekani
		break;

		case AUTO_CLASSIFIED:
			// priprava na dalsi beh
			auto_classifier.state = AUTO_STARTED;
		break;

		default:

		break;
	}

	return RETURN_OK;
}

RETURN_STATUS class_set_state(classifier_automat_states_t state)
{
	auto_classifier.state = state;

	return RETURN_OK;
}

classifier_automat_states_t class_get_state(void)
{
	return auto_classifier.state;
}

RETURN_STATUS class_send_enable(void)
{
	// zavolej classifier s odkazem na data
	dacq_call_classifier(EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, 0);

	return RETURN_OK;
}

RETURN_STATUS class_received_finished(uint8_t *data, uint16_t len)
{
	if (auto_classifier.enabled == TRUE)
	{
		auto_classifier.state = AUTO_CLASSIFIED;
		ei_impulse_result_t *p_result = (ei_impulse_result_t *)data;

		// vypis vysledku
		myprintf("\r\nResults:\r\n");
		for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
		{
			myprintf("    %s: %.5f\r\n", p_result->classification[ix].label, p_result->classification[ix].value);
		}
		myprintf("    %s: %.5f\r\n", "anomaly", p_result->anomaly);
		myprintf("    %s: %d ms\r\n", "time", p_result->timing.classification);
	}

	return RETURN_OK;
}

RETURN_STATUS class_data_length_ok(uint16_t freq, uint16_t period)
{
	if ((period * freq / 1000) >= EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE)
	{
		return RETURN_OK;
	}
	else
	{
		return RETURN_ERROR;
	}
}

