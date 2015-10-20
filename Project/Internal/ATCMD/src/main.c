#include "gpio.h"
#include "common.h"
#include "uart.h"

#include <string.h>

enum ATPStates
{
    kAT_CMDA,
    kAT_CMDT,
    kAT_DATA,
    kAT_END1,
};

/*
复位
AT+RST
设置波特率
AT+UART=115200,0,0
所有固件信息
AT+INFO
设置工作模式
AT+CMODE=0
输出原始数据
AT+RAW
数据输出频率
AT+OUTFREQ=100
设置校准值
AT+CAL(A/G/M)=10,33,55
烧录所有配置信息
AT+SAVE

*/

typedef struct  
{
    char		*name;
    int         (*cmd)(int argc, char * const argv[]);
}ATCMD_t;


int ATRST(int argc, char * const argv[])
{
    NVIC_SystemReset();
}

int AT(int argc, char * const argv[])
{
    return 0;
}

int INFO(int argc, char * const argv[])
{
    printf("INFO\r\n");
    return 1;
}


    
ATCMD_t ATCMD[] = 
{
    {"",        AT      },
    {"+RST",    ATRST   },
    {"+INFO",   INFO    },
};





void ATP_Get(uint8_t ch)
{
    static enum ATPStates States = kAT_CMDA;
    static char buf[64];
    static uint32_t i;
    uint32_t j;
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
                for(j=0; j<ARRAY_SIZE(ATCMD); j++)
                {
                    if(strchr(buf, '='))
                    {
                        i = (uint32_t)(strchr(buf, '=') - buf);
                    }
                    
                    if(!strncmp(ATCMD[j].name, buf, i))
                    {
                        ret = ATCMD[j].cmd(NULL, NULL);
                        (ret)?(printf("ERR\r\n")):(printf("OK\r\n"));
                        break;
                    }
                }
            }
            States = kAT_CMDA;
            break;
    }
}

static void UART_RX_ISR(uint16_t byteReceived)
{
    ATP_Get(byteReceived);
    //UART_WriteByte(HW_UART0, byteReceived);
}

 
int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    
    UART_QuickInit(UART0_RX_PB16_TX_PB17 , 115200);
    
    printf("type any character which will echo...\r\n");

    UART_CallbackRxInstall(HW_UART0, UART_RX_ISR);
    UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


