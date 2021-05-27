/**
  ******************************************************************************
  * @file    GPIO/GPIO_EXTI/CM4/Src/stm32h7xx_it.c
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines for Cortex-M4.
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
#include "stm32h7xx_it.h"
#include "beasth7.h"
#include "scheduler.h"
#include "application.h"
#include "uart_hal_cm4.h"
#include "adc_hal_cm4.h"
#include "mikrobus.h"
#include "dataacq.h"

/** @addtogroup STM32H7xx_HAL_Examples
  * @{
  */

/** @addtogroup GPIO_EXTI
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();

  Run_scheduler();
}

/******************************************************************************/
/*                 STM32H7xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32h7xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles external lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */
//void EXTI15_10_IRQHandler(void)
//{
//  HAL_GPIO_EXTI_IRQHandler(BUTTON_TAMPER_PIN);
//}

/**
  * @brief  This function handles CM7 Send event interrupt for CM4 interrupt request.
  * @param  None
  * @retval None
  */
//void CM7_SEV_IRQHandler(void)
//{
//
//}

/**
  * @brief  This function handles external line 2 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI2_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(BUTTON1_PIN);
}

/**
  * @brief  This function handles external line 3 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI3_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(BUTTON2_PIN);
}

/**
  * @brief  This function handles HSEM2 interrupt request.
  * @param  None
  * @retval None
  */
void HSEM2_IRQHandler(void)
{
  HAL_HSEM_IRQHandler();

}

/**
  * @brief  This function handles UART interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA stream
  *         used for USART data transmission
  */
void USARTUSB_IRQHandler(void)
{
  HAL_UART_IRQHandler(&Uart1Handle);
}

/**
  * @brief  This function handles UART interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA stream
  *         used for USART data transmission
  */
void USARTMIKROE_IRQHandler(void)
{
  HAL_UART_IRQHandler(&Uart2Handle);
}

/**
  * @brief  This function handles I2C event interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to I2C data transmission
  */
void I2CMIKROE_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&I2c4Handle);
}

/**
  * @brief  This function handles I2C error interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to I2C error
  */
void I2CMIKROE_ER_IRQHandler(void)
{
	HAL_I2C_ER_IRQHandler(&I2c4Handle);
}

/**
* @brief  This function handles DMA1_Stream1_IRQHandler interrupt request.
* @param  None
* @retval None
*/
void DMA1_Stream1_IRQHandler(void)
{
	HAL_DMA_IRQHandler(AdcHandle.DMA_Handle);
}

/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */
void TIM_ADC_SYNC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TimHandle_adc_sync);

	// obsluha vycitani seriovych dat
	if (dacq_get_serial_setup() == (dataacq_quantity_t)SERIAL_ACQ_ENABLED)
	{
		dacq_read_serial_channels();
	}

	// zvys pocitadlo nabranych radku
	dacq_increment_line_numbers();

	HAL_GPIO_TogglePin(LEDD_GPIO_PORT, LEDD_PIN);
}

/**
  * @brief  This function handles SPI interrupt request.
  * @param  None
  * @retval None
  */
void SPIMIKROE_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&Spi1Handle);
}

/**
  * @brief  This function handles external lines 1 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI1_IRQHandler(void)
{
	mikrobus_int_handler();
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
