#include "shell.h"
#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include "sram.h"

#include "mempool.h"

#define MP_SIZE         (1024*16)
#define MP_BLOCK_SIZE   (512)


static mempool mp;
//static uint8_t mp1[1024];
uint8_t *mp1 = SRAM_START_ADDRESS;
//static uint8_t *ptr[100];

int CMD_MP(int argc, char * const * argv)
{
    uint32_t i;
    uint8_t *ap; //有效的指针
    uint8_t **ptr; //存放malloc出来的指针数据 它也从malloc分配
    printf("MEMORY POOL TEST\r\n");
    mp_init(&mp, "mp1", &mp1[0], MP_SIZE, MP_BLOCK_SIZE);
    ptr = (uint8_t **)mp_alloc(&mp);
    for(i=0;i<100;i++)
    {
        ptr[i] = mp_alloc(&mp);
        if(ptr[i])
        {
            ap = ptr[i];
            printf("malloc succ %p\r\n", ptr[i]);
            //memcpy(ap, "I am storted in mp\r\n", MP_BLOCK_SIZE);
           // printf(ap);
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


const cmd_tbl_t CommandFun_MP = 
{
    .name = "MP",
    .maxargs = 4,
    .repeatable = 1,
    .cmd = CMD_MP,
    .usage = "FTM <CMD> (CMD = M,F)",
    .complete = NULL,
    .help = "FTM <CMD> (CMD = M,F)",
};
