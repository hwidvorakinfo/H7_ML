#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "stm32h7xx_hal.h"

#define SCH_MAX_TASKS		30
#define SCHEDULERPERIOD		1000

extern void Scheduler_Dispatch_Tasks(void);
extern void Scheduler_Dispatch_Task_with_index(uint8_t index);
extern unsigned char Scheduler_Add_Task(void (*pFunction)(void), const uint32_t DELAY, const uint32_t PERIOD);
extern unsigned char Scheduler_Delete_Task(const unsigned char TASK_INDEX);
extern void Scheduler_init(void);
extern void Scheduler_start(void);
extern void Run_scheduler(void);
extern void Scheduler_Refresh_task(const unsigned char TASK_INDEX);
uint8_t Scheduler_get_running_task(void);

// Total memory per task is 7 bytes
typedef struct
{
	void (*pTask)(void);						// Pointer to the task (must be a 'void (void)' function)
	uint32_t Delay;								// Delay (ticks) until the function will (next) be run
	uint32_t Period;							// Interval (ticks) between subsequent runs.
	uint8_t RunMe;								// Incremented (by scheduler) when task is due to execute
} sTask;


#endif
