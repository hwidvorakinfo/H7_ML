/*
 * commands.c
 *
 *  Created on: 7. 5. 2019
 *      Author: Petr Dvorak
 */

#include "commands.h"
#include "string.h"
#include "application.h"
#include "errorcodes.h"
#include "services.h"
#include "dataacq.h"
#include "adc_hal_cm4.h"

// prikazy
static const uint8_t COMMAND_BLANK[] = "";
static command_t command_blank = {(uint8_t *)&COMMAND_BLANK, NULL, TRUE};			// prazdny prikaz, pouze enter

static const uint8_t COMMAND_REST[] = "RESET";
static command_t cmd_rest = {(uint8_t *)&COMMAND_REST, &cmd_reset, TRUE};			// reset

static const uint8_t COMMAND_STIME[] = "ST****";
static command_t cmd_st = {(uint8_t *)&COMMAND_STIME, &cmd_stime, TRUE};			// sampling time

static const uint8_t COMMAND_COL[] = "C*****";
static command_t cmd_col = {(uint8_t *)&COMMAND_COL, &cmd_column, TRUE};			// prirad velicinu danemu sloupci

static const uint8_t COMMAND_COLN[] = "CN*";
static command_t cmd_coln = {(uint8_t *)&COMMAND_COLN, &cmd_colnum, TRUE};			// nastav pocet vypisovanych sloupcu

static const uint8_t COMMAND_START[] = "START";
static command_t cmd_strt = {(uint8_t *)&COMMAND_START, &cmd_start, TRUE};			// zacatek mereni

static const uint8_t COMMAND_STOP[] = "STOP";
static command_t cmd_stp = {(uint8_t *)&COMMAND_STOP, &cmd_stop, TRUE};			// konec mereni


COMMAND_STATUS commands_parse(uint8_t *pattern, uint8_t *command);

void commands_process(void)
{
	COMMAND_STATUS (*p_func)();

	// parsing prikazu Reset request
	if (commands_parse((uint8_t *)&COMMAND_REST, usart_get_rx_buffer()) == COMMANDOK)
	{
		if (cmd_rest.enabled)
		{
			// zavolani obsluzne funkce prikazu
			p_func = cmd_rest.p_itemfunc;
			p_func(usart_get_rx_buffer());
		}
		return;
	}
	// parsing prikazu Sampling time
	else if (commands_parse((uint8_t *)&COMMAND_STIME, usart_get_rx_buffer()) == COMMANDOK)
	{
		if (cmd_st.enabled)
		{
			// zavolani obsluzne funkce prikazu
			p_func = cmd_st.p_itemfunc;
			p_func(usart_get_rx_buffer());
		}
		return;
	}
	// parsing prikazu Column
	else if (commands_parse((uint8_t *)&COMMAND_COL, usart_get_rx_buffer()) == COMMANDOK)
	{
		if (cmd_col.enabled)
		{
			// zavolani obsluzne funkce prikazu
			p_func = cmd_col.p_itemfunc;
			p_func(usart_get_rx_buffer());
		}
		return;
	}
	// parsing prikazu Colnum
	else if (commands_parse((uint8_t *)&COMMAND_COLN, usart_get_rx_buffer()) == COMMANDOK)
	{
		if (cmd_coln.enabled)
		{
			// zavolani obsluzne funkce prikazu
			p_func = cmd_coln.p_itemfunc;
			p_func(usart_get_rx_buffer());
		}
		return;
	}
	// parsing prikazu Start
	else if (commands_parse((uint8_t *)&COMMAND_START, usart_get_rx_buffer()) == COMMANDOK)
	{
		if (cmd_strt.enabled)
		{
			// zavolani obsluzne funkce prikazu
			p_func = cmd_strt.p_itemfunc;
			p_func(usart_get_rx_buffer());
		}
		return;
	}
	// parsing prikazu Stop
	else if (commands_parse((uint8_t *)&COMMAND_STOP, usart_get_rx_buffer()) == COMMANDOK)
	{
		if (cmd_stp.enabled)
		{
			// zavolani obsluzne funkce prikazu
			p_func = cmd_stp.p_itemfunc;
			p_func(usart_get_rx_buffer());
		}
		return;
	}
	// parsing prazdneho prikazu
	else if (commands_parse(command_blank.command, usart_get_rx_buffer()) == COMMANDBLANK)
	{
		if (command_blank.enabled)
		{
			// zavolani obsluzne funkce prikazu
			commands_ok_cmd();
		}
		return;
	}
	else
	{
		// zbyva jiz jen posledni varianta - spatny prikaz
		commands_wrong_cmd();
	}
}

