/**
  ******************************************************************************
  * @file    rf_driver_hal_rng_v168.c
  * @author  RF Application Team
  * @brief   RNG HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Random Number Generator (RNG) peripheral:
  *           + Initialization and configuration functions
  *           + Peripheral Control functions
  *           + Peripheral State functions
  *
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
  [..]
      The RNG HAL driver can be used as follows:

      (#) Enable the RNG controller clock using __HAL_RCC_RNG_CLK_ENABLE() macro
          in HAL_RNG_MspInit().
      (#) Activate the RNG peripheral using HAL_RNG_Init() function.
      (#) Wait until the 32 bit Random Number Generator contains a valid
          random data using polling mode.
      (#) Get the 32 bit random number using HAL_RNG_GenerateRandomNumber() function.


  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "rf_driver_hal.h"
#include "rf_driver_ll_rng_v168.h"

/** @addtogroup RF_DRIVER_HAL_Driver
  * @{
  */

#if defined (RNG)

/** @addtogroup RNG
  * @brief RNG HAL module driver.
  * @{
  */

#ifdef HAL_RNG_MODULE_ENABLED

/* Private types -------------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup RNG_Private_Constants RNG Private Constants
  * @{
  */
#define RNG_TIMEOUT_VALUE     2U
/**
  * @}
  */
/* Private macros ------------------------------------------------------------*/
/* Private functions prototypes ----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @addtogroup RNG_Exported_Functions
  * @{
  */

/** @addtogroup RNG_Exported_Functions_Group1
 *  @brief   Initialization and configuration functions
 *
@verbatim
 ===============================================================================
          ##### Initialization and configuration functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize the RNG according to the specified parameters
          in the RNG_InitTypeDef and create the associated handle
      (+) DeInitialize the RNG peripheral
      (+) Initialize the RNG MSP
      (+) DeInitialize RNG MSP

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the RNG peripheral and creates the associated handle.
  * @param  hrng pointer to a RNG_HandleTypeDef structure that contains
  *                the configuration information for RNG.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RNG_Init(RNG_HandleTypeDef *hrng)
{
  /* Check the RNG handle allocation */
  if (hrng == NULL)
  {
    return HAL_ERROR;
  }
  /* Check the parameters */
  assert_param(IS_RNG_ALL_INSTANCE(hrng->Instance));
  assert_param(IS_RNG_CED(hrng->Init.SamplingClockDivider));

  if (hrng->State == HAL_RNG_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hrng->Lock = HAL_UNLOCKED;

    /* Init the low level hardware */
    HAL_RNG_MspInit(hrng);
  }

  /* Change RNG peripheral state */
  hrng->State = HAL_RNG_STATE_BUSY;

  /* Check that the divider value is equal to the one to configure. If not, it must redo the write and checking. */
  while( LL_RNG_GetSamplingClockEnableDivider(RNG) != hrng->Init.SamplingClockDivider)
  {
     /* While the new divider value is being resynchronized with the TRNG core clock domain, it is not possible to write another new value. */
     LL_RNG_SetSamplingClockEnableDivider(RNG, hrng->Init.SamplingClockDivider);
  }
  
  /* Enable the RNG Peripheral */
  __HAL_RNG_ENABLE(hrng);

  /* Initialize the RNG state */
  hrng->State = HAL_RNG_STATE_READY;

  /* Initialise the error code */
  hrng->ErrorCode = HAL_RNG_ERROR_NONE;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  DeInitializes the RNG peripheral.
  * @param  hrng pointer to a RNG_HandleTypeDef structure that contains
  *                the configuration information for RNG.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RNG_DeInit(RNG_HandleTypeDef *hrng)
{
  /* Check the RNG handle allocation */
  if (hrng == NULL)
  {
    return HAL_ERROR;
  }

  /* DeInit the low level hardware */
  HAL_RNG_MspDeInit(hrng);

  /* Enable the RNG Peripheral */
  __HAL_RNG_DISABLE(hrng);
  
  /* Update the RNG state */
  hrng->State = HAL_RNG_STATE_RESET;

  /* Initialise the error code */
  hrng->ErrorCode = HAL_RNG_ERROR_NONE;

  /* Release Lock */
  __HAL_UNLOCK(hrng);

  /* Return the function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the RNG MSP.
  * @param  hrng pointer to a RNG_HandleTypeDef structure that contains
  *                the configuration information for RNG.
  * @retval None
  */
WEAK_FUNCTION(void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng))
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrng);
  /* NOTE : This function should not be modified. When the callback is needed,
            function HAL_RNG_MspInit must be implemented in the user file.
   */
}

