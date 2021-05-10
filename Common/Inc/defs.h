/*
 * defs.h
 *
 *  Created on: Jan 1, 2015
 *      Author: Petr Dvorak
 */

#ifndef INCLUDES_DEFS_H_
#define INCLUDES_DEFS_H_

#include "sys/_stdint.h"

#define RESET 	0
#define SET 	!RESET
#define DISABLE 0
#define ENABLE 	!DISABLE
#define FALSE	0
#define TRUE	!FALSE
#define OK		RESET
#define NOK		!OK
#define LOW		0
#define HIGH	!LOW
#define OFF		0
#define ON		!OFF

typedef enum {
	RETURN_OK = 0,
	RETURN_ERROR,
} RETURN_STATUS;




#endif /* INCLUDES_DEFS_H_ */
