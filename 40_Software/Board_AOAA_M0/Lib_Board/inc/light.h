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
#ifndef __LIGHT_H
#define __LIGHT_H


typedef enum
{
    LIGHT_MODE_D1,
    LIGHT_MODE_D2,
    LIGHT_MODE_D1D2
} light_mode_t;

typedef enum
{
    LIGHT_WIDTH_16BITS,
    LIGHT_WIDTH_12BITS,
    LIGHT_WIDTH_08BITS,
    LIGHT_WIDTH_04BITS
} light_width_t;

typedef enum
{
    LIGHT_RANGE_1000,
    LIGHT_RANGE_4000,
    LIGHT_RANGE_16000,
    LIGHT_RANGE_64000
} light_range_t;

typedef enum
{
    LIGHT_CYCLE_1,
    LIGHT_CYCLE_4,
    LIGHT_CYCLE_8,
    LIGHT_CYCLE_16
} light_cycle_t;


void light_init (void);
void light_enable (void);
uint32_t light_read(void);
void light_setMode(light_mode_t mode);
void light_setWidth(light_width_t width);
void light_setRange(light_range_t newRange);
void light_setHiThreshold(uint32_t luxTh);
void light_setLoThreshold(uint32_t luxTh);
void light_setIrqInCycles(light_cycle_t cycles);
uint8_t light_getIrqStatus(void);
void light_clearIrqStatus(void);
void light_shutdown(void);


#endif /* end __LIGHT_H */
/****************************************************************************
**                            End Of File
*****************************************************************************/
