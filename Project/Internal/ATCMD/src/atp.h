#ifndef __ATP_H__
#define __ATP_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct  
{
    char		*name;
    int         (*cmd)(int argc, char * const argv[]);
}ATCMD_t;

#define ATP_INIT(x)     x, ARRAY_SIZE(x)

void ATP_Init(ATCMD_t *CMD, uint32_t size);
void ATP_Put(uint8_t ch);



#endif
