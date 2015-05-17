/**
  ******************************************************************************
  * File Name          : main.c
  * Date               : 15/05/2015 17:03:37
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2015 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include "can_dbc.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;

CAN_HandleTypeDef hcan;

osThreadId defaultTaskHandle;

/* USER CODE BEGIN PV */
#define STACK_SIZE 	1024
#define PRIORITY_SEND_CANMSG_LAMPS							(tskIDLE_PRIORITY + 2U)
#define PRIORITY_READ_USER_COMMAND							(tskIDLE_PRIORITY + 1U)

#define SWITCH_HIGH_BEAM			 		GPIO_PIN_13
#define SWITCH_LOW_BEAM				 		GPIO_PIN_1
#define SWITCH_TURN_RIGHT			 		GPIO_PIN_2
#define SWITCH_TURN_LEFT			 		GPIO_PIN_3
#define SWITCH_MARKER							GPIO_PIN_4
#define SWITCH_DRL								GPIO_PIN_5
#define SWITCH_EMERGENCY_STOP			GPIO_PIN_6	
#define SWITCH_FRONT_FOG					GPIO_PIN_7
#define SWITCH_REAR_FOG						GPIO_PIN_8
#define SWITCH_TAIL								GPIO_PIN_9
#define SWITCH_BRAKE							GPIO_PIN_10
#define SWITCH_DOME								GPIO_PIN_11

struct_Msg_Init_Ext_Light msg_Init_Ext_Light;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_CAN_Init(void);
void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */
void vTask_SendCanMsg_Lamps(void *parameters);
void vTask_ReadUserCommand(void *parameters);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC_Init();
  MX_CAN_Init();

  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	/* Task sends the message MSG_INT_EXT_LIGHT */
	xTaskCreate( vTask_SendCanMsg_Lamps, "SendLampMsg", STACK_SIZE, NULL, 2, NULL );
							 
	/* Task reads input from user */
	xTaskCreate( vTask_ReadUserCommand, "ReadUserCommand", STACK_SIZE, NULL, 0, NULL );
							 
	/* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
 

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI14|RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV16;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

  __SYSCFG_CLK_ENABLE();

}

/* ADC init function */
void MX_ADC_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC;
  hadc.Init.Resolution = ADC_RESOLUTION12b;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = OVR_DATA_PRESERVED;
  HAL_ADC_Init(&hadc);

    /**Configure for the selected ADC regular channel to be converted. 
    */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

    /**Configure for the selected ADC regular channel to be converted. 
    */
  sConfig.Channel = ADC_CHANNEL_1;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

}

