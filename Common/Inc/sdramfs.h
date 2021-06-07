/*
 * sdramfs.h
 *
 *  Created on: 25. 5. 2021
 *      Author: Petr Dvorak
 */

#ifndef COMMON_INC_SDRAMFS_H_
#define COMMON_INC_SDRAMFS_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx_hal.h"
#include "defs.h"
#include "main.h"
#include "application.h"
#include "dataacq_types.h"

typedef struct {
 	uint32_t id;
 	uint32_t address;
} sdramfs_record_t;

typedef struct {
 	uint32_t id;
 	uint32_t address;
 	uint32_t size;
 	dataacq_setup_t header;
} sdramfs_file_t;

#define KB(x)   ((uint82_t) (x) << 10)
#define MB(x)   ((uint8_t) (x) << 20)

#define MAXNUMBEROFFILES							100
#define SDRAMFS_ALLOCATION_TABLE_START				0xD0000000
#define SDRAMFS_SIZE								(MB(10))
#define SDRAMFS_ALLOCATION_TABLE_SIZE				(sizeof(sdramfs_file_t) * MAXNUMBEROFFILES)
#define SDRAMFS_FILE_AREA_START						(SDRAMFS_ALLOCATION_TABLE_START + SDRAMFS_ALLOCATION_TABLE_SIZE)
#define SDRAMFS_FILE_AREA_SIZE						(SDRAMFS_SIZE - (SDRAMFS_FILE_AREA_START - SDRAMFS_ALLOCATION_TABLE_START))

RETURN_STATUS sdramfs_init(void);
RETURN_STATUS sdramfs_allocate_new_file(uint32_t size, sdramfs_record_t *record);
RETURN_STATUS sdramfs_edit_file_header(dataacq_setup_t *header, uint32_t id);
RETURN_STATUS sdramfs_delete_file(uint32_t id);
uint32_t sdramfs_get_address_from_id(uint32_t id);
uint32_t sdramfs_get_file_header_address(uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* COMMON_INC_SDRAMFS_H_ */
