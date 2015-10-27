#include "atp.h"
#include <string.h>
#include <stdio.h>


#define BUF_SIZE        (64)

#if !defined(ARRAY_SIZE)
    #define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
#endif

#if !defined(MIN)
    #define MIN(a, b)       ((a) < (b) ? (a) : (b))
#endif

#if !defined(MAX)
    #define MAX(a, b)       ((a) > (b) ? (a) : (b))
#endif
    
enum ATPStates
{
    kAT_CMDA,
    kAT_CMDT,
    kAT_DATA,
    kAT_END1,
};

static ATCMD_t *ATCMD;
static uint32_t CmdCnt;


void ATP_Init(ATCMD_t *CMD, uint32_t size)
{
    ATCMD = CMD;
    CmdCnt = size;
}

void ATP_Put(uint8_t ch)
{
    static enum ATPStates States = kAT_CMDA;
    static char buf[BUF_SIZE];
    static uint32_t i;
    uint32_t j, len;
    int ret;
    
    switch(States)
    {
        case kAT_CMDA:
            if(ch == 'A')
            {
                States = kAT_CMDT;
            }
            break;
        case kAT_CMDT:
            (ch == 'T')?(States = kAT_DATA):(States = kAT_CMDA);
            i = 0;
            break;
        case kAT_DATA:
            ((ch == '\r') || (ch == '\n') || (i == sizeof(buf)))?(States = kAT_END1):(buf[i++] = ch);
            break;
        case kAT_END1:
            if((ch == '\r') || (ch == '\n'))
            {
                buf[i] = 0;
                for(j=0; j<CmdCnt; j++)
                {
                    (strchr(buf, '='))?(len = (uint32_t)(strchr(buf, '=') - buf)):(len = i);
                    
                    if(!strncmp(ATCMD[j].name, buf, MAX(len, strlen(ATCMD[j].name))))
                    {
                        char * argv[1];
                        argv[0] = (char*)(buf + strlen(ATCMD[j].name));
                        ret = ATCMD[j].cmd(1, argv);
                        (ret)?(printf("ERR\r\n")):(printf("OK\r\n"));
                        memset(buf, 0, sizeof(buf));
                        break;
                    }
                }
            }
            States = kAT_CMDA;
            break;
    }
}








