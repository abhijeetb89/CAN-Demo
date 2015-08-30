/*****************************************************************************
 *
 *   Copyright(C) 2011, Embedded Artists AB
 *   All rights reserved.
 *
 ******************************************************************************/

#include "lwip/opt.h"

#include "lwip/sys.h"


// available in Lib_Board/time.c
u32_t time_get();

u32_t sys_now(void)
{
  return time_get();
}



