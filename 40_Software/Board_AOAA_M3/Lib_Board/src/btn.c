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
#include "btn.h"

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
 *    Initialize Buttons driver
 *
 *****************************************************************************/
void btn_init (void)
{
	// SW2
    GPIO_SetDir( 2, (1<<11), 0 );
    // SW3
    GPIO_SetDir( 2, (1<<12), 0 );
}


/******************************************************************************
 *
 * Description:
 *    Get buttons state
 *
 * Returns:
 *    buttons state
 *
 *****************************************************************************/
uint8_t btn_get (void)
{
	uint32_t v = GPIO_ReadValue(2);
	uint8_t res = 0;

	if ((v & (1<<11)) == 0) {
		res |= BTN_SW2;
	}
	if ((v & (1<<12)) == 0) {
		res |= BTN_SW3;
	}

	return res;

}
