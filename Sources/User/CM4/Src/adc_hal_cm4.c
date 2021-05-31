/*
 * adc_hal_cm4.c
 *
 *  Created on: 17. 5. 2021
 *      Author: Petr Dvorak
 */

#include "main.h"
#include "application.h"
#include "adc_hal_cm4.h"
#include "defs.h"
#include "error_handler.h"
#include "services.h"
#include "beasth7.h"
#include "dataacq.h"

ADC_HandleTypeDef AdcHandle;
ADC_ChannelConfTypeDef sConfig_IN1;
ADC_ChannelConfTypeDef sConfig_IN2;
ADC_ChannelConfTypeDef sConfig_IN3;
TIM_HandleTypeDef TimHandle_adc_sync;
TIM_MasterConfigTypeDef sMasterConfig;

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	// HW peripheral init
	GPIO_InitTypeDef GPIO_InitStruct;
	static DMA_HandleTypeDef DmaHandle;

	ADC_CHANNEL_GPIO_CLK_ENABLE();
	ADC_CLK_ENABLE();
	__HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);
	DMAx_CHANNELx_CLK_ENABLE();

	GPIO_InitStruct.Pin = INPUT1_ADC_PIN | INPUT2_ADC_PIN | INPUT3_ADC_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(ADC_GPIO_PORT, &GPIO_InitStruct);

	// DMA pro IN1 az IN3
	DmaHandle.Instance                 = DMA1_Stream1;
	DmaHandle.Init.Request             = DMA_REQUEST_ADC1;
	DmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	DmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
	DmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
	DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	DmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
	DmaHandle.Init.Mode                = DMA_CIRCULAR;
	DmaHandle.Init.Priority            = DMA_PRIORITY_MEDIUM;
	HAL_DMA_DeInit(&DmaHandle);
	HAL_DMA_Init(&DmaHandle);

	__HAL_LINKDMA(hadc, DMA_Handle, DmaHandle);

	/* NVIC configuration for DMA Input data interrupt */
	HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
}

/**
  * @brief ADC MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO to their default state
  * @param hadc: ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{

  /*##-1- Reset peripherals ##################################################*/
  ADCx_FORCE_RESET();
  ADCx_RELEASE_RESET();
  /* ADC Periph clock disable (automatically reset all ADC instances of the ADC common group) */
  __HAL_RCC_ADC12_CLK_DISABLE();

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* De-initialize the ADC Channel GPIO pin */
  HAL_GPIO_DeInit(ADC_GPIO_PORT, ADC_CHANNEL_PINS);
}

