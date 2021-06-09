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
#include "adc_hal_cm4.h"
#include "sdramfs.h"
#include "mpu-9250.h"
#include "beasth7.h"
#include "msg_types.h"
#include "classifier.h"

static volatile dataacq_setup_t datacq;
static volatile uint8_t buffer[TX1BUFFERSIZE];
static volatile uint16_t buffer_index;
static uint8_t progressbar_id;
static uint32_t linenumbers = 0;
static uint8_t *datacq_serial_data = NULL;
static sdramfs_record_t classifier_file_record;

extern volatile struct rpmsg_endpoint rp_endpoint;

RETURN_STATUS dacq_add_id_to_adc_columns(uint32_t id);
RETURN_STATUS dacq_add_id_to_serial_columns(uint32_t id);
RETURN_STATUS dacq_enable_serial(uint32_t address);
RETURN_STATUS dacq_csv_set_format(uint8_t *frmt, uint8_t *pattern);

RETURN_STATUS dacq_init(void)
{
	datacq.num_of_columns = 0;
	datacq.freq = 0;
	datacq.period = 0;
	datacq.taskid = 0;

	uint8_t i;
	for (i = 0; i < MAX_NUM_OF_COLUMNS; i++)
	{
		datacq.p_getfunc[i] = NULL;
		datacq.quantity_in_column[i] = NONE;
		datacq.file_for_column[i] = 0;
	}

	// inicializuj sdramfs
	if (sdramfs_init() != RETURN_OK)
	{
		Error_Handler();
	}

	return RETURN_OK;
}

RETURN_STATUS dacq_set_freq(uint32_t freq)
{
	datacq.freq = freq;

	return RETURN_OK;
}

uint32_t dacq_get_freq(void)
{
	return datacq.freq;
}

RETURN_STATUS dacq_set_time(uint32_t time)
{
	datacq.period = time;

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
	// ukonci bezici ulohu, pokud je aktivni
	// zastav periferie a ukladani
	dacq_stop_logging();

	// ukonci sluzbu Datacq_service
	if (datacq.taskid != 0)
	{
		Scheduler_Delete_Task(datacq.taskid);
	}

	sdramfs_record_t adc_record;						// zaznam pro soubor k ulozeni ADC dat
	sdramfs_record_t serial_record;						// zaznam pro soubor k ulozeni seriovych dat

	// nastav zaznam pro adc data
	if (dacq_number_of_adc_channels() != 0)
	{
		if (sdramfs_allocate_new_file(dacq_adc_file_size(), (sdramfs_record_t *)&adc_record) != RETURN_OK)
		{
			Error_Handler();
		}
		// v file_record je nyni id souboru a adresa, kde soubor zacina
		dacq_add_id_to_adc_columns(adc_record.id);
		// adc_record.address je adresa zacatku ukladani ADC dat

		// edituj zaznam souboru pro adc data
		if (sdramfs_edit_file_header((dataacq_setup_t *)&datacq, adc_record.id) != RETURN_OK)
		{
			Error_Handler();
		}

		// nastaveni ADC
		if (adc_dma_config(adc_record.address) != RETURN_OK)
		{
			Error_Handler();
		}
	}

	// nastav zaznam pro seriova data
	if (dacq_number_of_serial_channels() != 0)
	{
		if (sdramfs_allocate_new_file(dacq_serial_file_size(), (sdramfs_record_t *)&serial_record) != RETURN_OK)
		{
			Error_Handler();
		}
		// v serial_record je nyni id souboru a adresa, kde soubor zacina
		dacq_add_id_to_serial_columns(serial_record.id);
		// serial_record.address je adresa zacatku ukladani ADC dat
		datacq_serial_data = (uint8_t *)serial_record.address;

		// edituj zaznam souboru pro serial data
		if (sdramfs_edit_file_header((dataacq_setup_t *)&datacq, serial_record.id) != RETURN_OK)
		{
			Error_Handler();
		}

		// odstartuj serial sber
		dacq_enable_serial(serial_record.address);
	}
	// pokud je pozadavek na sber nejakeho kanalu, odstartuj synchronizacni casovac
	if ((dacq_number_of_adc_channels() != 0) || (dacq_number_of_serial_channels() != 0))
	{
		// odstartuj synchronizacni casovac
		if (adc_sync_start() != RETURN_OK)
		{
			Error_Handler();
		}
	}

	// zaloz novou sluzbu pro ukonceni zaznamu
	datacq.taskid = Scheduler_Add_Task(Datacq_service, datacq.period + PERIOD_OFFSET, 0);
	if (datacq.taskid == SCH_MAX_TASKS)
	{
		// chyba pri zalozeni service
		return RETURN_ERROR;
	}

	// zaloz novou sluzbu pro vypis prubehu mereni
	uint32_t pb_period = (datacq.period + PERIOD_OFFSET)/PROGRESSBARLENGTH;
	if (pb_period == 0)
	{
		pb_period = 1;
	}
	progressbar_id = Scheduler_Add_Task(Progressbar_service, 0, pb_period);
	if (progressbar_id == SCH_MAX_TASKS)
	{
		// chyba pri zalozeni service
		return RETURN_ERROR;
	}

	linenumbers = 0;

//	if (sdramfs_delete_file(new_record.id) != RETURN_OK)
//	{
//		Error_Handler();
//	}
//
//	if (sdramfs_delete_file(new_record2.id) != RETURN_OK)
//	{
//		Error_Handler();
//	}

//	// test zapisu do SDRAM
//	volatile uint32_t *p_word = (uint32_t *)0xD0000000;
//	uint32_t i;
//	for (i = 0; i < 10; i++)
//	{
//		*p_word++ = 0x55AA55AA;
//	}

//	// zaloz novou funkci pro zaznam z ADC
//	if (adc_dma_config() != RETURN_OK)
//	{
//		Error_Handler();
//	}
//
//	// zaloz novou sluzbu
//	datacq.taskid = Scheduler_Add_Task(Datacq_service, 0, datacq.period);
//	if (datacq.taskid == SCH_MAX_TASKS)
//	{
//		// chyba pri zalozeni service
//		return RETURN_ERROR;
//	}
	return RETURN_OK;
}

