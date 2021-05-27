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

static volatile dataacq_setup_t datacq;
static volatile uint8_t buffer[TX1BUFFERSIZE];
static volatile uint16_t buffer_index;
static uint8_t progressbar_id;
static uint32_t linenumbers = 0;

RETURN_STATUS dacq_add_id_to_adc_columns(uint32_t id);
RETURN_STATUS dacq_add_id_to_serial_columns(uint32_t id);
RETURN_STATUS dacq_enable_serial(uint32_t address);

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
	dacq_stop_acq();

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

		// odstartuj ADC sber
		if (adc_dma_start() != RETURN_OK)
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

		// edituj zaznam souboru pro serial data
		if (sdramfs_edit_file_header((dataacq_setup_t *)&datacq, serial_record.id) != RETURN_OK)
		{
			Error_Handler();
		}

		// odstartuj serial sber
		dacq_enable_serial(serial_record.address);
	}

	// zaloz novou sluzbu pro ukonceni zaznamu
	datacq.taskid = Scheduler_Add_Task(Datacq_service, datacq.period, 0);
	if (datacq.taskid == SCH_MAX_TASKS)
	{
		// chyba pri zalozeni service
		return RETURN_ERROR;
	}

	// zaloz novou sluzbu pro vypis prubehu mereni
	uint32_t pb_period = datacq.period/PROGRESSBARLENGTH;
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
	adc_dma_unconfig();

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
	volatile int16_t number;
	uint8_t text[] = "         ";

	// vytvor hlavicku zaznamu
	buffer[0] = '\r';
	buffer[1] = '\n';
	buffer_index = 2;
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
				memcpy(&text, (const char *)"ACCY", strlen("ACCZ"));
				text[strlen("ACCZ")] = 0;
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
			else
			{
				// zakonci strednikem
				vspfunc(0, "%s;", buffer);
			}
		}
	}
	uart1_send_message((uint8_t *)buffer, strlen((const char *)buffer));

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
				break;

				case AIN2:
					number = *(p_ainputs + 1);
				break;

				case AIN3:
					number = *(p_ainputs + 2);
				break;

				case ACCX:
					number = *p_serials;
					p_serials++;
				break;

				case ACCY:
					number = *p_serials;
					p_serials++;
				break;

				case ACCZ:
					number = *p_serials;
					p_serials++;
				break;

				default:
					number = 0;
				break;
			}
			part = vspfunc(buffer_index, "%d", number);
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
				else
				{
					// zakonci strednikem
					vspfunc(0, "%s;", buffer);
				}
			}
		}
		uart1_send_message((uint8_t *)buffer, strlen((const char *)buffer));
		p_ainputs += NUMBER_OF_ADC_CHANNELS;
	}

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
	size = sizeof(uint16_t) * datacq.freq * (datacq.period/1000 + 1); // kazda zapocata cast sekundy se zapocitana jako cela sekunda - efektivni ale rychle

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
