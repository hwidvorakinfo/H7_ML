/*
 * dataacq.c
 *
 *  Created on: 18. 5. 2021
 *      Author: Petr Dvorak
 */

#include "main.h"
#include "application.h"
#include "dataacq.h"
#include "defs.h"
#include "error_handler.h"
#include "services.h"
#include "string.h"
#include "uart_hal_cm4.h"

static volatile dataacq_setup_t datacq;
static volatile uint8_t buffer[TX1BUFFERSIZE];
static volatile uint16_t buffer_index;

RETURN_STATUS dacq_init(void)
{
	datacq.num_of_columns = 0;
	datacq.stime = 0;
	datacq.taskid = 0;

	uint8_t i;
	for (i = 0; i < MAX_NUM_OF_COLUMNS; i++)
	{
		datacq.p_getfunc[i] = NULL;
	}

	return RETURN_OK;
}

RETURN_STATUS dacq_set_stime(uint16_t time)
{
	datacq.stime = time;

	return RETURN_OK;
}

RETURN_STATUS daqc_set_colfunc(uint8_t index, void *p_func)
{
	datacq.p_getfunc[index] = p_func;

	return RETURN_OK;
}

RETURN_STATUS daqc_set_colnum(uint8_t num)
{
	datacq.num_of_columns = num;

	return RETURN_OK;
}

RETURN_STATUS dacq_start_acq(void)
{
	dacq_stop_acq();

	// zaloz novou sluzbu
	datacq.taskid = Scheduler_Add_Task(Datacq_service, 0, datacq.stime);
	if (datacq.taskid == SCH_MAX_TASKS)
	{
		// chyba pri zalozeni service
		return RETURN_ERROR;
	}
	return RETURN_OK;
}

RETURN_STATUS dacq_stop_acq(void)
{
	if (datacq.taskid != 0)
	{
		Scheduler_Delete_Task(datacq.taskid);
		datacq.taskid = 0;
	}

	return RETURN_OK;
}

RETURN_STATUS datacq_send_data(void)
{
	// sluzbu obsluhujici funkce
	uint8_t i;
	volatile uint8_t part;

	// priprava bufferu
	buffer[0] = '\r';
	buffer[1] = '\n';
	buffer_index = 2;

	for (i = 0; i < datacq.num_of_columns; i++)
	{
		if (datacq.p_getfunc[i] != NULL)
		{
			part = vspfunc(buffer_index, "%d", datacq.p_getfunc[i]());
			if (part != 0)
			{
				// doslo k uspesnemu prevodu
				buffer_index += part;

				if (i < (datacq.num_of_columns - 1))
				{
					// zakonci carkou a mezerou
					vspfunc(0, "%s, ", buffer);
					buffer_index += 2;
				}
				else
				{
					// zakonci strednikem
					vspfunc(0, "%s;", buffer);
				}
			}
		}
	}

	uart1_send_message((uint8_t *)buffer, strlen((const char *)buffer));

	return RETURN_OK;
}

uint32_t vspfunc(uint16_t index, char *format, ...)
{
   va_list aptr;
   uint32_t ret;

   va_start(aptr, format);
   ret = vsprintf((char *)&buffer[index], format, aptr);
   va_end(aptr);

   return(ret);
}