RETURN_STATUS adc_dma_config(uint32_t file_address)
{
	AdcHandle.Instance = ADC;
	if (HAL_ADC_DeInit(&AdcHandle) != HAL_OK)
	{
		/* ADC de-initialization Error */
	    Error_Handler();
	}

	AdcHandle.Init.ClockPrescaler           = ADC_CLOCK_ASYNC_DIV2;            /* Asynchronous clock mode, input ADC clock divided by 2*/
	AdcHandle.Init.Resolution               = ADC_RESOLUTION_12B;              /* x-bit resolution for converted data */
	AdcHandle.Init.ScanConvMode             = ENABLE;                          /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
	AdcHandle.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;             /* EOC flag picked-up to indicate conversion end */
	AdcHandle.Init.LowPowerAutoWait         = DISABLE;                         /* Auto-delayed conversion feature disabled */

	//AdcHandle.Init.ContinuousConvMode       = ENABLE;                          /* Continuous mode enabled (automatic conversion restart after each conversion) */
	AdcHandle.Init.ContinuousConvMode    	= DISABLE;

	AdcHandle.Init.NbrOfConversion          = NUMBER_OF_ADC_CHANNELS;          /*  */
	AdcHandle.Init.DiscontinuousConvMode    = DISABLE;                         /* Parameter discarded because sequencer is disabled */
	AdcHandle.Init.NbrOfDiscConversion      = 1;                               /*  */

	//AdcHandle.Init.ExternalTrigConv         = ADC_SOFTWARE_START;              /* Software start to trig the 1st conversion manually, without external event */
	//AdcHandle.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_NONE;   /* Parameter discarded because software trigger chosen */
	AdcHandle.Init.ExternalTrigConv      	= TIM_ADC_SYNC_TRIGGER;       		/* Trig of conversion start done by external event */
	AdcHandle.Init.ExternalTrigConvEdge  	= ADC_EXTERNALTRIGCONVEDGE_RISING;


	AdcHandle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR; /* ADC DMA circular requested */
	AdcHandle.Init.Overrun                  = ADC_OVR_DATA_OVERWRITTEN;        /* DR register is overwritten with the last conversion result in case of overrun */
	AdcHandle.Init.OversamplingMode         = DISABLE;                         /* No oversampling */

	/* Initialize ADC peripheral according to the passed parameters */
	if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
	{
		Error_Handler();
	}

	/* ### - 2 - Start calibration ############################################ */
	if (HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
	{
		Error_Handler();
	}

	/* ### - 3 - Channel configuration ######################################## */
	sConfig_IN1.Channel      = INPUT1_CHANNEL;              /* Sampled channel number */
	sConfig_IN1.Rank         = ADC_REGULAR_RANK_1;          /* Rank of sampled channel number ADCx_CHANNEL */
	sConfig_IN1.SamplingTime = ADC_SAMPLETIME_387CYCLES_5;  /* Sampling time (number of clock cycles unit) */
	sConfig_IN1.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
	sConfig_IN1.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */
	sConfig_IN1.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig_IN1) != HAL_OK)
	{
		Error_Handler();
	}

	sConfig_IN2.Channel      = INPUT2_CHANNEL;              /* Sampled channel number */
	sConfig_IN2.Rank         = ADC_REGULAR_RANK_2;          /* Rank of sampled channel number ADCx_CHANNEL */
	sConfig_IN2.SamplingTime = ADC_SAMPLETIME_387CYCLES_5;  /* Sampling time (number of clock cycles unit) */
	sConfig_IN2.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
	sConfig_IN2.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */
	sConfig_IN2.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig_IN2) != HAL_OK)
	{
		Error_Handler();
	}

	sConfig_IN3.Channel      = INPUT3_CHANNEL;              /* Sampled channel number */
	sConfig_IN3.Rank         = ADC_REGULAR_RANK_3;          /* Rank of sampled channel number ADCx_CHANNEL */
	sConfig_IN3.SamplingTime = ADC_SAMPLETIME_387CYCLES_5;  /* Sampling time (number of clock cycles unit) */
	sConfig_IN3.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
	sConfig_IN3.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */
	sConfig_IN3.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig_IN3) != HAL_OK)
	{
		Error_Handler();
	}

	/* ### - 4 - Start conversion in DMA mode ################################# */
	if (HAL_ADC_Start_DMA(&AdcHandle, (uint32_t *)file_address, dacq_adc_file_size()) != HAL_OK)
	//if (HAL_ADC_Start_DMA(&AdcHandle, (uint32_t *)ADC_buffer, dacq_adc_file_size()) != HAL_OK)
	{
		Error_Handler();
	}
//
//	// nastaveni startovaciho casovace
//	adc_sync_timer_init(dacq_get_freq());

	return RETURN_OK;
}

RETURN_STATUS adc_sync_start(void)
{
	// nastaveni startovaciho casovace
	adc_sync_timer_init(dacq_get_freq());

	return RETURN_OK;
}

RETURN_STATUS adc_dma_unconfig(void)
{
	AdcHandle.Instance = ADC;

	if (HAL_ADC_Stop_DMA(&AdcHandle) != HAL_OK)
	{
		/* ADC stop call Error */
		Error_Handler();
	}
	if (HAL_ADC_DeInit(&AdcHandle) != HAL_OK)
	{
		/* ADC de-initialization Error */
	    Error_Handler();
	}

	return RETURN_OK;
}

