#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"


#define SWD_CLK_INSTANCE    HW_GPIOE
#define SWD_DIO_INSTANCE    HW_GPIOE
#define SWD_CLK_PIN         26
#define SWD_DO_PIN          25

#define SWD_CLK             PEout(SWD_CLK_PIN)
#define SWD_DO              PEout(SWD_DO_PIN)
#define SWD_DIO_H           (SWD_DO = 1)
#define SWD_DIO_L           (SWD_DO = 0)


#define SWD_CLK_H           (SWD_CLK = 1)
#define SWD_CLK_L           (SWD_CLK = 0)

#define SWD_Delay()         DelayUs(1)
#define SWD_OUT             do {GPIO_PinConfig(SWD_DIO_INSTANCE, SWD_DO_PIN, kOutput);}while(0)
#define SWD_IN              do {GPIO_PinConfig(SWD_DIO_INSTANCE, SWD_DO_PIN, kInput);}while(0)

typedef uint8_t u8;
typedef uint32_t u32;

static inline uint8_t SWD_DII(void)
{
    return GPIO_ReadBit(SWD_DIO_INSTANCE, SWD_DO_PIN);
}

static void SWD_LineReset(void)
{
    u8 i; 
    SWD_OUT;
    SWD_DIO_H;
    for(i=0;i<56;i++)
    {
        SWD_Delay();
        SWD_CLK_H;
        SWD_Delay();
        SWD_CLK_L;
    }
    SWD_DIO_L;
    for(i=0;i<5;i++)
    {
        SWD_Delay();
        SWD_CLK_H;
        SWD_Delay();
        SWD_CLK_L;
    }
}

static void SwdSendByte(u8 dat)
{
    u8 i;
    SWD_OUT;
    for(i=0;i<8;i++)
    {  
        if(dat&0x80)
        {
            SWD_DIO_H;
        }
        else
        {
            SWD_DIO_L;
        }
        dat <<= 1; 
        SWD_CLK_H;
        SWD_Delay();
        SWD_CLK_L;
        SWD_Delay();
    }
}

static u32 SeqRead(u8 cmd)
{
    u32 dat=0;
    u8 i=0;
    SwdSendByte(cmd);
    SWD_IN;
    for(i=0;i<3;i++)
    { 
        SWD_CLK_H;
        SWD_Delay();
        SWD_CLK_L;
        SWD_Delay();
    }
    dat=0;
    for(i=0;i<32;i++)
    {
        dat=dat>>1; 
        SWD_CLK_H;
        SWD_Delay();
        SWD_CLK_L;
    if(SWD_DII())
    {
      dat |= 0x80000000;
    }
    SWD_Delay(); 
  }
    //parity
    SWD_IN; //trn
    for(i=0;i<2;i++)
    {
        SWD_CLK_H;
        SWD_Delay();
        SWD_CLK_L;
        SWD_Delay();
    }
    SWD_DIO_L;
    SWD_OUT;
    for(i=0;i<5;i++)
    {
        SWD_CLK_H;
        SWD_Delay();
        SWD_CLK_L;
        SWD_Delay();
        SWD_CLK_H;
        SWD_Delay();
        SWD_CLK_L;
        SWD_Delay();
    }
    SWD_IN;
    return dat;
}

void JTAG2SWD(void)
{
    u8 i; 
    SWD_OUT;
    SWD_DIO_H;
    for(i=0;i<56;i++)
    {
        SWD_Delay();
        SWD_CLK_H;
        SWD_Delay();
        SWD_CLK_L;
    }
    SWD_DIO_L;
    SwdSendByte(0x79);
    SwdSendByte(0xE7);
    SWD_LineReset();
    SwdSendByte(0x6D);
    SwdSendByte(0xB7);
    SWD_LineReset();
}

int main(void)
{
    uint32_t clock;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);

    GPIO_QuickInit(SWD_CLK_INSTANCE, SWD_CLK_PIN, kGPIO_Mode_OPP);
    GPIO_QuickInit(SWD_DIO_INSTANCE, SWD_DO_PIN, kGPIO_Mode_OPP);
    //PORT_PinPullConfig(SWD_DIO_INSTANCE, SWD_DO_PIN, kPullDown);
    //GPIO_QuickInit(SWD_DIO_INSTANCE, SWD_DI_PIN, kGPIO_Mode_IPU);
    
    printf("%s - %dP\r\n", CPUIDY_GetFamID(), CPUIDY_GetPinCount());

    CLOCK_GetClockFrequency(kCoreClock, &clock);
    printf("core clock:%dHz\r\n", clock);
    CLOCK_GetClockFrequency(kBusClock, &clock);
    printf("bus clock:%dHz\r\n", clock);
    
    JTAG2SWD();
    DelayMs(1);
    uint32_t val;
    val =  SeqRead(0xA5);
    printf("IDR:0x%X\r\n", val);
    val =  SeqRead(0xA5);
    printf("IDR:0x%X\r\n", val); 
    while(1)
    {
        /* ÉÁË¸Ð¡µÆ */
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


