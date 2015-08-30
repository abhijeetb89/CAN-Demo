/*****************************************************************************
 *
 *   Copyright(C) 2011, Embedded Artists AB
 *   All rights reserved.
 *
 ******************************************************************************/
#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__

/* Include some files for defining library routines */
#include <string.h>

/* Define platform endianness */
#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif /* BYTE_ORDER */

/* Define generic types used in lwIP */
typedef unsigned   char    u8_t;
typedef signed     char    s8_t;
typedef unsigned   short   u16_t;
typedef signed     short   s16_t;
typedef unsigned   long     u32_t;
typedef signed     long     s32_t;

typedef unsigned long mem_ptr_t;

/* Define (sn)printf formatters for these lwIP types */
#define X8_F  "x"
#define U16_F "u"
#define S16_F "d"
#define X16_F "x"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"

/* If only we could use C99 and get %zu */
#if defined(__x86_64__)
#define SZT_F "lu"
#else
#define SZT_F "u"
#endif

/* Compiler hints for packing structures */
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

/* prototypes for printf() and abort() */
#include <stdio.h>
#include <stdlib.h>
#include "eadebug.h"
/* Plaform specific diagnostic output */
#define LWIP_PLATFORM_DIAG(x) do {dbg("%8d: ", time_get());/*dbg("%s:%d\r\n", __FILE__, __LINE__);*/ dbg x; /*dbg("\r\n")*/}while(0)

#define LWIP_PLATFORM_ASSERT(x) do {dbg("Assertion \"%s\" failed at line %d in %s\r\n", x, __LINE__, __FILE__);} while(0)



#define LWIP_RAND() ((u32_t)rand())

#endif /* __ARCH_CC_H__ */
