/**
  ******************************************************************************
  * @file    GPIO/GPIO_EXTI/CM7/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to configure and use GPIOs through
  *          the STM32H7xx HAL API.
  *          This is the main program for Cortex-M7
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
#include <main.hpp>
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "model-parameters/model_metadata.h"
#include <stdarg.h>
#include "beasth7.h"
#include "defs.h"
#include "string.h"
#include "uart_hal_cm7.h"
#include "crc_hal_cm7.h"
#include "error_handler.h"
#include "errorcodes.h"
#include "msg_types.h"
#include "amp_messaging_cm7.h"
#include "arm_common_tables.h"
#include "leds.h"
#include "classifier.h"

/** @addtogroup STM32H7xx_HAL_Examples
  * @{
  */

/** @addtogroup GPIO_EXTI
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define RPMSG_CHAN_NAME              "BH7_ML"
/* Private variables ---------------------------------------------------------*/

extern struct rpmsg_endpoint rp_endpoint;

osSemaphoreId osSemaphore_ChannelCreation;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void CPU_CACHE_Enable(void);
static void MPU_Config(void);
static void SystemClock_Config(void);

// thread handlery
static void Thread_Init(void const *argument);
static void Thread_ReadMailbox(void const *argument);
static void Thread_Classifier(void const *argument);

// definice threadu
osThreadDef(Thread0, Thread_Init, osPriorityLow, 0, appliSTACK_SIZE);
osThreadDef(Thread1, Thread_ReadMailbox, osPriorityNormal, 1, configMINIMAL_STACK_SIZE);
osThreadDef(Thread2, Thread_Classifier, osPriorityHigh, 1, classifierSTACK_SIZE);

// classifier
static int16_t array[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];

/* Private functions ---------------------------------------------------------*/

#define SDRAM_BASE_ADDR        0xD0000000


void ei_printf(const char *format, ...)
{
	va_list myargs;
	va_start(myargs, format);
	vprint(format, myargs);
	va_end(myargs);
}

/**
 * Get raw signal data
 */
