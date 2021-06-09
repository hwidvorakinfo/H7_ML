/*
 * commands.h
 *
 *  Created on: 7. 5. 2019
 *      Author: Petr Dvorak
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx_hal.h"
#include "defs.h"
#include "main.h"
#include "uart_hal_cm4.h"

typedef enum {
	COMMANDOK = 0,
	COMMANDWRONG,
	COMMANDBLANK
} COMMAND_STATUS;

typedef struct {
	uint8_t *command;
	COMMAND_STATUS (*p_itemfunc)(void *);
	uint8_t enabled;
} command_t;

void commands_process(void);
void commands_wrong_cmd(void);
void commands_ok_cmd(void);
uint8_t cmd_isnumber(uint8_t *character);

typedef uint8_t CMD_RETURN;

// parsovaci prikazy
COMMAND_STATUS cmd_reset(void *p_i);
COMMAND_STATUS cmd_stime(void *p_i);
COMMAND_STATUS cmd_column(void *p_i);
COMMAND_STATUS cmd_colnum(void *p_i);
COMMAND_STATUS cmd_time(void *p_i);
COMMAND_STATUS cmd_start(void *p_i);
COMMAND_STATUS cmd_stop(void *p_i);
COMMAND_STATUS cmd_export(void *p_i);
COMMAND_STATUS cmd_delall(void *p_i);
COMMAND_STATUS cmd_classen(void *p_i);
COMMAND_STATUS cmd_auto(void *p_i);

// tela prikazu
CMD_RETURN command_reset(void *p_i);
CMD_RETURN command_stime(void *p_i);
CMD_RETURN command_col(void *p_i);
CMD_RETURN command_colnum(void *p_i);
CMD_RETURN command_time(void *p_i);
CMD_RETURN command_start(void *p_i);
CMD_RETURN command_stop(void *p_i);
CMD_RETURN command_csv(void *p_i);
CMD_RETURN command_delall(void *p_i);
CMD_RETURN command_classen(void *p_i);
CMD_RETURN command_auto(void *p_i);

#ifdef __cplusplus
}
#endif

#endif /* COMMANDS_H_ */
