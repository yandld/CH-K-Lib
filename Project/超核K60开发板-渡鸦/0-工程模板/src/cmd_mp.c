#include "shell.h"
#include "common.h"
#include <stdlib.h>
#include <stdio.h>

#include "mempool.h"

static mempool mp;
static uint8_t mp1[1024];
static uint8_t *ptr[100];
int CMD_MP(int argc, char * const * argv)
{
    uint32_t i;
    uint8_t *ap; //有效的指针
    printf("MEMORY POOL TEST\r\n");
    mp_init(&mp, "mp1", &mp1[0], sizeof(mp1), 64);
    for(i=0;i<100;i++)
    {
        ptr[i] = mp_alloc(&mp);
        if(ptr[i])
        {
            ap = ptr[i];
            printf("malloc succ %p\r\n", ptr[i]);
        }
        else
        {
            printf("malloc fail %p\r\n", ptr[i]);
            printf("free %p\r\n", ap);
            mp_free(ap);
        }
    }
    return 0;
}



const cmd_tbl_t CommandFun_BUZZER = 
{
    .name = "MP",
    .maxargs = 4,
    .repeatable = 1,
    .cmd = CMD_MP,
    .usage = "FTM <CMD> (CMD = M,F)",
    .complete = NULL,
    .help = "FTM <CMD> (CMD = M,F)",
};