static int get_signal_data(size_t offset, size_t length, float *out_ptr)
{
	// idle simulation
//	memset(&array, 0, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);

//	// triangle
//	uint16_t i;
//	int16_t value = 0;
//	for (i = 0; i < length/4; i++)
//	{
//		array[i] = value;
//		value += 500;
//	}
//	for (i = length/4; i < length/2; i++)
//	{
//		array[i] = value;
//		value -= 500;
//	}
//	for (i = length/2; i < 3*length/4; i++)
//	{
//		array[i] = value;
//		value += 500;
//	}
//	for (i = 3*length/4; i < length; i++)
//	{
//		array[i] = value;
//		value -= 500;
//	}

//	// sinus
//	uint16_t i;
//	uint16_t index = 0;
//	for (i = 0; i < length/2; i++)
//	{
//		array[i] = (int16_t)(30000*sinTable_f32[index]);
//		index += 8;
//	}
//	index = 0;
//	for (i = length/2; i < length; i++)
//	{
//		array[i] = (int16_t)(30000*sinTable_f32[index]);
//		index += 8;
//	}

//	numpy::int16_to_float(array, out_ptr, length);

	int16_t *p_buffer = (int16_t *)classifier_get_data_address();
	//numpy::int16_to_float(p_buffer, out_ptr, length);

	for (size_t ix = 0; ix < length; ix++)
	{
		out_ptr[ix] = 10.0f * (float)(p_buffer[ix]) / 4096;
	}

	return 0;
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* System Init, System clock, voltage scaling and L1-Cache configuration are done by CPU1 (Cortex-M7) 
     in the meantime Domain D2 is put in STOP mode(Cortex-M4 in deep-sleep)
  */
  
  /* Enable the CPU Cache */
	CPU_CACHE_Enable();
  
  // enable debug in stop mode for all domains
  DBGMCU->CR |= 0x0070003F;
  CoreDebug->DHCSR = 0xA05F0001;
  
 /* STM32H7xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */

  // pockej, nez nastartuje CM4 svuj kod
  while (__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET)
  {
	  asm("nop");
  }

  /* Configure the MPU attributes as Write Through for SDRAM*/
  MPU_Config();

  if (HAL_Init() != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure the system clock to 400 MHz */
  SystemClock_Config();  
  
  /* When system initialization is finished, Cortex-M7 will release (wakeup) Cortex-M4  by means of 
     HSEM notification. Cortex-M4 release could be also ensured by any Domain D2 wakeup source (SEV,EXTI..).
  */

  if (crc_config() != RETURN_OK)
  {
	  Error_Handler();
  }

  if (leds_config() != RETURN_OK)
  {
	  Error_Handler();
  }

  if (classifier_config() != RETURN_OK)
  {
	  Error_Handler();
  }

  /* Define used semaphore */
  osSemaphoreDef(CHN_CREAT);

  /* Create the semaphore */
  osSemaphore_ChannelCreation  = osSemaphoreCreate(osSemaphore(CHN_CREAT) , 1);

  osThreadCreate(osThread(Thread0), NULL);		// Init
  osThreadCreate(osThread(Thread1), NULL);		// ReadMailBox
  osThreadCreate(osThread(Thread2), NULL);		// Classifier

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  for (;;);
}

/**
  * @brief  thread0 function body
  * @param  argument: Not used
  * @retval None
  */
static void Thread_Init(void const *argument)
{
  int32_t status;
  int32_t timeout;

  /* Initialize the mailbox use notify the other core on new message */
  MAILBOX_Init();

  /* Initialize the rpmsg endpoint to set default addresses to RPMSG_ADDR_ANY */
  rpmsg_init_ept(&rp_endpoint, RPMSG_CHAN_NAME, RPMSG_ADDR_ANY, RPMSG_ADDR_ANY, NULL, NULL);

  /* Initialize OpenAmp and libmetal libraries as RPMSG_MASTER */
  if (MX_OPENAMP_Init(RPMSG_MASTER, new_service_cb) != HAL_OK)
  {
    Error_Handler();
  }

  /*HW semaphore Clock enable*/
  __HAL_RCC_HSEM_CLK_ENABLE();

  /*Take HSEM */
  HAL_HSEM_FastTake(HSEM_ID_0);
  /*Release HSEM in order to notify the CPU2(CM4)*/
  HAL_HSEM_Release(HSEM_ID_0,0);

  /* wait until CPU2 wakes up from stop mode */
  timeout = 0xFFFF;
  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0));
  if ( timeout < 0 )
  {
    Error_Handler();
  }

  /*
   * The rpmsg service is initiate by the remote processor, on M7 new_service_cb
   * callback is received on service creation. Wait for the callback
   */
  OPENAMP_Wait_EndPointready(&rp_endpoint);

  // odesli zpravu o priprave na komunikaci
  status = amp_send_message(MSG_COMM_BIND, NULL, 0);

  if (status < 0)
  {
	  Error_Handler();
  }

  amp_set_status(MESSAGING_ALIVE);

  while (1)
  {
	  osDelay(1000);
	  amp_send_alive_message();
	  //ei_printf("\r\nAL");
  }
}


/**
  * @brief  OpenAMP receive message thread
  * @param  argument: Not used
  * @retval None
  */
static void Thread_ReadMailbox(void const *argument)
{
	static volatile uint16_t number_of_messages_received = 0;

	while (1)
	{
		while (amp_get_status() == MESSAGING_INIT)
		{
			osDelay(1);
		}

		// pokud bezi classifier, nevyzvedavej zpravy
		if (classifier_get_state() == STOPPED)
		{
			/* Receive the message from the remote CPU */
			volatile uint8_t index = 0;
			do
			{
				number_of_messages_received = amp_receive_message();
				amp_message_decode(index);									// zpracuj zpravu z buferu, zacni s nejstarsi

				index++;													// zvys index dalsi zpravy ke zpracovani
			}
			while (number_of_messages_received > 0);
		}

		osDelay(1);

	}
}

/**
  * @brief  Classifier running thread
  * @param  argument: Not used
  * @retval None
  */
