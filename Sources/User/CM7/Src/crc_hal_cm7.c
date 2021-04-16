/*
 * crc_hal_cm7.c
 *
 *  Created on: 15. 4. 2021
 *      Author: Petr Dvorak
 */

#include <main.hpp>
#include "crc_hal_cm7.h"
#include "defs.h"
#include "error_handler.h"

CRC_HandleTypeDef hcrc;

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
RETURN_STATUS crc_config(void)
{
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;

  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
	  return RETURN_ERROR;
  }
  else
  {
	  return RETURN_OK;
  }
}
