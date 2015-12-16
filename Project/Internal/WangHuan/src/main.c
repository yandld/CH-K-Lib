#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"
#include "pit.h"

/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请使用 CMSIS标准文件 startup_MKxxxx.c 中的 CLOCK_SETUP 宏 */

/* UART 快速初始化结构所支持的引脚* 使用时还是推荐标准初始化 */
/*
 UART1_RX_PE01_TX_PE00   
 UART0_RX_PF17_TX_PF18   
 UART3_RX_PE05_TX_PE04   
 UART5_RX_PF19_TX_PF20   
 UART5_RX_PE09_TX_PE08   
 UART2_RX_PE17_TX_PE16   
 UART4_RX_PE25_TX_PE24   
 UART0_RX_PA01_TX_PA02   
 UART0_RX_PA15_TX_PA14   
 UART3_RX_PB10_TX_PB11   
 UART0_RX_PB16_TX_PB17   
 UART1_RX_PC03_TX_PC04   
 UART4_RX_PC14_TX_PC15   
 UART3_RX_PC16_TX_PC17   
 UART2_RX_PD02_TX_PD03   
 UART0_RX_PD06_TX_PD07   
 UART2_RX_PF13_TX_PF14   
 UART5_RX_PD08_TX_PD09   
*/
 
 /*
     实验名称：UART打印信息
     实验平台：凤凰开发板
     板载芯片：MK64DN512ZVQ10
 实验效果：使用串口UART将芯片的出厂信息在芯片上电后发送出去
        发送完毕后，进入while中，执行小灯闪烁效果
*/
#define KEY3  PAin(7)  //定义PTA端口的7引脚为输入
#define LED_R  PAout(9)  //定义PTA端口的9引脚输出控制
 
 /* 按键返回状态 0未按 1按下 */
#define NO_KEY          (0x00)
#define KEY_SINGLE      (0x01)
 /* 按键返回值 */
static uint8_t gRetValue;

#define REV_IDLE        (0x01)
#define REV_CNT1 		    (0x02)
#define REV_CNT2  		  (0x03)
#define REV_CNT3  		  (0x04)
#define REV_WIDE        (0x05)
#define REV_WIDE2       (0x06)
#define REV_RATE        (0x07)
#define REV_WAIT				(0x08)

uint32_t PulseCnt = 20;
uint32_t CurrentPulseCnt = 0;
uint32_t CurrentCnt = 0;
uint32_t PulseWide = 6; //us
uint8_t PulseRate = 5; // speed rate
uint8_t CurrentLevel = 0;
uint8_t CurrentRate = 0;
uint32_t CurrentPulseWide = 0;
uint32_t old_CurrentCnt = 0;
uint8_t i = 0;
uint8_t timer_on =0;

/* 状态机表 */
typedef enum
{
    kKEY_Idle,          /*空闲态 */
    kKEY_Debounce,      /*确认与消抖态 */
    kKEY_Confirm,       /*确认按键状态*/
}KEY_Status;


/* PIT0中断服务函数 */
//此函数中编写用户中断需要做的事情
static void PIT_ISR(void)
{
	CurrentCnt++; //record how low this state last
	
    //static uint32_t i;
    //中断中所做的事情
    //printf("enter PIT interrupt! %d\r\n", i++); 
    /* 闪烁小灯 */
    //GPIO_ToggleBit(HW_GPIOA, 9);
	if (CurrentLevel == 1)
	{
		PAout(4)=1;
	}
	else
	{
		PAout(4)=0;
	}
		
}

/*
static void PIT_ISR(void)
{
    static uint32_t i;
    //中断中所做的事情
    printf("enter PIT interrupt! %d\r\n", i++); 
    // 闪烁小灯
    GPIO_ToggleBit(HW_GPIOA, 9);
}
*/

static void UART_RX_ISR(uint16_t ch)
{
    static uint32_t State = REV_IDLE;
		//printf("recive");
    switch(State)
    {
        case REV_IDLE:
            if(ch == 0x55)
            {
                State = REV_CNT1;
            }
        break;
        case REV_CNT1:
            PulseCnt = ch;
            State = REV_CNT2;
        break;
				case REV_CNT2:
            PulseCnt = ch+(PulseCnt<<8);
            State = REV_CNT3;
        break;
				case REV_CNT3:
            PulseCnt = ch+(PulseCnt<<8);
            State = REV_WIDE;
        break;
				case REV_WIDE:
            PulseWide = ch;
            State = REV_WIDE2;
        break;
				case REV_WIDE2:
            PulseWide = ch+(PulseWide<<8);
            State = REV_RATE;
        break;
        case REV_RATE:
            PulseRate = ch;
            State = REV_IDLE;
				printf("Param Configuration: PulseCnt:%d  PulseWide:%d PulseRate:%d\r\n", PulseCnt, PulseWide, PulseRate);
        break;
    }

}

