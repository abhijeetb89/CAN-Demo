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

#include "lpc17xx_gpio.h"
#include "rgb.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/

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
	// LED6
    GPIO_SetDir( 2, (1<<0), 1 );
    GPIO_SetDir( 2, (1<<1), 1 );
    GPIO_SetDir( 2, (1<<2), 1 );
    // turn leds off
    GPIO_SetValue( 2, (1<<0|1<<1|1<<2));

    // LED7
    GPIO_SetDir( 2, (1<<3), 1 );
    GPIO_SetDir( 2, (1<<4), 1 );
    GPIO_SetDir( 2, (1<<5), 1 );
    GPIO_SetValue( 2, (1<<3|1<<4|1<<5));
}


/******************************************************************************
 *
 * Description:
 *    Set LED states
 *
 * Params:
 *    [in]  ledOnMask  - The mask for LEDs to turn on
 *    [in]  ledOnMask  - The mask for LEDs to turn off
 *
 *****************************************************************************/
void rgb_setLeds (rgb_led_t led, uint8_t ledOnMask, uint8_t ledOffMask)
{
	if (led == LED_6) {

    if ((ledOffMask & RGB_RED) != 0) {
      GPIO_SetValue( 2, (1<<0));
    }
    if ((ledOffMask & RGB_BLUE) != 0) {
      GPIO_SetValue( 2, (1<<1) );
    }
    if ((ledOffMask & RGB_GREEN) != 0) {
      GPIO_SetValue( 2, (1<<2) );
    }

	  if ((ledOnMask & RGB_RED) != 0) {
      GPIO_ClearValue( 2, (1<<0) );
	  }
    if ((ledOnMask & RGB_BLUE) != 0) {
      GPIO_ClearValue( 2, (1<<1) );
    }
    if ((ledOnMask & RGB_GREEN) != 0) {
      GPIO_ClearValue( 2, (1<<2) );
    }

	}
	else {

    if ((ledOffMask & RGB_RED) != 0) {
      GPIO_SetValue( 2, (1<<3));
    }
    if ((ledOffMask & RGB_BLUE) != 0) {
      GPIO_SetValue( 2, (1<<4) );
    }
    if ((ledOffMask & RGB_GREEN) != 0) {
      GPIO_SetValue( 2, (1<<5) );
    }

    if ((ledOnMask & RGB_RED) != 0) {
      GPIO_ClearValue( 2, (1<<3) );
    }
    if ((ledOnMask & RGB_BLUE) != 0) {
      GPIO_ClearValue( 2, (1<<4) );
    }
    if ((ledOnMask & RGB_GREEN) != 0) {
      GPIO_ClearValue( 2, (1<<5) );
    }


	}


}
