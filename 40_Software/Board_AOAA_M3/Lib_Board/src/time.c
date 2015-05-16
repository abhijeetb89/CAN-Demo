/*****************************************************************************
 *
 *   Copyright(C) 2011, Embedded Artists AB
 *   All rights reserved.
 *
 ******************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * Embedded Artists AB assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. Embedded Artists AB
 * reserves the right to make changes in the software without
 * notification. Embedded Artists AB also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/

#include "lpc17xx_timer.h"
#include "time.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/

/******************************************************************************
 * External global variables
 *****************************************************************************/

/******************************************************************************
 * Local variables
 *****************************************************************************/

static uint32_t timeMs = 0;

/******************************************************************************
 * Local Functions
 *****************************************************************************/

/******************************************************************************
 * Public Functions
 *****************************************************************************/

/******************************************************************************
 *
 * Description:
 *    Initialize time functions
 *
 *****************************************************************************/
void time_init (void)
{
  TIM_TIMERCFG_Type TIM_ConfigStruct;
  TIM_MATCHCFG_Type TIM_MatchConfigStruct ;

  // Initialize timer 0, prescale count time of 100uS
  TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
  TIM_ConfigStruct.PrescaleValue  = 100;

  // use channel 0, MR0
  TIM_MatchConfigStruct.MatchChannel = 0;
  // Enable interrupt when MR0 matches the value in TC register
  TIM_MatchConfigStruct.IntOnMatch   = TRUE;
  //Enable reset on MR0: TIMER will reset if MR0 matches it
  TIM_MatchConfigStruct.ResetOnMatch = TRUE;
  //Stop on MR0 if MR0 matches it
  TIM_MatchConfigStruct.StopOnMatch  = FALSE;
  //Toggle MR0.0 pin if MR0 matches it
  TIM_MatchConfigStruct.ExtMatchOutputType =TIM_EXTMATCH_NOTHING;
  // Set Match value, count value of 10 (10 * 100uS = 1000uS = 1ms --> 1000 Hz)
  TIM_MatchConfigStruct.MatchValue   = 10;

  TIM_Init(LPC_TIM1, TIM_TIMER_MODE,&TIM_ConfigStruct);
  TIM_ConfigMatch(LPC_TIM1,&TIM_MatchConfigStruct);

  NVIC_EnableIRQ(TIMER1_IRQn);
  // To start timer 0
  TIM_Cmd(LPC_TIM1,ENABLE);

}


/******************************************************************************
 *
 * Description:
 *    Get time in ms since startup
 *
 * Returns:
 *    time in ms
 *
 *****************************************************************************/
uint32_t time_get (void)
{
  return timeMs;
}


/******************************************************************************
 *
 * Description:
 *   Timer interrupt handler
 *
 *****************************************************************************/
void TIMER1_IRQHandler(void)
{
  timeMs++;
  TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);
}