/* CAN init function */
void MX_CAN_Init(void)
{

  static CanTxMsgTypeDef TxMessage;
	
	hcan.Instance = CAN;
  hcan.pTxMsg = &TxMessage;
	
	hcan.Init.Prescaler = 2;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SJW = CAN_SJW_1TQ;
  hcan.Init.BS1 = CAN_BS1_9TQ;
  hcan.Init.BS2 = CAN_BS2_2TQ;
  hcan.Init.TTCM = DISABLE;
  hcan.Init.ABOM = DISABLE;
  hcan.Init.AWUM = DISABLE;
  hcan.Init.NART = DISABLE;
  hcan.Init.RFLM = DISABLE;
  hcan.Init.TXFP = DISABLE;
  HAL_CAN_Init(&hcan);
	
}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
     PA2   ------> USART2_TX
     PA3   ------> USART2_RX
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOC_CLK_ENABLE();
  __GPIOF_CLK_ENABLE();
  __GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	/*Configure GPIO pin : PC01 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	/*Configure GPIO pin : PC02 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
 /*Configure GPIO pin : PC03 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	/*Configure GPIO pin : PC04 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	/*Configure GPIO pin : PC05 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	/*Configure GPIO pin : PC06 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	/*Configure GPIO pin : PC07 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	/*Configure GPIO pin : PC08 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	/*Configure GPIO pin : PC09 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	/*Configure GPIO pin : PC10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	/*Configure GPIO pin : PC11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
  /*Configure GPIO pins : PA2 PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void vTask_ReadUserCommand(void *parameters)
{
	for(;;)
	{
		
	(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOC, SWITCH_HIGH_BEAM))?(msg_Init_Ext_Light.signal_high_beam = 0)
																														 :(msg_Init_Ext_Light.signal_high_beam = 1);
	
	
	(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOC, SWITCH_LOW_BEAM))?(msg_Init_Ext_Light.signal_low_beam  = 0)
																														:(msg_Init_Ext_Light.signal_low_beam = 1);
	
	(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOC, SWITCH_TURN_RIGHT))?(msg_Init_Ext_Light.signal_right_turn_indicator = 0)
																															:(msg_Init_Ext_Light.signal_right_turn_indicator = 1);
		
	(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOC, SWITCH_TURN_LEFT))?(msg_Init_Ext_Light.signal_left_turn_indicator = 0)
																														 :(msg_Init_Ext_Light.signal_left_turn_indicator = 1);
		
	(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOC, SWITCH_MARKER))?(msg_Init_Ext_Light.signal_marker_light = 0)
																													:(msg_Init_Ext_Light.signal_marker_light = 1);
	
	(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOC, SWITCH_DRL))?(msg_Init_Ext_Light.signal_drl = 0)
																											 :(msg_Init_Ext_Light.signal_drl = 1);

	(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOC, SWITCH_EMERGENCY_STOP))?(msg_Init_Ext_Light.signal_emergency_stop = 0)
																																	:(msg_Init_Ext_Light.signal_emergency_stop = 1);
		
	(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOC, SWITCH_FRONT_FOG))?(msg_Init_Ext_Light.signal_front_fog_lamp = 0)
																														 :(msg_Init_Ext_Light.signal_front_fog_lamp = 1);

	(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOC, SWITCH_REAR_FOG))?(msg_Init_Ext_Light.signal_rear_fog_lamp = 0)
																														:(msg_Init_Ext_Light.signal_rear_fog_lamp = 1);
	
	(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOC, SWITCH_TAIL))?(msg_Init_Ext_Light.signal_tail_lamp = 0)
																												:(msg_Init_Ext_Light.signal_tail_lamp = 1);

	(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOC, SWITCH_BRAKE))?(msg_Init_Ext_Light.signal_brake_lamp = 0)
																												 :(msg_Init_Ext_Light.signal_brake_lamp = 1);
		
	(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOC, SWITCH_DOME))?(msg_Init_Ext_Light.signal_dome_lamp = 0)
																												:(msg_Init_Ext_Light.signal_dome_lamp = 1);
																												
	
		
	}

}

void vTask_SendCanMsg_Lamps(void *parameters)
{
	
	
	msg_Init_Ext_Light.id  = ID_INT_EXT_LIGHT;
	msg_Init_Ext_Light.dlc = DLC_INT_EXT_LIGHT;
	msg_Init_Ext_Light.cycle_time = 100;
	
	for(;;)
	{
		hcan.pTxMsg->StdId  = msg_Init_Ext_Light.id;
		hcan.pTxMsg->DLC    = msg_Init_Ext_Light.dlc;
		hcan.pTxMsg->IDE    = CAN_RTR_DATA;
		hcan.pTxMsg->RTR    = CAN_ID_STD;
		
		
		hcan.pTxMsg->Data[0]= (msg_Init_Ext_Light.signal_high_beam << SIGNAL_HIGH_BEAM)|						
													(msg_Init_Ext_Light.signal_low_beam << SIGNAL_LOW_BEAM)|								
													(msg_Init_Ext_Light.signal_left_turn_indicator << SIGNAL_LEFT_TURN_INDICATOR)|	
													(msg_Init_Ext_Light.signal_right_turn_indicator << SIGNAL_RIGHT_TURN_INDICATOR)|		
													(msg_Init_Ext_Light.signal_marker_light << SIGNAL_MARKER_LIGHT)|						
													(msg_Init_Ext_Light.signal_drl << SIGNAL_DRL)|		
													(msg_Init_Ext_Light.signal_emergency_stop << SIGNAL_EMERGENCY_STOP)|		
													(msg_Init_Ext_Light.signal_front_fog_lamp << SIGNAL_FRONT_FOG_LAMP);
		
		hcan.pTxMsg->Data[1]= (msg_Init_Ext_Light.signal_rear_fog_lamp << SIGNAL_REAR_FOG_LAMP)|				
													(msg_Init_Ext_Light.signal_tail_lamp << SIGNAL_TAIL_LAMP)|									
													(msg_Init_Ext_Light.signal_brake_lamp << SIGNAL_BRAKE_LAMP)|									
													(msg_Init_Ext_Light.signal_dome_lamp << SIGNAL_DOME_LAMP);
		
		
		HAL_CAN_Transmit(&hcan, 10);
		
		vTaskDelay(msg_Init_Ext_Light.cycle_time);	/* wait for 100mS and send the message again*/
	  
	}

}
/* USER CODE END 4 */

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
