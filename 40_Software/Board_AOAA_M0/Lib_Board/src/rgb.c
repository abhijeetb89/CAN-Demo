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

#include "mcu_regs.h"
#include "type.h"
#include "gpio.h"
#include "rgb.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/

//#define P0_10_SWCLK_AS_GPIO

/******************************************************************************
 * External global variables
 *****************************************************************************/

/******************************************************************************
 * Local variables
 *****************************************************************************/

/******************************************************************************
 * Local Functions
 *****************************************************************************/

/******************************************************************************
 * Public Functions
 *****************************************************************************/

/******************************************************************************
 *
 * Description:
 *    Initialize RGB driver
 *
 *****************************************************************************/
void rgb_init (void)
{
#ifdef P0_10_SWCLK_AS_GPIO
	LPC_IOCON->SWCLK_PIO0_10 &= ~0x07;
	LPC_IOCON->SWCLK_PIO0_10 |= 0x01;
#endif


	// LED12
	GPIOSetDir( 0, 8, 1 );
	GPIOSetDir( 0, 9, 1 );
	GPIOSetDir( 0, 10, 1 );


    // turn leds off
	GPIOSetValue( 0, 8, 1);
	GPIOSetValue( 0, 9, 1);
	GPIOSetValue( 0, 10, 1);
}


/******************************************************************************
 *
 * Description:
 *    Set LED states
 *
 * Params:
 *    [in]  ledMask  - The mask is used to turn LEDs on or off
 *
 *****************************************************************************/
void rgb_setLeds (uint8_t ledOnMask, uint8_t ledOffMask)
{

  if ((ledOffMask & RGB_RED) != 0 ){
    GPIOSetValue( 0, 8, 1 );
  }
  if ((ledOffMask & RGB_BLUE) != 0 ){
    GPIOSetValue( 0, 9, 1 );
  }
  if ((ledOffMask & RGB_GREEN) != 0 ){
    GPIOSetValue( 0, 10, 1 );
  }


	if ((ledOnMask & RGB_RED) != 0) {
		GPIOSetValue( 0, 8, 0);
	}
  if ((ledOnMask & RGB_BLUE) != 0) {
    GPIOSetValue( 0, 9, 0);
  }
  if ((ledOnMask & RGB_GREEN) != 0) {
    GPIOSetValue( 0, 10, 0);
  }

}
