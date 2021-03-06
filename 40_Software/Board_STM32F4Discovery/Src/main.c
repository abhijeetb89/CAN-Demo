/**
  ******************************************************************************
  * File Name          : main.c
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
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "usb_device.h"
#include "usbd_customhid.h"
#include "can_dbc.h"
#include "timers.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;

osThreadId defaultTaskHandle;

/* USER CODE BEGIN PV */

uint8_t engine_switch_pressed = 0;
uint8_t message_received_flag = 0;
uint8_t message_transmitted_flag = 0;
uint8_t timer_expire_count=0;

#define LAMP_HIGH_BEAM			 		GPIO_PIN_12
#define LAMP_LOW_BEAM				 		GPIO_PIN_13
#define LAMP_TURN_RIGHT			 		GPIO_PIN_14
#define LAMP_TURN_LEFT			 		GPIO_PIN_15
#define LAMP_MARKER							GPIO_PIN_8
#define LAMP_DRL								GPIO_PIN_9
#define LAMP_EMERGENCY_STOP			GPIO_PIN_10	
#define LAMP_FRONT_FOG					GPIO_PIN_11
#define LAMP_REAR_FOG						GPIO_PIN_3
#define LAMP_TAIL								GPIO_PIN_4
#define LAMP_BRAKE							GPIO_PIN_5
#define LAMP_DOME								GPIO_PIN_6

#define STACK_SIZE  1024


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_CAN1_Init(void);
static void MX_CAN2_Init(void);
void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */

void vTask_ControlLamps(void *parameters);
void vTask_SendEngineStatus(void *parameters);
void vTask_SendDataToPc(void *parameters);

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
  MX_ADC1_Init();
  MX_CAN1_Init();
  MX_CAN2_Init();
	MX_USB_DEVICE_Init();
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
  //osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  //defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	
	/* Task reads the message MSG_INT_EXT_LIGHT and sets interior and exterior lamps accordingly */
	//xTaskCreate( vTask_SendDataToPc, "USBTask", STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( vTask_ControlLamps, "SetLeds", STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( vTask_SendEngineStatus, "SendEngineStatus", STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );
	
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

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

}

/* ADC1 init function */
void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION12b;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = EOC_SINGLE_CONV;
  HAL_ADC_Init(&hadc1);

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

}

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  IRQn_Type irq;
  static CanRxMsgTypeDef RxMessage;
	static CanTxMsgTypeDef TxMessage;
  CAN_FilterConfTypeDef  sFilterConfig; 
	
  __CAN_CLK_ENABLE();
	
  hcan1.Instance = CAN1;
  hcan1.pRxMsg   = &RxMessage;
	hcan1.pTxMsg   = &TxMessage;
  hcan1.Init.Prescaler = 16;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SJW = CAN_SJW_1TQ;
  hcan1.Init.BS1 = CAN_BS1_14TQ;
  hcan1.Init.BS2 = CAN_BS2_6TQ;
  hcan1.Init.TTCM = DISABLE;
  hcan1.Init.ABOM = DISABLE;
  hcan1.Init.AWUM = DISABLE;
  hcan1.Init.NART = DISABLE;
  hcan1.Init.RFLM = DISABLE;
  hcan1.Init.TXFP = DISABLE;
  HAL_CAN_Init(&hcan1);
	
  /*##-2- Configure the CAN Filter ###########################################*/
  sFilterConfig.FilterNumber = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = 0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.BankNumber = 14;
  
  if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    while(1);
  }
	
  irq = CAN1_RX0_IRQn;
	
  HAL_NVIC_EnableIRQ(irq);
	
  HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);

}

/* CAN2 init function */
void MX_CAN2_Init(void)
{

  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 16;
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SJW = CAN_SJW_1TQ;
  hcan2.Init.BS1 = CAN_BS1_1TQ;
  hcan2.Init.BS2 = CAN_BS2_1TQ;
  hcan2.Init.TTCM = DISABLE;
  hcan2.Init.ABOM = DISABLE;
  hcan2.Init.AWUM = DISABLE;
  hcan2.Init.NART = DISABLE;
  hcan2.Init.RFLM = DISABLE;
  hcan2.Init.TXFP = DISABLE;
  HAL_CAN_Init(&hcan2);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
     PC3   ------> I2S2_SD
     PA4   ------> I2S3_WS
     PA5   ------> SPI1_SCK
     PA6   ------> SPI1_MISO
     PA7   ------> SPI1_MOSI
     PB10   ------> I2S2_CK
     PC7   ------> I2S3_MCK
     PC10   ------> I2S3_CK
     PC12   ------> I2S3_SD
     PB6   ------> I2C1_SCL
     PB9   ------> I2C1_SDA
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOE_CLK_ENABLE();
  __GPIOC_CLK_ENABLE();
  __GPIOH_CLK_ENABLE();
  __GPIOA_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();
  __GPIOD_CLK_ENABLE();

  /*Configure GPIO pin : PE3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PC0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA5 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PD12 PD13 PD14 PD15 
                           PD4 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15 
                          |GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PC7 PC10 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_10|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PD5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PB6 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void vTimerCallback( TimerHandle_t pxTimer )
{
	const uint8_t xMaxExpiryCountBeforeStopping = 5;
	
	
	/* Optionally do something if the pxTimer parameter is NULL. */
  configASSERT( pxTimer );
	
	if(xMaxExpiryCountBeforeStopping == timer_expire_count)
	{
		timer_expire_count = 0;	 
		xTimerStop( pxTimer, 0 );
	}
	else
	{
					
					
	
	}

}

