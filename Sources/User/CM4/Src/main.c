/**
  ******************************************************************************
  * @file    GPIO/GPIO_EXTI/CM4/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to configure and use GPIOs through
  *          the STM32H7xx HAL API.
  *          This is the main program for Cortex-M4
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "beasth7.h"
#include "scheduler.h"
#include "services.h"
#include "mcuperipherals.h"
#include "msg_types.h"

/** @addtogroup STM32H7xx_HAL_Examples
  * @{
  */

/** @addtogroup GPIO_EXTI
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define RPMSG_SERVICE_NAME              "BH7_ML"
/* Private variables ---------------------------------------------------------*/
volatile msg_t tx_message;
volatile msg_t rx_message;
static volatile int16_t received_data;
static volatile uint16_t message_received;

volatile struct rpmsg_endpoint rp_endpoint;
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

HSEM_TypeDef * HSEM_DEBUG= HSEM;

static int rpmsg_recv_callback(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv)
{
//	received_data = *((unsigned int *) data);
//	message_received = 1;

	memcpy((void *)&rx_message, data, len);

	received_data = len;
	message_received = 1;

	return 0;
}

unsigned int receive_message(void)
{
	if (message_received == 0)
	{
		OPENAMP_check_for_message();
		return 0;
	}
	else
	{
		message_received = 0;
		return received_data;
	}
}


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	volatile int32_t status = 0;

	__HAL_RCC_HSEM_CLK_ENABLE();												// HW semaphore Clock enable
	HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));			// HW semaphore Notification enable

	HAL_PWREx_ClearPendingEvent();												// When system initialization is finished, Cortex-M7 will release Cortex-M4  by means of HSEM notification
	HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE, PWR_D2_DOMAIN);

	// Initialize HAL : systick
	if (HAL_Init() != HAL_OK)
	{
		Error_Handler();
	}

	HSEM_COMMON->ICR |= ((uint32_t)__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));		// Clear Flags generated during the wakeup notification
	HAL_NVIC_ClearPendingIRQ(HSEM2_IRQn);

	/* Add Cortex-M4 user application code here */
	Scheduler_init();															// inicializace kooperativniho scheduleru a jeho odstartovani
	Scheduler_start();															// nyni je jiz mozne zakladat tasky
	mcuperipherals_init();														// inicializace periferii MCU

	MAILBOX_Init();																// Inilitize the mailbox use notify the other core on new message
	if (MX_OPENAMP_Init(RPMSG_REMOTE, NULL) != HAL_OK)							// Inilitize OpenAmp and libmetal libraries as RPMSG_REMOTE
	{
		Error_Handler();
	}

	/* create a endpoint for rmpsg communication */
	status = OPENAMP_create_endpoint(&rp_endpoint, RPMSG_SERVICE_NAME, RPMSG_ADDR_ANY, rpmsg_recv_callback, NULL);
	if (status < 0)
	{
		Error_Handler();
	}

	if(Scheduler_Add_Task(Receive_service, 0, RECEIVE_SERVICE_PERIOD) == SCH_MAX_TASKS)
	{
		// chyba pri zalozeni service
		return RETURN_ERROR;
	}

	while(1)
	{
		Scheduler_Dispatch_Tasks();						// sprava tasku - spousti tasky, maze neplatne tasky
	}

	/* Deinitialize OpenAMP */
	OPENAMP_DeInit();

//  /* -1- Initialize LEDs mounted on the board */
//  LEDD_GPIO_CLK_ENABLE();
//  GPIO_InitTypeDef  gpio_init_structure;
//  gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
//  gpio_init_structure.Pull = GPIO_PULLUP;
//  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
//  gpio_init_structure.Pin = LEDD_PIN;
//  HAL_GPIO_Init(LEDD_GPIO_PORT, &gpio_init_structure);
//  HAL_GPIO_WritePin(LEDD_GPIO_PORT, LEDD_PIN, GPIO_PIN_RESET);
//
//  HAL_GPIO_TogglePin(LEDD_GPIO_PORT, LEDD_PIN);
//  HAL_GPIO_TogglePin(LEDD_GPIO_PORT, LEDD_PIN);
//  HAL_GPIO_TogglePin(LEDD_GPIO_PORT, LEDD_PIN);
//  HAL_GPIO_TogglePin(LEDD_GPIO_PORT, LEDD_PIN);
//
//  /* Enable and set EXTI lines 2 Interrupt to the lowest priority */
//  //HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
//  HAL_NVIC_SetPriority(BUTTON1_EXTI_IRQn, 2, 0);
//  HAL_NVIC_EnableIRQ(BUTTON1_EXTI_IRQn);
//
//  /* Enable and set EXTI lines 3 Interrupt to the lowest priority */
//  //HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
//  HAL_NVIC_SetPriority(BUTTON2_EXTI_IRQn, 2, 0);
//  HAL_NVIC_EnableIRQ(BUTTON2_EXTI_IRQn);
//
//  /* Infinite loop */
//  uint32_t time;
//  while (1)
//  {
//	  if (time++ == 20000000)
//	  {
//		  HAL_GPIO_TogglePin(LEDD_GPIO_PORT, LEDD_PIN);
//		  time = 0;
//
//		  //__HAL_GPIO_EXTI_GENERATE_SWIT(BUTTON2_PIN);
//	  }
//  }
}




/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{  
	if (GPIO_Pin == BUTTON1_PIN)
	{
		/* Toggle LEDD */
		HAL_GPIO_TogglePin(LEDD_GPIO_PORT, LEDD_PIN);
	}
}

/**
* @brief  This function is executed in case of error occurrence.
* @param  None
* @retval None
*/
void Error_Handler(void)
{
	/* User may add here some code to deal with this error */
	while(1)
	{
	}
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
