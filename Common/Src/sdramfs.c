/*
 * sdramfs.c
 *
 *  Created on: 25. 5. 2021
 *      Author: Petr Dvorak
 */

#include "main.h"
#include "application.h"
#include "defs.h"
#include "error_handler.h"
#include "string.h"
#include "sdramfs.h"

uint32_t sdramfs_find_max_id(void);


// inicializuj misto pro vsechny soubory na prazdne zaznamy - vymaz celou alokacni tabulku
RETURN_STATUS sdramfs_init(void)
{
	if (memset((uint32_t *)SDRAMFS_ALLOCATION_TABLE_START, 0, SDRAMFS_ALLOCATION_TABLE_SIZE) == (uint32_t *)SDRAMFS_ALLOCATION_TABLE_START)
	{
		return RETURN_OK;
	}
	else
	{
		return RETURN_ERROR;
	}

//	volatile uint16_t *p_word = (uint16_t *)SDRAMFS_FILE_AREA_START;
//	volatile uint32_t *p_double;
//	volatile uint8_t i;
//	for (i = 0; i < 5; i++)
//	{
//		*p_word++ = i;
//	}
//	p_double = (uint32_t *)p_word;
//	for (i = 0; i < 5; i++)
//	{
//		*p_double++ = i;
//	}
//	return RETURN_OK;
}

uint32_t sdramfs_find_max_id(void)
{
	// prohledej celou alokacni tabulku a vrat nejvyssi nalezene ID
	uint32_t id = 0;
	sdramfs_file_t *p_file = (sdramfs_file_t *)SDRAMFS_ALLOCATION_TABLE_START;

	uint8_t i;
	for (i = 0; i < MAXNUMBEROFFILES; i++)
	{
		if (p_file[i].id > id)
		{
			id = p_file[i].id;
		}
	}

	return id;
}

// najdi misto pro novy soubor, v pripade neuspechu vrat NULL
// v pripade uspechu vrat ID a adresu zacatku zapisu obsahu souboru
RETURN_STATUS sdramfs_allocate_new_file(uint32_t size, sdramfs_record_t *record)
{
	sdramfs_record_t *p_return = NULL;
	if (record != NULL)
	{
		p_return = record;
	}
	else
	{
		return RETURN_ERROR;
	}

	sdramfs_file_t *p_file = (sdramfs_file_t *)SDRAMFS_ALLOCATION_TABLE_START;
	uint32_t free_size = (uint32_t)SDRAMFS_FILE_AREA_SIZE;
	uint32_t address = SDRAMFS_FILE_AREA_START;
	uint8_t i;

	for (i = 0; i < MAXNUMBEROFFILES; i++)
	{
		// prohledej alokacni tabulku
		if (p_file[i].id == 0)
		{
			// nasel jsi volne misto pro soubor
			if (free_size >= size)
			{
				// pro soubor je volne misto
				p_return->id = sdramfs_find_max_id() + 1;
				p_return->address = address;

				// zapis novy zaznam do pameti
				p_file[i].id = p_return->id;
				p_file[i].address = address;
				p_file[i].size = size;

				// vymaz misto pro novy soubor
				memset((uint32_t *)address, 0, size);

				return RETURN_OK;
			}
		}
		else
		{
			// zaznam je obsazeny, posun adresu a vypocitej nove volne misto
			address += p_file[i].size;
			free_size -= p_file[i].size;
		}
	}
	return RETURN_ERROR;
}

// vlozi do alokacni tabulky informace ze vstupniho parametru
// v polozce id struktury vstupniho parametru je identifikator daneho zaznamu
RETURN_STATUS sdramfs_edit_file_header(dataacq_setup_t *header, uint32_t id)
{
	dataacq_setup_t *p_header = NULL;
	if (header != NULL)
	{
		p_header = header;
	}
	else
	{
		return RETURN_ERROR;
	}

	uint8_t i;
	sdramfs_file_t *p_file = (sdramfs_file_t *)SDRAMFS_ALLOCATION_TABLE_START;
	for (i = 0; i < MAXNUMBEROFFILES; i++)
	{
		if (p_file[i].id == id)
		{
			// nasel jsi dany zaznam
			if (memcpy(&p_file[i].header, p_header, sizeof(dataacq_setup_t)) == &p_file[i].header)
			{
				return RETURN_OK;
			}
			else
			{
				// nepovedlo se editovat
				return RETURN_ERROR;
			}
		}
	}
	return RETURN_ERROR;
}

// vymaze zaznam s danym id
RETURN_STATUS sdramfs_delete_file(uint32_t id)
{
	uint8_t i;
	sdramfs_file_t *p_file = (sdramfs_file_t *)SDRAMFS_ALLOCATION_TABLE_START;
	for (i = 0; i < MAXNUMBEROFFILES; i++)
	{
		if (p_file[i].id == id)
		{
			// nasel jsi dany zaznam, vymaz ho
			if (memset(&p_file[i], 0, sizeof(sdramfs_file_t)) == &p_file[i])
			{
				return RETURN_OK;
			}
			else
			{
				// nepovedlo se vymazat
				return RETURN_ERROR;
			}
		}
	}
	return RETURN_ERROR;
}

uint32_t sdramfs_get_address_from_id(uint32_t id)
{
	uint8_t i;
	sdramfs_file_t *p_file = (sdramfs_file_t *)SDRAMFS_ALLOCATION_TABLE_START;
	for (i = 0; i < MAXNUMBEROFFILES; i++)
	{
		if (p_file[i].id == id)
		{
			return p_file[i].address;
		}
	}
	return 0;
}
