#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "stdinclude.h"

//#define DEBUG


#ifdef DEBUG
#define DMSG(args...)       printf(args);
#define DMSG_STR(str)       printf(str);
#define DMSG_HEX(num)       putchar(' '); printf("%08X", num);
#define DMSG_INT(num)/*       Serial.print(' '); Serial.print(num)*/
#else
#define DMSG(args...)
#define DMSG_STR(str)
#define DMSG_HEX(num)
#define DMSG_INT(num)
#endif

#endif