/*
Task: 			 vTask_SendEngineStatus	
Parameters:  void
Return: 		 void
Description: This FreeRTOS thread sends the engine status to all ECUs. Note that engine button is push to start
             and push to stop.
*/

void vTask_SendEngineStatus(void *parameters)
{
	uint8_t i=0;
	uint8_t data[11];
	static uint8_t msg_transmit_cnt=5;
	TickType_t xLastWakeTime;
  const TickType_t xFrequency = 1000;
	TimerHandle_t xTimer;
	xTimer = xTimerCreate
								(  /* Just a text name, not used by the RTOS kernel. */
                     "Timer",
                     /* The timer period in ticks. */
                     ( 200 ),
                     /* The timers will auto-reload themselves when they expire. */
                     pdTRUE,
                     /* Assign each timer a unique id equal to its array index. */
                     ( void * ) 1,
                     /* Each timer calls the same callback when it expires. */
                     vTimerCallback
                );
										 
	for(;;)
		{
			//switch has been pressed. inital value of engine_switch_pressed is 0.
			if((GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) && (msg_transmit_cnt==5))
			{
				engine_switch_pressed= !engine_switch_pressed;
				msg_transmit_cnt = 0;
				/*
				timer_expire_count = 0;				
				if( xTimerStart( xTimer, 0 ) != pdPASS )
        {
           // The timer could not be set into the Active state. 
        }
				*/
				
			}	
			
			if(msg_transmit_cnt<5)
			{
				//prepare message
					hcan1.pTxMsg->StdId   = MSG_Engine_Status;
					hcan1.pTxMsg->DLC     = DLC_Engine_Status;
					hcan1.pTxMsg->IDE     = CAN_RTR_DATA;
					hcan1.pTxMsg->RTR     = CAN_ID_STD;
					hcan1.pTxMsg->Data[0] = (engine_switch_pressed & SIGNAL_ENGINE_STATUS);
					
					//Transmit the message								
					if(HAL_OK == HAL_CAN_Transmit(&hcan1, 100))
					{
							HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
					}
					else
					{
							HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
					}
					
					//timer_expire_count += 1;
					msg_transmit_cnt =  msg_transmit_cnt + 1;
					
					//make message_transmitted flag to 1 so that USB task will send message to PC
					message_transmitted_flag = 1;
					
					i=0;
		
					data[i++] =  hcan1.pTxMsg->StdId;
					data[i++] = (hcan1.pTxMsg->StdId) >> 8;
					data[i++] =  hcan1.pTxMsg->DLC;
			
					for(; i<11; i++)
					{
					data[i] = hcan1.pTxMsg->Data[i-3];
					}
				
				
					if(USBD_OK == USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, data, sizeof(data)))
					{
					
					}
					else
					{
						
					}
		
			}

			vTaskDelayUntil(&xLastWakeTime,xFrequency);
	
			
		}

}