// vstupni hodnota je od 1 do 32000 Hz
RETURN_STATUS adc_sync_timer_init(uint32_t freq)
{
	RCC_ClkInitTypeDef clk_init_struct = {0};       /* Temporary variable to retrieve RCC clock configuration */
	uint32_t latency;                               /* Temporary variable to retrieve Flash Latency */
	volatile uint32_t timer_clock_frequency = 0;    /* Timer clock frequency */
	uint32_t prescaler;
	uint32_t period;

	TIM_ADC_SYNC_CLK_ENABLE();
	HAL_NVIC_SetPriority(TIM_ADC_SYNC_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(TIM_ADC_SYNC_IRQn);

	 /* Retrieve timer clock source frequency */
	HAL_RCC_GetClockConfig(&clk_init_struct, &latency);
	/* If APB1 prescaler is different of 1, timers have a factor x2 on their clock source. */
	if (clk_init_struct.APB1CLKDivider == RCC_HCLK_DIV1)
	{
		timer_clock_frequency = HAL_RCC_GetPCLK1Freq();
	}
	else
	{
		timer_clock_frequency = HAL_RCC_GetPCLK1Freq() * 2;
	}

	// vychozi hodiny jsou 200 MHz
	// prescaler muze mit hodnotu mezi 0 a 65535
	// period muze mit hodnotu mezi 0 a 65535
	// minimalne frekvence pri prescaler = 65535 a period = 65535 -> 200 MHz / (65535 x 65535) = 46 mHz
	// maximalni frekvence pri prescaler = 0 a period = 1 -> 200 MHz
	// vstupni parametr je perioda pocitana z 200 MHz hodin a prescaler
	// pro celkovou periodu vetsi nez 65535 je nutne pouzit prescaler a rozdelit vstupni periodu na dva nasobitele prescaler a period, aby soucin byl co nejpodobnejsi pozadovane periode

	if (freq != 0)
	{
		// initialized dataacq values
		prescaler = (timer_clock_frequency / ((0xFFFF-1) * 1U)) + 1;
		period = ((timer_clock_frequency / (prescaler * freq)) - 1)/2;
	}
	else
	{
		// uninitialized dataacq values
		prescaler = PRESC_1HZ;
		period = PERIO_1HZ;
	}

	TimHandle_adc_sync.Instance = TIM_ADC_SYNC;
	TimHandle_adc_sync.Init.Prescaler         = prescaler;
	TimHandle_adc_sync.Init.Period            = period;
	TimHandle_adc_sync.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
	TimHandle_adc_sync.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TimHandle_adc_sync.Init.RepetitionCounter = 0;
	if(HAL_TIM_Base_Init(&TimHandle_adc_sync) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}

	sMasterConfig.MasterOutputTrigger  = TIM_TRGO_UPDATE;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode      = TIM_MASTERSLAVEMODE_DISABLE;
	if(HAL_TIMEx_MasterConfigSynchronization(&TimHandle_adc_sync, &sMasterConfig) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}

	if (HAL_TIM_Base_Start_IT(&TimHandle_adc_sync) != HAL_OK)
	{
		/* Starting Error */
		Error_Handler();
	}

	return RETURN_OK;
}

RETURN_STATUS adc_sync_timer_deinit(void)
{
	HAL_TIM_Base_Stop(&TimHandle_adc_sync);
	HAL_TIM_Base_DeInit(&TimHandle_adc_sync);

	return RETURN_OK;
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//HAL_GPIO_TogglePin(LEDC_GPIO_PORT, LEDC_PIN);
}

/**
  * @brief  Conversion DMA half-transfer callback in non-blocking mode
  * @param  hadc: ADC handle
  * @retval None
  */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
  /* Invalidate Data Cache to get the updated content of the SRAM on the first half of the ADC converted data buffer: 32 bytes */
//  SCB_InvalidateDCache_by_Addr((uint32_t *) &ADC_buffer[0], ADC_CONVERTED_DATA_BUFFER_SIZE);
}


/**
  * @brief  Conversion complete callback in non-blocking mode
  * @param  hadc: ADC handle
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
   /* Invalidate Data Cache to get the updated content of the SRAM on the second half of the ADC converted data buffer: 32 bytes */
//  SCB_InvalidateDCache_by_Addr((uint32_t *) &ADC_buffer[ADC_CONVERTED_DATA_BUFFER_SIZE/2], ADC_CONVERTED_DATA_BUFFER_SIZE);
}

// vrat hodnotu AIN1 vypocitanou z nabranych vzorku
uint32_t adc_get_ain1(void)
{
	uint32_t value = 0;
	uint16_t i;

//	for (i = 0; i < 3*ADC_CONVERTED_DATA_BUFFER_SIZE; i += 3)
//	{
//		value += ADC_buffer[i];
//	}
//
//	return value /= ADC_CONVERTED_DATA_BUFFER_SIZE;
	return 0;
}

// vrat hodnotu AIN1 vypocitanou z nabranych vzorku
uint32_t adc_get_ain2(void)
{
	uint32_t value = 0;
	uint16_t i;

//	for (i = 1; i < 3*ADC_CONVERTED_DATA_BUFFER_SIZE; i += 3)
//	{
//		value += ADC_buffer[i];
//	}
//
//	return value /= ADC_CONVERTED_DATA_BUFFER_SIZE;
	return 0;
}

// vrat hodnotu AIN1 vypocitanou z nabranych vzorku
uint32_t adc_get_ain3(void)
{
	uint32_t value = 0;
	uint16_t i;

//	for (i = 2; i < 3*ADC_CONVERTED_DATA_BUFFER_SIZE; i += 3)
//	{
//		value += ADC_buffer[i];
//	}
//
//	return value /= ADC_CONVERTED_DATA_BUFFER_SIZE;
	return 0;
}
