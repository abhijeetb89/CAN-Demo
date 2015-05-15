/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @date    15/05/2015 11:09:07
  * @brief   Interrupt Service Routines.
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
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_hal_can.h"
#include "cmsis_os.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern void xPortSysTickHandler(void);
extern HCD_HandleTypeDef hhcd_USB_OTG_FS;
extern CAN_HandleTypeDef hcan1;
/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  osSystickHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles USB On The Go FS global interrupt.
*/
void OTG_FS_IRQHandler(void)
{
  /* USER CODE BEGIN OTG_FS_IRQn 0 */

  /* USER CODE END OTG_FS_IRQn 0 */
  HAL_HCD_IRQHandler(&hhcd_USB_OTG_FS);
  /* USER CODE BEGIN OTG_FS_IRQn 1 */

  /* USER CODE END OTG_FS_IRQn 1 */
}

/* USER CODE BEGIN 1 */

void CAN1_RX0_IRQHandler(void)
{
			hcan1.pRxMsg->StdId = ((CAN1->sFIFOMailBox[0].RIR) & (0xFFE00000))>> 21;
	
	    hcan1.pRxMsg->DLC = (CAN1->sFIFOMailBox[0].RDTR & (0xF));
	
			hcan1.pRxMsg->Data[0] = (CAN1->sFIFOMailBox[0].RDLR & 0x000000FF);
	    hcan1.pRxMsg->Data[1] = (CAN1->sFIFOMailBox[0].RDLR & 0x0000FF00)>>8;
	    hcan1.pRxMsg->Data[2] = (CAN1->sFIFOMailBox[0].RDLR & 0x00FF0000)>>16;
	    hcan1.pRxMsg->Data[3] = (CAN1->sFIFOMailBox[0].RDLR & 0xFF000000)>>24;
		  hcan1.pRxMsg->Data[4] = (CAN1->sFIFOMailBox[0].RDHR & 0x000000FF);
	    hcan1.pRxMsg->Data[5] = (CAN1->sFIFOMailBox[0].RDHR & 0x0000FF00)>>8;
	    hcan1.pRxMsg->Data[6] = (CAN1->sFIFOMailBox[0].RDHR & 0x00FF0000)>>16;
	    hcan1.pRxMsg->Data[7] = (CAN1->sFIFOMailBox[0].RDHR & 0xFF00000)>>24;
	
	    hcan1.pRxMsg->RxTime = (CAN1->sFIFOMailBox[0].RDTR & 0xFFFF0000) >> 16;
	    
		  CAN1->RF0R |= CAN_RF0R_RFOM0;
	
}


/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