/*
Task: 			 vTask_ControlLamps	
Parameters:  void
Return: 		 void
Description: This FreeRTOS thread reads message MSG_INT_EXT_LIGHT(0x350) and sets corresponding lamps on or off accordingly
*/
void vTask_ControlLamps(void *parameters)
{
	uint8_t data[11];
	int i=0;
	TickType_t xLastWakeTime;
  const TickType_t xFrequency = 50;

  // Initialise the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();
	
	for(;;)	//run task continuously
	{
		if(hcan1.pRxMsg->StdId==MSG_INT_EXT_LIGHT)
		{	
			
			//set message_received_flag to 1 so that USB task can send this message to PC
			message_received_flag = 1;
			
			/* For high beam */
			if(hcan1.pRxMsg->Data[0] & SIGNAL_HIGH_BEAM){
				HAL_GPIO_WritePin(GPIOD, LAMP_HIGH_BEAM, GPIO_PIN_SET);
			}
			else{
				HAL_GPIO_WritePin(GPIOD, LAMP_HIGH_BEAM, GPIO_PIN_RESET);
			}
				
			/* For low beam */
			if((hcan1.pRxMsg->Data[0] & SIGNAL_LOW_BEAM)){
				HAL_GPIO_WritePin(GPIOD, LAMP_LOW_BEAM, GPIO_PIN_SET);
			}
			else{
				HAL_GPIO_WritePin(GPIOD, LAMP_LOW_BEAM, GPIO_PIN_RESET);
			}
			
			/* For left turn indicator */
			if((hcan1.pRxMsg->Data[0] & SIGNAL_LEFT_TURN_INDICATOR)){
				HAL_GPIO_WritePin(GPIOD, LAMP_TURN_LEFT, GPIO_PIN_SET);
			}
			else{
				HAL_GPIO_WritePin(GPIOD, LAMP_TURN_LEFT, GPIO_PIN_RESET);
			}
			
			/* For right turn indicator */
			if((hcan1.pRxMsg->Data[0] & SIGNAL_RIGHT_TURN_INDICATOR)){
				HAL_GPIO_WritePin(GPIOD, LAMP_TURN_RIGHT, GPIO_PIN_SET);
			}
			else{
				HAL_GPIO_WritePin(GPIOD, LAMP_TURN_RIGHT, GPIO_PIN_RESET);
			}
			
			/* For marker light */
			if((hcan1.pRxMsg->Data[0] & SIGNAL_MARKER_LIGHT)){
				HAL_GPIO_WritePin(GPIOD, LAMP_MARKER, GPIO_PIN_SET);
			}
			else{
				HAL_GPIO_WritePin(GPIOD, LAMP_MARKER, GPIO_PIN_RESET);
			}
			
			/* For DRL */
			if((hcan1.pRxMsg->Data[0] & SIGNAL_DRL)){
				HAL_GPIO_WritePin(GPIOD, LAMP_DRL, GPIO_PIN_SET);
			}
			else{
				HAL_GPIO_WritePin(GPIOD, LAMP_DRL, GPIO_PIN_RESET);
			}
			
			/* For emergency stop */
			if((hcan1.pRxMsg->Data[0] & SIGNAL_EMERGENCY_STOP)){
				HAL_GPIO_WritePin(GPIOD, LAMP_EMERGENCY_STOP, GPIO_PIN_SET);
			}
			else{
				HAL_GPIO_WritePin(GPIOD, LAMP_EMERGENCY_STOP, GPIO_PIN_RESET);
			}
			
			/* For front fog lamp */
			if((hcan1.pRxMsg->Data[0] & SIGNAL_FRONT_FOG_LAMP)){
				HAL_GPIO_WritePin(GPIOD, LAMP_FRONT_FOG, GPIO_PIN_SET);
			}
			else{
				HAL_GPIO_WritePin(GPIOD, LAMP_FRONT_FOG, GPIO_PIN_RESET);
			}
			
			/* For rear fog lamp */
			if((hcan1.pRxMsg->Data[1] & SIGNAL_REAR_FOG_LAMP)){
				HAL_GPIO_WritePin(GPIOD, LAMP_REAR_FOG, GPIO_PIN_SET);
			}
			else{
				HAL_GPIO_WritePin(GPIOD, LAMP_REAR_FOG, GPIO_PIN_RESET);
			}
			
			/* For tail lamp */
			if((hcan1.pRxMsg->Data[1] & SIGNAL_TAIL_LAMP)){
				HAL_GPIO_WritePin(GPIOD, LAMP_TAIL, GPIO_PIN_SET);
			}
			else{
				HAL_GPIO_WritePin(GPIOD, LAMP_TAIL, GPIO_PIN_RESET);
			}
			
			/* For brake lamp */
			if((hcan1.pRxMsg->Data[1] & SIGNAL_BRAKE_LAMP)){
				HAL_GPIO_WritePin(GPIOD, LAMP_BRAKE, GPIO_PIN_SET);
			}
			else{
				HAL_GPIO_WritePin(GPIOD, LAMP_BRAKE, GPIO_PIN_RESET);
			}
			
			/* For dome lamp */
			if((hcan1.pRxMsg->Data[1] & SIGNAL_DOME_LAMP)){
				HAL_GPIO_WritePin(GPIOD, LAMP_DOME, GPIO_PIN_SET);
			}
			else{
				HAL_GPIO_WritePin(GPIOD, LAMP_DOME, GPIO_PIN_RESET);
			}
			
			
				i=0;
		
				data[i++] =  hcan1.pRxMsg->StdId;
				data[i++] = (hcan1.pRxMsg->StdId) >> 8;
				data[i++] =  hcan1.pRxMsg->DLC;
			
				for(; i<11; i++)
				{
					data[i] = hcan1.pRxMsg->Data[i-3];
				}
				
				if(USBD_OK == USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, data, sizeof(data)))
				{
				//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
				}
				else
				{
				//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
				}
			}							
		
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	
	}
	
}

/* USER CODE END 4 */
/*
Task: 			 vTask_SendDataToPc	
Parameters:  void
Return: 		 void
Description: This FreeRTOS thread sends all CAN messages seen on CAN bus to PC for analysis.
*/
void vTask_SendDataToPc(void *parameters)
{
	 /* init code for USB_DEVICE */
  
	
	//Format: ID-Byte1, ID-Byte2, DLC, Bytes-0 to 7
	//uint8_t data[11]= {0x50, 0x03, 2, 0, 10, 20, 30, 40, 50, 60, 70};
	//int i=0;
	
	uint8_t data[11];
	int i=0;
	
	
  for(;;)
  {
			
			if(message_received_flag)
			{	
				
				message_received_flag = 0;
			}
			
			
			if(message_transmitted_flag)
			{
				
				
				message_transmitted_flag = 0;
			}
    
		vTaskDelay(10);
  }

}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
 
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