/* 按键扫描程序 */
static void KEY_Scan(void)
{
    static KEY_Status status = kKEY_Idle;
    switch(status)
    {
        case kKEY_Idle:
            gRetValue = NO_KEY;
            if(KEY3 == 0) /* 如果按键被按下 进入确认与消抖态 */
            {
                status = kKEY_Debounce;
            }
            break;
        case kKEY_Debounce: /* 确认与消抖态 */
            if(KEY3 == 0)
            {
                status = kKEY_Confirm;
            }
            else
            {
                status = kKEY_Idle;
                gRetValue = NO_KEY;
            }
            break;
        case kKEY_Confirm: /* 确认态 */
            if(KEY3 == 1) /* 按键松开 */
            {
                gRetValue = KEY_SINGLE;
                status = kKEY_Idle;
            }
            break;
        default:
            break;
    }
}
 
 void GenPulse(uint32_t cnt, uint32_t us)
 {
	 uint8_t Rate = 0;

	
	 cnt = cnt - 2*PulseRate;
	 Rate = PulseRate;
	 

	  do
		{
			PAout(4)=1;
			DelayUs(us*Rate);
			PAout(4)=0;
			DelayUs(us*Rate);
			
			Rate--;
		
		}while(Rate > 0x00);
		
		while(cnt--)
		{
			//GPIO_WriteBit(HW_GPIOA, 9, 1);
			//GPIO_WriteBit(HW_GPIOA, 4, 1);
			PAout(4)=1;
			DelayUs(us);
			PAout(4)=0;
			DelayUs(us);
			//GPIO_WriteBit(HW_GPIOA, 9, 0);
			//GPIO_WriteBit(HW_GPIOA, 4, 0);	
		}
		
		do
		{
			Rate++;
			PAout(4)=1;
			DelayUs(us*Rate);
			PAout(4)=0;
			DelayUs(us*Rate);
		
		}while(PulseRate>Rate);

 }
 
 void CalPulse(uint32_t PulseCnt, uint32_t PulseWide)
 {
	 //uint8_t Rate = 0;
	 //uint16_t PulseWide = 0;
	/*
	 if (old_CurrentCnt == CurrentCnt)
	{
		if (CurrentLevel == 0)
		{
		CurrentLevel = 1; //high vol
		//CurrentCnt=0;
		}
		else
		{
		CurrentLevel = 0; //low vol
		//CurrentCnt= 0;
		}
	
	}
	old_CurrentCnt = CurrentCnt;
	*/
	 
	CurrentPulseWide = PulseWide * CurrentRate;
	 
	if (CurrentPulseCnt<=PulseCnt && timer_on==1) //still have pulse to sent out
	{
		if (old_CurrentCnt == CurrentCnt)// less than 1us
		{}
		else
		{
			if (CurrentCnt < CurrentPulseWide)//reach 1us and not long enough
			{}
			else//reach 1us and long enough 
			{
				if (CurrentLevel == 0)
				{
					CurrentLevel = 1; //high vol
					CurrentCnt=0;
				}
				else
				{
					CurrentLevel = 0; //low vol
					CurrentCnt= 0;
					
					if(CurrentRate>1 && CurrentPulseCnt<=(PulseRate)) //start up process 
					{
						CurrentRate--;//reach 1us and long enough than move to next pulse
						CurrentPulseCnt++;
					}
					else 
					{
						if (CurrentPulseCnt>=(PulseCnt-PulseRate) && CurrentPulseCnt<=PulseCnt) // slow down process
						{
							CurrentRate++;
							CurrentPulseCnt++;
						}
						else
						{
							CurrentRate = 1;//normal running process
							CurrentPulseCnt++;
						}
					}
				}
				
			}
			
		}
		
	}
	else
	{
		timer_on = 0;
		PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, false);
	}
	old_CurrentCnt = CurrentCnt;

 }

 
int main(void)
{
    uint32_t clock;
    DelayInit();
	
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOA, 4, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOA, 5, kGPIO_Mode_OPP);
    //GPIO_QuickInit(PULSE_PORT, PULSE_PIN, kGPIO_Mode_OPP);
    /* 将GPIO设置为输入模式 芯片内部自动配置上拉电阻 */
    GPIO_QuickInit(HW_GPIOA, 7, kGPIO_Mode_IPU);
    /* 设置为输出 */
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);  
    /*  配置UART 中断配置 打开接收中断 安装中断回调函数 */
    UART_CallbackRxInstall(HW_UART0, UART_RX_ISR);
    /* 打开串口接收中断功能 IT 就是中断的意思*/
    UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
	
	
		/* 初始化PIT模块 */
    PIT_InitTypeDef PIT_InitStruct1;  //申请结构体变量
    PIT_InitStruct1.chl = HW_PIT_CH0; /* 使用0号定时器 */
    PIT_InitStruct1.timeInUs = 1; /* 定时周期1uS */
    PIT_Init(&PIT_InitStruct1); //pit模块初始化
    /* 注册PIT 中断回调函数 */
    PIT_CallbackInstall(HW_PIT_CH0, PIT_ISR); //0号定时器的中断处理

		
    /* 打印时钟频率 */
    clock = GetClock(kCoreClock);
    printf("core clock:%dHz\r\n", clock);
    clock = GetClock(kBusClock);
    printf("bus clock:%dHz\r\n", clock);
    GPIO_WriteBit(HW_GPIOA, 5, 0);
	
    CurrentRate = PulseRate;
    while(1)
    {
        KEY_Scan(); //调用按键扫描程序  
        /* 闪烁小灯 */
        //GPIO_ToggleBit(HW_GPIOA, 9);
        if(gRetValue == KEY_SINGLE) //按键按下，小灯反正
        {
            LED_R = !LED_R;
            /* 开启PIT0定时器中断 */
            PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, true);
            timer_on = 1;
            CurrentPulseCnt = 0;
            CurrentRate = PulseRate;

            //GenPulse(PulseCnt, PulseWide);  
        }
            CalPulse(PulseCnt, PulseWide);  
				
    }
}