/**
  * @brief  DeInitializes the RNG MSP.
  * @param  hrng pointer to a RNG_HandleTypeDef structure that contains
  *                the configuration information for RNG.
  * @retval None
  */
WEAK_FUNCTION(void HAL_RNG_MspDeInit(RNG_HandleTypeDef *hrng))
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrng);
  /* NOTE : This function should not be modified. When the callback is needed,
            function HAL_RNG_MspDeInit must be implemented in the user file.
   */
}

/**
  * @}
  */

/** @addtogroup RNG_Exported_Functions_Group2
 *  @brief   Peripheral Control functions
 *
@verbatim
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Get the 32 bit Random number

@endverbatim
  * @{
  */

/**
  * @brief  Generates a 32-bit random number.
  * @note   Each time the random number data is read the RNG_FLAG_DRDY flag
  *         is automatically cleared.
  * @param  hrng pointer to a RNG_HandleTypeDef structure that contains
  *                the configuration information for RNG.
  * @param  random32bit pointer to generated random number variable if successful.
  * @retval HAL status
  */

HAL_StatusTypeDef HAL_RNG_GenerateRandomNumber(RNG_HandleTypeDef *hrng, uint32_t *random32bit)
{
  uint32_t tickstart;
  HAL_StatusTypeDef status = HAL_OK;

  /* Process Locked */
  __HAL_LOCK(hrng);

  /* Check RNG peripheral state */
  if (hrng->State == HAL_RNG_STATE_READY)
  {
    /* Change RNG peripheral state */
    hrng->State = HAL_RNG_STATE_BUSY;

    /* Get tick */
    tickstart = HAL_GetTick();

    /* Check if data register contains valid random data */
    while (__HAL_RNG_GET_FLAG(hrng, RNG_FLAG_DRDY) == RESET)
    {
      if ((HAL_GetTick() - tickstart) > RNG_TIMEOUT_VALUE)
      {
        hrng->State = HAL_RNG_STATE_READY;
        hrng->ErrorCode |= HAL_RNG_ERROR_TIMEOUT;
        /* Process Unlocked */
        __HAL_UNLOCK(hrng);
        return HAL_ERROR;
      }
    }

    /* Get a 32bit Random number */
    hrng->RandomNumber = hrng->Instance->VAL;
    *random32bit = hrng->RandomNumber;

    hrng->State = HAL_RNG_STATE_READY;
  }
  else
  {
    status = HAL_ERROR;
  }

  /* Process Unlocked */
  __HAL_UNLOCK(hrng);

  return status;
}

/**
  * @brief  Read latest generated random number.
  * @param  hrng pointer to a RNG_HandleTypeDef structure that contains
  *                the configuration information for RNG.
  * @retval random value
  */
uint32_t HAL_RNG_ReadLastRandomNumber(RNG_HandleTypeDef *hrng)
{
  return (hrng->RandomNumber);
}

/**
  * @}
  */


/** @addtogroup RNG_Exported_Functions_Group3
 *  @brief   Peripheral State functions
 *
@verbatim
 ===============================================================================
                      ##### Peripheral State functions #####
 ===============================================================================
    [..]
    This subsection permits to get in run-time the status of the peripheral
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Returns the RNG state.
  * @param  hrng pointer to a RNG_HandleTypeDef structure that contains
  *                the configuration information for RNG.
  * @retval HAL state
  */
HAL_RNG_StateTypeDef HAL_RNG_GetState(RNG_HandleTypeDef *hrng)
{
  return hrng->State;
}

/**
  * @brief  Return the RNG handle error code.
  * @param  hrng: pointer to a RNG_HandleTypeDef structure.
  * @retval RNG Error Code
*/
uint32_t HAL_RNG_GetError(RNG_HandleTypeDef *hrng)
{
  /* Return RNG Error Code */
  return hrng->ErrorCode;
}
/**
  * @}
  */

/**
  * @}
  */


#endif /* HAL_RNG_MODULE_ENABLED */
/**
  * @}
  */

#endif /* RNG */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