RETURN_STATUS dacq_stop_acq(void)
{
	// zastav periferie a ukladani
	dacq_stop_logging();

	// ukonci sluzbu Datacq_service
	if (datacq.taskid != 0)
	{
		Scheduler_Delete_Task(datacq.taskid);
	}

	if (class_get_state() == AUTO_STOPPED)
	{
		// autoclassifier nebezi
		dacq_logging_finished_message();	// vypis zaverecnou zpravu
	}
	else
	{
		// autoclassifier bezi
		class_stop();						// ukonci autoclassifier
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

RETURN_STATUS dacq_csv(void)
{
	// podle velicin ve sloupcich vypisuje vsechny dane hodnoty
	// id pro ADC data je nutne vycist ze zaznamu pro dany sloupec, pak podle nej adresu souboru z alokacni tabulky
	// id pro serial data je nutne vycist ze zaznamu pro dany sloupec, pak podle nej adresu souboru z alokacni tabulky
	uint32_t adc_id = 0;
	uint32_t adc_base_address = 0;
	uint32_t serial_id = 0;
	uint32_t serial_base_address = 0;
	uint8_t i;

	// najdi zacatek ADC souboru, pokud je pouzivan
	if (dacq_number_of_adc_channels() != 0)
	{
		for (i = 0; i < MAX_NUM_OF_COLUMNS; i++)
		{
			if ((datacq.quantity_in_column[i] > AINFIRST) && (datacq.quantity_in_column[i] < AINLAST))
			{
				adc_id = datacq.file_for_column[i];
				break;
			}
		}
		adc_base_address = sdramfs_get_address_from_id(adc_id);
		if (adc_base_address == 0)
		{
			return RETURN_ERROR;
		}
	}
	// najdi zacatek serial souboru, pokud je pouzivan
	if (dacq_number_of_serial_channels() != 0)
	{
		for (i = 0; i < MAX_NUM_OF_COLUMNS; i++)
		{
			if ((datacq.quantity_in_column[i] > SERIALFIRST) && (datacq.quantity_in_column[i] < SERIALLAST))
			{
				serial_id = datacq.file_for_column[i];
				break;
			}
		}
		serial_base_address = sdramfs_get_address_from_id(serial_id);
		if (serial_base_address == 0)
		{
			return RETURN_ERROR;
		}
	}

	volatile uint16_t *p_ainputs = (uint16_t *)adc_base_address;
	volatile uint16_t *p_serials = (uint16_t *)serial_base_address;

	uint32_t lines = linenumbers;
	volatile uint8_t column;
	volatile uint8_t part;
	volatile int32_t number;
	volatile float f_number;
	uint8_t text[] = "         ";

	// vytvor hlavicku zaznamu
	buffer[0] = '\r';
	buffer[1] = '\n';
	buffer_index = 2;

	// priprava pro timestamp
	volatile float f_timestamp = 0.0f;

	for (column = 0; column < datacq.num_of_columns; column++)
	{
		switch(datacq.quantity_in_column[column])
		{
			case AIN1:
				memcpy(&text, (const char *)"AIN1", strlen("AIN1"));
				text[strlen("AIN1")] = 0;
			break;

			case AIN2:
				memcpy(&text, (const char *)"AIN2", strlen("AIN2"));
				text[strlen("AIN2")] = 0;
			break;

			case AIN3:
				memcpy(&text, (const char *)"AIN3", strlen("AIN3"));
				text[strlen("AIN3")] = 0;
			break;

			case ACCX:
				memcpy(&text, (const char *)"ACCX", strlen("ACCX"));
				text[strlen("ACCX")] = 0;
			break;

			case ACCY:
				memcpy(&text, (const char *)"ACCY", strlen("ACCY"));
				text[strlen("ACCY")] = 0;
			break;

			case ACCZ:
				memcpy(&text, (const char *)"ACCZ", strlen("ACCZ"));
				text[strlen("ACCZ")] = 0;
			break;

			case GYROX:
				memcpy(&text, (const char *)"GYRX", strlen("GYRX"));
				text[strlen("GYRX")] = 0;
			break;

			case GYROY:
				memcpy(&text, (const char *)"GYRY", strlen("GYRY"));
				text[strlen("GYRY")] = 0;
			break;

			case GYROZ:
				memcpy(&text, (const char *)"GYRZ", strlen("GYRZ"));
				text[strlen("GYRZ")] = 0;
			break;

			case MAGX:
				memcpy(&text, (const char *)"MAGX", strlen("MAGX"));
				text[strlen("MAGX")] = 0;
			break;

			case MAGY:
				memcpy(&text, (const char *)"MAGY", strlen("MAGY"));
				text[strlen("MAGY")] = 0;
			break;

			case MAGZ:
				memcpy(&text, (const char *)"MAGZ", strlen("MAGZ"));
				text[strlen("MAGZ")] = 0;
			break;

			case TEMP:
				memcpy(&text, (const char *)"TEMP", strlen("TEMP"));
				text[strlen("TEMP")] = 0;
			break;

			case TIME:
				memcpy(&text, (const char *)"timestamp", strlen("timestamp"));
				text[strlen("timestamp")] = 0;
			break;

			default:
				memcpy(&text, (const char *)" -- ", strlen(" -- "));
			break;
		}
		part = vspfunc(buffer_index, "%s", text);
		if (part != 0)
		{
			// doslo k uspesnemu prevodu
			buffer_index += part;

			if (column < (datacq.num_of_columns - 1))
			{
				// zakonci carkou a mezerou
				vspfunc(0, "%s, ", buffer);
				buffer_index += 2;
			}
//			else
//			{
//				// zakonci strednikem
//				vspfunc(0, "%s;", buffer);
//			}
		}
	}
	uart1_send_message((uint8_t *)buffer, strlen((const char *)buffer));

	// format string
	volatile uint8_t format[] = "%f   ";

	// projdi pres vsechny radky
	while (lines--)
	{
		// priprava bufferu
		buffer[0] = '\r';
		buffer[1] = '\n';
		buffer_index = 2;

		// projdi pres vsechny sloupce
		for (column = 0; column < datacq.num_of_columns; column++)
		{
			switch(datacq.quantity_in_column[column])
			{
				case AIN1:
					number = *p_ainputs;
					// prevod na normalizovany rozmer
					#ifdef ADCRES_16B
					#endif // ADCRES_16B
					#ifdef ADCRES_14B
					number = number << 2;
					#endif // ADCRES_14B
					#ifdef ADCRES_12B
					number = number << 4;
					#endif // ADCRES_12B
					#ifdef ADCRES_10B
					number = number << 6;
					#endif // ADCRES_10B
					f_number = ADCRANGEV * number / 65536;
					dacq_csv_set_format((uint8_t *)format, (uint8_t *)"%5.4f");
				break;

				case AIN2:
					number = *(p_ainputs + 1);
					#ifdef ADCRES_16B
					#endif // ADCRES_16B
					#ifdef ADCRES_14B
					number = number << 2;
					#endif // ADCRES_14B
					#ifdef ADCRES_12B
					number = number << 4;
					#endif // ADCRES_12B
					#ifdef ADCRES_10B
					number = number << 6;
					#endif // ADCRES_10B
					f_number = ADCRANGEV * number / 65536;
					dacq_csv_set_format((uint8_t *)format, (uint8_t *)"%5.4f");
				break;

				case AIN3:
					number = *(p_ainputs + 2);
					#ifdef ADCRES_16B
					#endif // ADCRES_16B
					#ifdef ADCRES_14B
					number = number << 2;
					#endif // ADCRES_14B
					#ifdef ADCRES_12B
					number = number << 4;
					#endif // ADCRES_12B
					#ifdef ADCRES_10B
					number = number << 6;
					#endif // ADCRES_10B
					f_number = ADCRANGEV * number / 65536;
					dacq_csv_set_format((uint8_t *)format, (uint8_t *)"%5.4f");
				break;

				case ACCX:
					number = (int16_t)*p_serials;
					// prevod na normalizovany rozmer
//					#ifdef ACC_RANGE_2G
//					f_number = number / 32768.0f;
//					#endif // ACC_RANGE_2G
//					#ifdef ACC_RANGE_4G
//					f_number = number / 8192.0f;
//					#endif // ACC_RANGE_4G
//					#ifdef ACC_RANGE_8G
//					f_number = number / 4096.0f;
//					#endif // ACC_RANGE_8G
//					#ifdef ACC_RANGE_16G
//					f_number = number / 2048.0f;
//					#endif // ACC_RANGE_16G
					f_number = number / 32768.0f;
					p_serials++;
					dacq_csv_set_format((uint8_t *)format, (uint8_t *)"%7.6f");
				break;

				case ACCY:
					number = (int16_t)*p_serials;
					// prevod na normalizovany rozmer
//					#ifdef ACC_RANGE_2G
//					f_number = number / 32768.0f;
//					#endif // ACC_RANGE_2G
//					#ifdef ACC_RANGE_4G
//					f_number = number / 8192.0f;
//					#endif // ACC_RANGE_4G
//					#ifdef ACC_RANGE_8G
//					f_number = number / 4096.0f;
//					#endif // ACC_RANGE_8G
//					#ifdef ACC_RANGE_16G
//					f_number = number / 2048.0f;
//					#endif // ACC_RANGE_16G
					f_number = number / 32768.0f;
					p_serials++;
					dacq_csv_set_format((uint8_t *)format, (uint8_t *)"%7.6f");
				break;

				case ACCZ:
					number = (int16_t)*p_serials;
					// prevod na normalizovany rozmer
//					#ifdef ACC_RANGE_2G
//					f_number = number / 16384.0f;
//					#endif // ACC_RANGE_2G
//					#ifdef ACC_RANGE_4G
//					f_number = number / 8192.0f;
//					#endif // ACC_RANGE_4G
//					#ifdef ACC_RANGE_8G
//					f_number = number / 4096.0f;
//					#endif // ACC_RANGE_8G
//					#ifdef ACC_RANGE_16G
//					f_number = number / 2048.0f;
//					#endif // ACC_RANGE_16G
					f_number = number / 32768.0f;
					p_serials++;
					dacq_csv_set_format((uint8_t *)format, (uint8_t *)"%7.6f");
				break;

				case GYROX:
					number = (int16_t)*p_serials;
					// prevod na normalizovany rozmer
					#ifdef GYRO_RANGE_250
					//f_number = number / 131.0f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_250
					#ifdef GYRO_RANGE_500
					//f_number = number / 65.5f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_500
					#ifdef GYRO_RANGE_1000
					//f_number = number / 32.8f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_1000
					#ifdef GYRO_RANGE_2000
					//f_number = number / 16.4f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_2000
					p_serials++;
					dacq_csv_set_format((uint8_t *)format, (uint8_t *)"%6.5f");
				break;

				case GYROY:
					number = (int16_t)*p_serials;
					// prevod na normalizovany rozmer
					#ifdef GYRO_RANGE_250
					//f_number = number / 131.0f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_250
					#ifdef GYRO_RANGE_500
					//f_number = number / 65.5f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_500
					#ifdef GYRO_RANGE_1000
					//f_number = number / 32.8f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_1000
					#ifdef GYRO_RANGE_2000
					//f_number = number / 16.4f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_2000
					p_serials++;
					dacq_csv_set_format((uint8_t *)format, (uint8_t *)"%6.5f");
				break;

				case GYROZ:
					number = (int16_t)*p_serials;
					// prevod na normalizovany rozmer
					#ifdef GYRO_RANGE_250
					//f_number = number / 131.0f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_250
					#ifdef GYRO_RANGE_500
					//f_number = number / 65.5f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_500
					#ifdef GYRO_RANGE_1000
					//f_number = number / 32.8f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_1000
					#ifdef GYRO_RANGE_2000
					//f_number = number / 16.4f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_2000
					p_serials++;
					dacq_csv_set_format((uint8_t *)format, (uint8_t *)"%6.5f");
				break;

				case MAGX:
					number = (int16_t)*p_serials;
					// prevod na normalizovany rozmer
					f_number = number * 0.15f / 4912;
					p_serials++;
					dacq_csv_set_format((uint8_t *)format, (uint8_t *)"%6.5f");
				break;

				case MAGY:
					number = (int16_t)*p_serials;
					// prevod na normalizovany rozmer
					f_number = number * 0.15f / 4912;
					p_serials++;
					dacq_csv_set_format((uint8_t *)format, (uint8_t *)"%6.5f");
				break;

				case MAGZ:
					number = (int16_t)*p_serials;
					// prevod na normalizovany rozmer
					f_number = number * 0.15f / 4912;
					p_serials++;
					dacq_csv_set_format((uint8_t *)format, (uint8_t *)"%6.5f");
				break;

				case TEMP:
					number = (int16_t)*p_serials;
					f_number = number * 1.0f / 32768;
					p_serials++;
					dacq_csv_set_format((uint8_t *)format, (uint8_t *)"%4.3f");
				break;

				case TIME:
					f_timestamp += (1000.0f/datacq.freq);
					f_number = f_timestamp;
					dacq_csv_set_format((uint8_t *)format, (uint8_t *)"%9.3f");
				break;

				default:
					number = 0;
				break;
			}
			part = vspfunc(buffer_index, (char *)format, f_number);
			if (part != 0)
			{
				// doslo k uspesnemu prevodu
				buffer_index += part;

				if (column < (datacq.num_of_columns - 1))
				{
					// zakonci carkou a mezerou
					vspfunc(0, "%s, ", buffer);
					buffer_index += 2;
				}
//				else
//				{
//					// zakonci strednikem
//					vspfunc(0, "%s;", buffer);
//				}
			}
		}
		uart1_send_message((uint8_t *)buffer, strlen((const char *)buffer));
		p_ainputs += NUMBER_OF_ADC_CHANNELS;
	}

	return RETURN_OK;
}

RETURN_STATUS dacq_call_classifier(uint32_t lines, uint32_t offset)
{
	// podle velicin ve sloupcich vypisuje vsechny dane hodnoty
	// id pro ADC data je nutne vycist ze zaznamu pro dany sloupec, pak podle nej adresu souboru z alokacni tabulky
	// id pro serial data je nutne vycist ze zaznamu pro dany sloupec, pak podle nej adresu souboru z alokacni tabulky
	uint32_t adc_id = 0;
	uint32_t adc_base_address = 0;
	uint32_t serial_id = 0;
	uint32_t serial_base_address = 0;
	uint8_t i;

	// ukladani do noveho souboru
	uint32_t size = (sizeof(float) * datacq.num_of_columns *lines);
	if (sdramfs_allocate_new_file(size, (sdramfs_record_t *)&classifier_file_record) != RETURN_OK)
	{
		Error_Handler();
	}
	// ve file_record je nyni id souboru a adresa, kde soubor zacina

	// edituj zaznam souboru pro data
	if (sdramfs_edit_file_header((dataacq_setup_t *)&datacq, classifier_file_record.id) != RETURN_OK)
	{
		Error_Handler();
	}
	volatile int16_t *p_value = (int16_t *)classifier_file_record.address;
//	volatile float *p_value = (float *)classifier_file_record.address;

	// najdi zacatek ADC souboru, pokud je pouzivan
	if (dacq_number_of_adc_channels() != 0)
	{
		for (i = 0; i < MAX_NUM_OF_COLUMNS; i++)
		{
			if ((datacq.quantity_in_column[i] > AINFIRST) && (datacq.quantity_in_column[i] < AINLAST))
			{
				adc_id = datacq.file_for_column[i];
				break;
			}
		}
		// nastav adresu zacatku souboru a pripocti offset
		adc_base_address = sdramfs_get_address_from_id(adc_id) + (offset * datacq.num_of_columns * sizeof(uint16_t));
		if (adc_base_address == 0)
		{
			return RETURN_ERROR;
		}
	}
	// najdi zacatek serial souboru, pokud je pouzivan
	if (dacq_number_of_serial_channels() != 0)
	{
		for (i = 0; i < MAX_NUM_OF_COLUMNS; i++)
		{
			if ((datacq.quantity_in_column[i] > SERIALFIRST) && (datacq.quantity_in_column[i] < SERIALLAST))
			{
				serial_id = datacq.file_for_column[i];
				break;
			}
		}
		// nastav adresu zacatku souboru a pripocti offset
		serial_base_address = sdramfs_get_address_from_id(serial_id) + (offset * datacq.num_of_columns * sizeof(uint16_t));
		if (serial_base_address == 0)
		{
			return RETURN_ERROR;
		}
	}

	volatile uint16_t *p_ainputs = (uint16_t *)adc_base_address;
	volatile uint16_t *p_serials = (uint16_t *)serial_base_address;

	volatile uint8_t column;
	volatile int32_t number;
	volatile float f_number;
	volatile float f_timestamp = 0.0f;

	// projdi pres vsechny radky
	while (lines--)
	{
		// projdi pres vsechny sloupce
		for (column = 0; column < datacq.num_of_columns; column++)
		{
			switch(datacq.quantity_in_column[column])
			{
				case AIN1:
					number = *p_ainputs;
					// prevod na normalizovany rozmer
					#ifdef ADCRES_16B
					#endif // ADCRES_16B
					#ifdef ADCRES_14B
					number = number << 2;
					#endif // ADCRES_14B
					#ifdef ADCRES_12B
					number = number << 4;
					#endif // ADCRES_12B
					#ifdef ADCRES_10B
					number = number << 6;
					#endif // ADCRES_10B
					f_number = ADCRANGEV * number / 65536;
				break;

				case AIN2:
					number = *(p_ainputs + 1);
					// prevod na normalizovany rozmer
					#ifdef ADCRES_16B
					#endif // ADCRES_16B
					#ifdef ADCRES_14B
					number = number << 2;
					#endif // ADCRES_14B
					#ifdef ADCRES_12B
					number = number << 4;
					#endif // ADCRES_12B
					#ifdef ADCRES_10B
					number = number << 6;
					#endif // ADCRES_10B
					f_number = ADCRANGEV * number / 65536;
				break;

				case AIN3:
					number = *(p_ainputs + 2);
					// prevod na normalizovany rozmer
					#ifdef ADCRES_16B
					#endif // ADCRES_16B
					#ifdef ADCRES_14B
					number = number << 2;
					#endif // ADCRES_14B
					#ifdef ADCRES_12B
					number = number << 4;
					#endif // ADCRES_12B
					#ifdef ADCRES_10B
					number = number << 6;
					#endif // ADCRES_10B
					f_number = ADCRANGEV * number / 65536;
				break;

				case ACCX:
					number = (int16_t)*p_serials;
					// prevod na normalizovany rozmer
//					#ifdef ACC_RANGE_2G
//					f_number = number / 16384.0f;
//					#endif // ACC_RANGE_2G
//					#ifdef ACC_RANGE_4G
//					f_number = number / 8192.0f;
//					#endif // ACC_RANGE_4G
//					#ifdef ACC_RANGE_8G
//					f_number = number / 4096.0f;
//					#endif // ACC_RANGE_8G
//					#ifdef ACC_RANGE_16G
//					f_number = number / 2048.0f;
//					#endif // ACC_RANGE_16G
					f_number = number / 32768.0f;
					p_serials++;
				break;

				case ACCY:
					number = (int16_t)*p_serials;
					// prevod na normalizovany rozmer
//					#ifdef ACC_RANGE_2G
//					f_number = number / 16384.0f;
//					#endif // ACC_RANGE_2G
//					#ifdef ACC_RANGE_4G
//					f_number = number / 8192.0f;
//					#endif // ACC_RANGE_4G
//					#ifdef ACC_RANGE_8G
//					f_number = number / 4096.0f;
//					#endif // ACC_RANGE_8G
//					#ifdef ACC_RANGE_16G
//					f_number = number / 2048.0f;
//					#endif // ACC_RANGE_16G
					f_number = number / 32768.0f;
					p_serials++;
				break;

				case ACCZ:
					number = (int16_t)*p_serials;
					// prevod na normalizovany rozmer
//					#ifdef ACC_RANGE_2G
//					f_number = number / 16384.0f;
//					#endif // ACC_RANGE_2G
//					#ifdef ACC_RANGE_4G
//					f_number = number / 8192.0f;
//					#endif // ACC_RANGE_4G
//					#ifdef ACC_RANGE_8G
//					f_number = number / 4096.0f;
//					#endif // ACC_RANGE_8G
//					#ifdef ACC_RANGE_16G
//					f_number = number / 2048.0f;
//					#endif // ACC_RANGE_16G
					f_number = number / 32768.0f;
					p_serials++;
				break;

				case GYROX:
					number = (int16_t)*p_serials;
					// prevod na normalizovany rozmer
					#ifdef GYRO_RANGE_250
					//f_number = number / 131.0f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_250
					#ifdef GYRO_RANGE_500
					//f_number = number / 65.5f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_500
					#ifdef GYRO_RANGE_1000
					//f_number = number / 32.8f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_1000
					#ifdef GYRO_RANGE_2000
					//f_number = number / 16.4f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_2000
					p_serials++;
				break;

				case GYROY:
					number = (int16_t)*p_serials;
					// prevod na normalizovany rozmer
					#ifdef GYRO_RANGE_250
					//f_number = number / 131.0f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_250
					#ifdef GYRO_RANGE_500
					//f_number = number / 65.5f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_500
					#ifdef GYRO_RANGE_1000
					//f_number = number / 32.8f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_1000
					#ifdef GYRO_RANGE_2000
					//f_number = number / 16.4f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_2000
					p_serials++;
				break;

				case GYROZ:
					number = (int16_t)*p_serials;
					// prevod na normalizovany rozmer
					#ifdef GYRO_RANGE_250
					//f_number = number / 131.0f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_250
					#ifdef GYRO_RANGE_500
					//f_number = number / 65.5f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_500
					#ifdef GYRO_RANGE_1000
					//f_number = number / 32.8f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_1000
					#ifdef GYRO_RANGE_2000
					//f_number = number / 16.4f;
					f_number = number / 32768.0f;
					#endif // GYRO_RANGE_2000
					p_serials++;
				break;

				case MAGX:
					number = (int16_t)*p_serials;
					// prevod na normalizovany rozmer
					f_number = number * 0.15f / 4912;
					p_serials++;
				break;

				case MAGY:
					number = (int16_t)*p_serials;
					// prevod na normalizovany rozmer
					f_number = number * 0.15f / 4912;
					p_serials++;
				break;

				case MAGZ:
					number = (int16_t)*p_serials;
					// prevod na normalizovany rozmer
					f_number = number * 0.15f / 4912;
					p_serials++;
				break;

				case TEMP:
					number = (int16_t)*p_serials;
					f_number = number * 1.0f / 32768;
					p_serials++;
				break;

				case TIME:
					f_timestamp += (1000.0f/datacq.freq);
					f_number = f_timestamp;
				break;

				default:
					number = 0;
				break;
			}
//			*p_value = f_number;	// float
			*p_value = number;		// i16
			p_value++;
		}
		p_ainputs += NUMBER_OF_ADC_CHANNELS;
	}

	// soubor je pripraveny ke zpracovani, posli OpenAMP zpravu s identifikatorem souboru a prikazem ke zpracovani dat
	volatile msg_t message;
	message.header.cmd = MSG_CLASS001_MSG;
	message.header.length = sizeof(classifier_file_record);
	memcpy((void *)&message.data, (uint8_t *)&classifier_file_record, sizeof(sdramfs_record_t));

	OPENAMP_send((struct rpmsg_endpoint *)&rp_endpoint, (const void *)&message, sizeof(msg_header_t) + message.header.length);

	// soubor je potreba vymazat po prijeti zpravy s vysledky classifieru
	return RETURN_OK;
}

uint32_t vspfunc(uint16_t index, char *frmt, ...)
{
   va_list aptr;
   uint32_t ret;

   va_start(aptr, frmt);
   ret = vsprintf((char *)&buffer[index], frmt, aptr);
   va_end(aptr);

   return(ret);
}

RETURN_STATUS dacq_set_colquantity(uint8_t column, dataacq_quantity_t quantity)
{
	datacq.quantity_in_column[column] = quantity;

	return RETURN_OK;
}

RETURN_STATUS dacq_set_colfile(uint8_t column, uint32_t file)
{
	datacq.file_for_column[column] = file;

	return RETURN_OK;
}

uint32_t dacq_adc_file_size(void)
{
	uint32_t size;

	// spocitej pro pocet vsech ADC kanalu
	size = NUMBER_OF_ADC_CHANNELS * dacq_calculate_dma_size(); // kazda zapocata cast sekundy se zapocitana jako cela sekunda - efektivni ale rychle

	return size;
}


uint32_t dacq_serial_file_size(void)
{
	uint32_t size;

	// spocitej pro pocet vsech seriovych kanalu
	size = dacq_number_of_serial_channels() * dacq_calculate_dma_size(); // kazda zapocata cast sekundy se zapocitana jako cela sekunda - efektivni ale rychle

	return size;
}

uint32_t dacq_calculate_dma_size(void)
{
	uint32_t size;

	// spocitej pro pocet vsech ADC kanalu
	size = sizeof(uint16_t) * datacq.freq * (datacq.period/1000 + 1 + 2);
	// kazda zapocata cast sekundy se zapocitana jako cela sekunda - neefektivni ale rychle
	// dale +2 pro udalost ihned po odstartovani casovace a po skonceni, kdy casovac jeste dojede

	return size;
}

RETURN_STATUS dacq_add_id_to_adc_columns(uint32_t id)
{
	uint8_t i;

	// projdi vsechny sloupce a pokud je v nich ADC pozadavek, zapis k tomuto soupci id souboru
	for (i = 0; i < MAX_NUM_OF_COLUMNS; i++)
	{
		if ((datacq.quantity_in_column[i] > AINFIRST) && (datacq.quantity_in_column[i] < AINLAST))
		{
			datacq.file_for_column[i] = id;
		}
	}

	return RETURN_OK;
}

RETURN_STATUS dacq_add_id_to_serial_columns(uint32_t id)
{
	uint8_t i;

	// projdi vsechny sloupce a pokud je v nich serial pozadavek, zapis k tomuto soupci id souboru
	for (i = 0; i < MAX_NUM_OF_COLUMNS; i++)
	{
		if ((datacq.quantity_in_column[i] > SERIALFIRST) && (datacq.quantity_in_column[i] < SERIALLAST))
		{
			datacq.file_for_column[i] = id;
		}
	}

	return RETURN_OK;
}

uint8_t dacq_number_of_serial_channels(void)
{
	uint8_t i;
	uint8_t num = 0;

	// projdi vsechny sloupce a pokud je v nich serial pozadavek, zapis k tomuto soupci id souboru
	for (i = 0; i < MAX_NUM_OF_COLUMNS; i++)
	{
		if ((datacq.quantity_in_column[i] > SERIALFIRST) && (datacq.quantity_in_column[i] < SERIALLAST))
		{
			num++;
		}
	}

	return num;
}

uint8_t dacq_number_of_adc_channels(void)
{
	uint8_t i;
	uint8_t num = 0;

	// projdi vsechny sloupce a pokud je v nich ADC pozadavek, zapis k tomuto soupci id souboru
	for (i = 0; i < MAX_NUM_OF_COLUMNS; i++)
	{
		if ((datacq.quantity_in_column[i] > AINFIRST) && (datacq.quantity_in_column[i] < AINLAST))
		{
			num++;
		}
	}

	return num;
}

RETURN_STATUS dacq_enable_serial(uint32_t address)
{
	dacq_set_serial_setup(SERIAL_ACQ_ENABLED);

	return RETURN_OK;
}

RETURN_STATUS dacq_set_serial_setup(dataacq_quantity_t mode)
{
	datacq.serialsetup = mode;

	return RETURN_OK;
}

dataacq_quantity_t dacq_get_serial_setup(void)
{
	return datacq.serialsetup;
}

RETURN_STATUS dacq_read_serial_channels(void)
{
	// zde probiha cteni dat ze seriovych kanalu
	// napr. z mpu-9250.c jsou ziskana data pomoci uint8_t *mpu_get_data_buffer(void)
	if (mpu_read_accgyro() == RETURN_OK)
	{
		accgyro_data_t *p_values = (accgyro_data_t *)mpu_get_data();
		if (datacq_serial_data != NULL)
		{
			// proved vyber dle pozadovanych velicin
			// projdi pres vsechny sloupce
			uint8_t column;
			uint8_t offset = 0;
			for (column = 0; column < datacq.num_of_columns; column++)
			{
				switch(datacq.quantity_in_column[column])
				{
					case ACCX:
						*(uint16_t *)datacq_serial_data = (uint16_t)p_values->accx;
						datacq_serial_data += sizeof(uint16_t);
					break;

					case ACCY:
						*(uint16_t *)datacq_serial_data = (uint16_t)p_values->accy;
						datacq_serial_data += sizeof(uint16_t);
					break;

					case ACCZ:
						*(uint16_t *)datacq_serial_data = (uint16_t)p_values->accz;
						datacq_serial_data += sizeof(uint16_t);
					break;

					case GYROX:
						*(uint16_t *)datacq_serial_data = (uint16_t)p_values->gyrox;
						datacq_serial_data += sizeof(uint16_t);
					break;

					case GYROY:
						*(uint16_t *)datacq_serial_data = (uint16_t)p_values->gyroy;
						datacq_serial_data += sizeof(uint16_t);
					break;

					case GYROZ:
						*(uint16_t *)datacq_serial_data = (uint16_t)p_values->gyroz;
						datacq_serial_data += sizeof(uint16_t);
					break;

					case TEMP:
						*(uint16_t *)datacq_serial_data = (uint16_t)p_values->temp;
						datacq_serial_data += sizeof(uint16_t);
					break;

					case MAGX:
						*(uint16_t *)datacq_serial_data = (uint16_t)p_values->magx;
						datacq_serial_data += sizeof(uint16_t);
					break;

					case MAGY:
						*(uint16_t *)datacq_serial_data = (uint16_t)p_values->magy;
						datacq_serial_data += sizeof(uint16_t);
					break;

					case MAGZ:
						*(uint16_t *)datacq_serial_data = (uint16_t)p_values->magz;
						datacq_serial_data += sizeof(uint16_t);
					break;

					default:

					break;
				}
			}
		}
	}

	return RETURN_OK;
}

RETURN_STATUS dacq_cancel_progressbar(void)
{
	if (progressbar_id != 0)
	{
		Scheduler_Delete_Task(progressbar_id);
		progressbar_id = 0;
	}
	return RETURN_OK;
}

RETURN_STATUS dacq_increment_line_numbers(void)
{
	linenumbers++;

	return RETURN_OK;
}

RETURN_STATUS dacq_delall(void)
{
	sdramfs_init();

	uint8_t i;
	for (i = 0; i < MAX_NUM_OF_COLUMNS; i++)
	{
		datacq.file_for_column[i] = 0;
	}
	return RETURN_OK;
}

RETURN_STATUS dacq_logging_finished_message(void)
{
	// vypis zpravu
	const uint8_t text[] = "\r\nData logging finished, ";
	uart1_send_message((uint8_t *)text, strlen((const char *)text));

	// vypis pocet nabranych radku
	vspfunc(0, "%d", linenumbers);
	vspfunc(0, "%s lines logged\r\n", buffer);
	uart1_send_message((uint8_t *)buffer, strlen((const char *)buffer));

	return RETURN_OK;
}

RETURN_STATUS dacq_stop_logging(void)
{
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

	// zhasni LEDD
	HAL_GPIO_WritePin(LEDD_GPIO_PORT, LEDD_PIN, GPIO_PIN_SET);

	return RETURN_OK;
}

RETURN_STATUS dacq_csv_set_format(uint8_t *frmt, uint8_t *pattern)
{
	memcpy(frmt, (char *)pattern, strlen((char *)pattern));

	return RETURN_OK;
}

RETURN_STATUS dacq_acquisition_ready(void)
{
	if ((datacq.freq != 0) && (datacq.period != 0))
	{
		return RETURN_OK;
	}
	else
	{
		return RETURN_ERROR;
	}
}


