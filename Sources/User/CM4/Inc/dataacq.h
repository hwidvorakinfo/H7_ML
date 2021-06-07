/*
 * dataacq.h
 *
 *  Created on: 18. 5. 2021
 *      Author: Petr Dvorak
 */

#ifndef SOURCES_USER_CM4_INC_DATAACQ_H_
#define SOURCES_USER_CM4_INC_DATAACQ_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx_hal.h"
#include "defs.h"
#include "main.h"
#include "application.h"
#include "dataacq_types.h"

#define PROGRESSBARLENGTH						10
#define PERIOD_OFFSET							10

uint32_t vspfunc(uint16_t index, char *frmt, ...);

RETURN_STATUS dacq_init(void);
RETURN_STATUS dacq_set_freq(uint32_t freq);
RETURN_STATUS dacq_set_time(uint32_t time);
RETURN_STATUS daqc_set_colfunc(uint8_t index, void *p_func);
RETURN_STATUS daqc_set_colnum(uint8_t num);
RETURN_STATUS datacq_send_data(void);
RETURN_STATUS dacq_csv(void);
RETURN_STATUS dacq_call_classifier(uint32_t lines, uint32_t offset);
RETURN_STATUS dacq_start_acq(void);
RETURN_STATUS dacq_stop_acq(void);
RETURN_STATUS dacq_set_colquantity(uint8_t column, dataacq_quantity_t quantity);
RETURN_STATUS dacq_set_colfile(uint8_t column, uint32_t file);
RETURN_STATUS dacq_csv(void);
RETURN_STATUS dacq_delall(void);



uint32_t dacq_get_freq(void);
uint32_t dacq_calculate_dma_size(void);
RETURN_STATUS dacq_set_serial_setup(dataacq_quantity_t mode);
dataacq_quantity_t dacq_get_serial_setup(void);
RETURN_STATUS dacq_read_serial_channels(void);
uint32_t dacq_adc_file_size(void);
uint32_t dacq_serial_file_size(void);
uint8_t dacq_number_of_adc_channels(void);
uint8_t dacq_number_of_serial_channels(void);
RETURN_STATUS dacq_cancel_progressbar(void);
RETURN_STATUS dacq_increment_line_numbers(void);
RETURN_STATUS dacq_read_serial_channels(void);
RETURN_STATUS dacq_logging_finished_message(void);
RETURN_STATUS dacq_stop_logging(void);


#ifdef __cplusplus
}
#endif

#endif /* SOURCES_USER_CM4_INC_DATAACQ_H_ */