COMMAND_STATUS commands_parse(uint8_t *pattern, uint8_t *command)
{
	volatile uint8_t i = 0;
	volatile uint8_t retval = COMMANDWRONG;

	// test kvuli odstraneni testu prazdneho prikazu (enteru)
	if (command[0] != 0)
	{
		// prikaz neni prazdny
		//

		// test na odlisnou delku, pokud maji retezce odlisnou desku, rozhodne se lisi
		// textova verze
		if (strlen((const char *)pattern) != strlen((const char *)command))
		{
			retval = COMMANDWRONG;
			return retval;
		}

		// retezce maji stejnou delku
		//
		// porovnani prichoziho prikazu se vzorem. Hvezdicka je zastupny znak pro jedinou cifru cisla.
		// projdi vsechny znaky vzoru, maximum je delka bufferu, pokud by se cokoliv stalo pri testu delek vyse
		while ((pattern[i] != 0) && (i < RX1BUFFERSIZE))
		{
			if (pattern[i] == command[i])
			{
				// shoduji se
				//
				// pokracuj
				retval = COMMANDOK;
				i++;
				continue;
			}
			else
			{
				// jsou ruzne
				//
				// test na hvezdicku
				if (pattern[i] == '*')
				{
					retval = COMMANDOK;
					i++;
					continue;
				}
				else
				{
					// odlisuji se a znak neni hvezdicka, tedy se retezce lisi
					retval = COMMANDWRONG;
					return retval;
				}
			}
		}
	}
	else
	{
		// prazdny prikaz
		retval = COMMANDBLANK;
	}
	return retval;
}

void commands_wrong_cmd(void)
{
	uint8_t text[] = " ER";

	uart1_send_message((uint8_t *)&text, strlen((char *)text));
	uart1_new_line();
}

void commands_ok_cmd(void)
{
	uint8_t text[] = " OK";

	uart1_send_message((uint8_t *)&text, strlen((char *)text));
	uart1_new_line();
}

// handlery prikazu


// prikaz REST
COMMAND_STATUS cmd_reset(void *p_i)
{
	if (command_reset(p_i) == RETURN_ERROR)
	{
		return COMMANDWRONG;
	}
	return COMMANDOK;
}