static void Thread_Classifier(void const *argument)
{
	static volatile int16_t len = 0;
	static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal

	while (1)
	{
		if (classifier_get_state() == ENABLED)
		{
			HAL_GPIO_WritePin(LEDB_GPIO_PORT, LEDB_PIN, GPIO_PIN_RESET);		// rozsvit LEDB

			classifier_set_state(RUNNING);

			uint32_t dsp_start_ms = ei_read_timer_ms();

//			ei_printf("\r\nInferencing settings:\r\n");
//			ei_printf("\tInterval: %.2f ms.\r\n", (float)EI_CLASSIFIER_INTERVAL_MS);
//			ei_printf("\tFrame size: %d\r\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
//			ei_printf("\tSample length: %d ms.\r\n", EI_CLASSIFIER_RAW_SAMPLE_COUNT / 16);
//			ei_printf("\tNo. of classes: %d\r\n", sizeof(ei_classifier_inferencing_categories) / sizeof(ei_classifier_inferencing_categories[0]));

			int print_results = -(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW);

			// Do classification (i.e. the inference part)
			signal_t signal;

			// int16_t
			signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
			signal.get_data = &get_signal_data;

			// float
//			float *p_buffer = (float *)classifier_get_data_address();
//			int err = numpy::signal_from_buffer(p_buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
//			if (err != 0) {
//				ei_printf("Failed to create signal from buffer (%d)\n", err);
//				return;
//			}

			ei_impulse_result_t result = { 0 };
			//EI_IMPULSE_ERROR r = run_classifier_continuous(&signal, &result, debug_nn);
			EI_IMPULSE_ERROR r = run_classifier(&signal, &result, debug_nn);
			uint32_t time = ei_read_timer_ms() - dsp_start_ms;
			if (r != EI_IMPULSE_OK)
			{
				ei_printf("ERROR: Failed to run classifier (%d)\r\n", r);
			}

//			// Print output predictions
//			if(++print_results >= (EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW >> 1))
//			{
//				// Comment this section out if you don't want to see the raw scores
//				ei_printf("Predictions (DSP: %d ms, NN: %d ms)\r\n", result.timing.dsp, result.timing.classification);
//				for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
//				{
//				  ei_printf("    %s: %.5f\r\n", result.classification[ix].label, result.classification[ix].value);
//				}
//				print_results = 0;
//			}

			// ukonceni a vysledky
			classifier_set_state(STOPPED);
			for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
			{
				ei_printf("    %s: %.5f\r\n", result.classification[ix].label, result.classification[ix].value);
			}
			ei_printf("    %s: %.5f\r\n", "anomaly", result.anomaly);
			ei_printf("    %s: %d ms\r\n", "time", time);
			HAL_GPIO_WritePin(LEDB_GPIO_PORT, LEDB_PIN, GPIO_PIN_SET);		// zhasni LEDB
		}
		else
		{
			osDelay(10);
		}
	}
}
 
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 400000000 (Cortex-M7 CPU Clock)
  *            HCLK(Hz)                       = 200000000 (Cortex-M4 CPU, Bus matrix Clocks)
  *            AHB Prescaler                  = 2
  *            D1 APB3 Prescaler              = 2 (APB3 Clock  100MHz)
  *            D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
  *            D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
  *            D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 5
  *            PLL_N                          = 160
  *            PLL_P                          = 2
  *            PLL_Q                          = 4
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;
  
  /*!< Supply configuration update enable */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;

  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }
  
/* Select PLL as system clock source and configure  bus clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 | \
                                 RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_D3PCLK1);

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;  
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2; 
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2; 
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2; 
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }
  
  /*
  Note : The activation of the I/O Compensation Cell is recommended with communication  interfaces
          (GPIO, SPI, FMC, QSPI ...)  when  operating at  high frequencies(please refer to product datasheet)       
          The I/O Compensation Cell activation  procedure requires :
        - The activation of the CSI clock
        - The activation of the SYSCFG clock
        - Enabling the I/O Compensation Cell : setting bit[0] of register SYSCFG_CCCSR
  
          To do this please uncomment the following code 
*/
 
  /*  
  __HAL_RCC_CSI_ENABLE() ;
  
  __HAL_RCC_SYSCFG_CLK_ENABLE() ;
  
  HAL_EnableCompensationCell();
  */  
}

/**
* @brief  CPU L1-Cache enable.
* @param  None
* @retval None
*/
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
//  SCB_EnableICache();
  
  /* Enable D-Cache */
//  SCB_EnableDCache();
}

/**
  * @brief  Configure the MPU attributes as Write Through for External SDRAM.
  * @note   The Base Address is 0xD0000000 .
  *         The Configured Region Size is 32MB because same as SDRAM size.
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU attributes as WT for SDRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = SDRAM_DEVICE_ADDR;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
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
