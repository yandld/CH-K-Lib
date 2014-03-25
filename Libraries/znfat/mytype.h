#ifndef _MYTYPE_H_
#define _MYTYPE_H_

/*============================================================================================
  此文件用于进行数据类型的重新定义，请按照实际使用的硬件平台上的数据类型对宏进行重新定义
=============================================================================================*/

//加入类型相关头文件 比如AVR GCC中的 ROM类型在<AVR/pgmspace.h>中定义

#define UINT8   unsigned char
#define UINT16  unsigned short
#define UINT32  unsigned int 

#define INT8    char 
#define INT16   short
#define INT32   int

#define ROM_TYPE_UINT8  const unsigned char 
#define ROM_TYPE_UINT16 const unsigned short 
#define ROM_TYPE_UINT32 const unsigned int 

#endif