uint8_t cmd_isnumber(uint8_t *character)
{
	if ((*character >= '0') && (*character <= '9'))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// tela prikazu

// restartuje aplikaci
CMD_RETURN command_reset(void *p_i)
{
	// zalozeni ulohy restartu aplikace
	if(Scheduler_Add_Task(Reset_service, RESET_SERVICE_PERIOD, 0) == SCH_MAX_TASKS)
	{
		// chyba pri zalozeni service
		return RETURN_ERROR;
	}
	else
	{
		return RETURN_OK;
	}

	commands_ok_cmd();
}

// prikaz Sampling time
COMMAND_STATUS cmd_stime(void *p_i)
{
	if (command_stime(p_i) == RETURN_ERROR)
	{
		return COMMANDWRONG;
	}
	return COMMANDOK;
}

CMD_RETURN command_stime(void *p_i)
{
	uint8_t *p_char = (uint8_t *)p_i;

	// prikaz ma tvar "STxxxx", kde xxxx je cas v ms od 1 do 9999
	uint8_t i;

#define ST_0XXX		(2)
#define ST_X0XX		(3)
#define ST_XX0X		(4)
#define ST_XXX0		(5)

	for (i = ST_0XXX; i <= ST_XXX0; i++)
	{
		if (!cmd_isnumber(&p_char[i]))
		{
			commands_wrong_cmd();
			return RETURN_ERROR;
		}
	}

	uint16_t time;
	time = 1000 * (p_char[ST_0XXX] - '0');
	time += 100 * (p_char[ST_X0XX] - '0');
	time += 10 * (p_char[ST_XX0X] - '0');
	time += (p_char[ST_XXX0] - '0');

	if (time == 0)
	{
		time = 1;
	}

	// uloz hodnotu do zaznamoveho systemu
	dacq_set_stime(time);

#undef ST_0XXX
#undef ST_X0XX
#undef ST_XX0X
#undef ST_XXX0

	commands_ok_cmd();
	return RETURN_OK;
}

// prikaz Column
COMMAND_STATUS cmd_column(void *p_i)
{
	if (command_col(p_i) == RETURN_ERROR)
	{
		return COMMANDWRONG;
	}
	return COMMANDOK;
}

CMD_RETURN command_col(void *p_i)
{
	uint8_t *p_char = (uint8_t *)p_i;

	// prikaz ma tvar "CNxxxx", kde N je cislo od 0 do 9, xxxx je TEXT
	uint8_t i;

#define COL_N		(1)
#define COL_0XXX	(2)
#define COL_X0XX	(3)
#define COL_XX0X	(4)
#define COL_XXX0	(5)

	// test na znak N od 0 do 9
	if (!cmd_isnumber(&p_char[COL_N]))
	{
		commands_wrong_cmd();
		return RETURN_ERROR;
	}

	// cislo sloupce
	uint8_t column = p_char[COL_N] - '0';

	// prevezmi operand
	uint8_t operand[5];
	for (i = 0; i < 4; i++)
	{
		operand[i] = p_char[COL_0XXX+i];
	}
	operand[4] = 0;

	// vzory
	const uint8_t ain1[] = "AIN1";
	const uint8_t ain2[] = "AIN2";
	const uint8_t ain3[] = "AIN3";
	const uint8_t accx[] = "ACCX";
	const uint8_t accy[] = "ACCY";
	const uint8_t accz[] = "ACCZ";

	if (strcmp((const char *)operand, (const char *)ain1) == 0)
	{
		// operand je AIN1
		daqc_set_colfunc(column, adc_get_ain1);

	}
	else if (strcmp((const char *)operand, (const char *)ain2) == 0)
	{
		// operand je AIN2
		daqc_set_colfunc(column, adc_get_ain2);
	}
	else if (strcmp((const char *)operand, (const char *)ain3) == 0)
	{
		// operand je AIN3
		daqc_set_colfunc(column, adc_get_ain3);
	}
	else if (strcmp((const char *)operand, (const char *)accx) == 0)
	{
		// operand je ACCX
	}
	else if (strcmp((const char *)operand, (const char *)accy) == 0)
	{
		// operand je ACCY
	}
	else if (strcmp((const char *)operand, (const char *)accz) == 0)
	{
		// operand je ACCZ
	}
	else
	{
		// neplatna kombinace
		commands_wrong_cmd();
		return RETURN_ERROR;
	}

#undef COL_N
#undef COL_0XXX
#undef COL_X0XX
#undef COL_XX0X
#undef COL_XXX0

	commands_ok_cmd();
	return RETURN_OK;
}

// prikaz Colnum
COMMAND_STATUS cmd_colnum(void *p_i)
{
	if (command_colnum(p_i) == RETURN_ERROR)
	{
		return COMMANDWRONG;
	}
	return COMMANDOK;
}

CMD_RETURN command_colnum(void *p_i)
{
	uint8_t *p_char = (uint8_t *)p_i;

	// prikaz ma tvar "CNx", kde x je cislo od 0 do 9
	uint8_t i;

#define COL_0		(2)

	// test na znak od 0 do 9
	if (!cmd_isnumber(&p_char[COL_0]))
	{
		commands_wrong_cmd();
		return RETURN_ERROR;
	}

	// nastav pocet vypisovanych sloupcu
	daqc_set_colnum(p_char[COL_0] - '0');

#undef COL_0

	commands_ok_cmd();
	return RETURN_OK;
}

// prikaz Start
COMMAND_STATUS cmd_start(void *p_i)
{
	if (command_start(p_i) == RETURN_ERROR)
	{
		return COMMANDWRONG;
	}
	return COMMANDOK;
}

CMD_RETURN command_start(void *p_i)
{
	if (dacq_start_acq() == RETURN_OK)
	{
		commands_ok_cmd();
		return RETURN_OK;
	}
	else
	{
		commands_wrong_cmd();
		return RETURN_ERROR;
	}



}

// prikaz Stop
COMMAND_STATUS cmd_stop(void *p_i)
{
	if (command_stop(p_i) == RETURN_ERROR)
	{
		return COMMANDWRONG;
	}
	return COMMANDOK;
}

CMD_RETURN command_stop(void *p_i)
{
	if (dacq_stop_acq() == RETURN_OK)
	{
		commands_ok_cmd();
		return RETURN_OK;
	}
	else
	{
		commands_wrong_cmd();
		return RETURN_ERROR;
	}



}

