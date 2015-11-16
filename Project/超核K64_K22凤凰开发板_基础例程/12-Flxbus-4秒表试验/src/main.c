#include "gpio.h"
#include "pit.h"
#include "common.h"
#include "uart.h"
#include "ili9320.h"
#include "gui.h"
#include "stdint.h"
#include "stdio.h"

/*
     实验名称：秒表实验
     实验平台：凤凰开发板
     板载芯片：MK64FN1MVLQ12
 实验效果：使用DMA模块实现脉冲计数功能，这期间需要使用DMA、PIT模块协同
       使用DMA的0通到连接到PTA5引脚脉冲计数
       使用DMA的1通道连接到PTB23引脚脉冲计数
       在PIT中断中处理脉冲计数结果
*/
//首先定义位操作的宏定义
#define KEY1	PAin(4)  
#define KEY2	PAin(6)
#define KEY3	PAin(7)
#define LED1  PAout(9)  //定义PTA端口的9引脚输出控制

/* 按键返回状态 0未按 1按下 */
#define NO_KEY          (0x00)
#define KEY_SINGLE      (0x01)
/* 按键返回值 */
static uint8_t gRetValue;

/* 状态机表 */
typedef enum
{
    kKEY_Idle,          /*空闲态 */
    kKEY_Debounce,      /*确认与消抖态 */
    kKEY_Confirm,       /*确认按键状态*/
}KEY_Status;

uint32_t hour,minute,second,millisecond;
uint8_t flag;
char DispBuff [32];
char RecdBuff[32];

void PIT_Clock_ISR(void);
void PortA_Key_ISR  (uint32_t pinxArray);
void TimeRecord (void);
static void KEY_Scan(void);


int main(void)
{
	/**Systemtick Init*/
    DelayInit();

	/**interrupt on 1ms*/
	PIT_QuickInit(HW_PIT_CH0, 1000);
	/**install ISR function for PIT0*/
	PIT_CallbackInstall(HW_PIT_CH0,PIT_Clock_ISR);
	
	/**Key init*/
	GPIO_QuickInit(HW_GPIOA, 4, kGPIO_Mode_IPU); /**KEY  Restart & Start*/	
	GPIO_QuickInit(HW_GPIOA, 6, kGPIO_Mode_IPU); /**KEY  Restart & Start*/	
	GPIO_QuickInit(HW_GPIOA, 7, kGPIO_Mode_IPU);
	/**key interrupt ISR*/
	GPIO_CallbackInstall(HW_GPIOA, PortA_Key_ISR);
    /**打开PTE26、PTE27 引脚的中断下降沿触发*/
  GPIO_ITDMAConfig(HW_GPIOA, 4,  kGPIO_IT_FallingEdge, true);
	GPIO_ITDMAConfig(HW_GPIOA, 7,  kGPIO_IT_FallingEdge, true);
	/**PE6 is a led show system status*/
	GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
	/**UART console*/
  UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    /**this message is shown at console,please using Putty Tools to check it*/
  printf("flexbus lcd test\r\n");
	/**GUI initation*/
  GUI_Init();
	GUI_SetBkColor(GUI_WHITE);
	GUI_SetColor(GUI_BLACK);
	GUI_Clear();
	GUI_SetFont(&GUI_Font16_1HK);
	/**GUI system show Static String at LCD*/
	GUI_DispStringHCenterAt("second chronograph exprement",120,0);
	GUI_DispStringHCenterAt("Please click KEY1 \n\r on Development Board",120,15); 
	while(1) 
    {
		sprintf(DispBuff,"Now time: %2d:%2d:%2d:%2d",hour,minute,second,millisecond);
		GUI_DispStringAt(DispBuff,0,50);
		KEY_Scan();
		if(gRetValue == KEY_SINGLE) //按键按下，小灯反正
        {
			TimeRecord();
        }
	}
}

void PIT_Clock_ISR(void)
{
	if(!flag)
	{
		millisecond ++;
		if(millisecond > 999)
		{
			second ++;
			millisecond = 0;
		}
		if(second >59)
		{
			minute ++;
			second = 0;
		}
		if(minute > 59)
		{
			hour++;
			minute = 0;
		}
	}
}

void PortA_Key_ISR (uint32_t pinxArray) 
{
	/**Key Start & Restart*/
	if(pinxArray & (1 << 4))
	{
		static uint8_t flag_key = 0;
		if(!flag_key)
		{
			PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, true);
		}
		else
		{
			PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, false);
			millisecond = 0;
			second = 0;
			minute = 0;
			hour = 0;
		}
		flag_key = !flag_key;
	}
	/**Key stop*/
	if(pinxArray & (1 << 7))	
	{
		flag = !flag;
	}
}

void TimeRecord (void)
{
	static uint8_t disp_line;
	LED1 = !LED1;
	switch(disp_line)
	{
		case 0:
			sprintf(RecdBuff,"Record 1:%2d:%2d:%2d:%3d",hour,minute,second,millisecond);
			GUI_DispStringAtCEOL(RecdBuff,0,70);
			disp_line ++;
			break;
		case 1:
			sprintf(RecdBuff,"Record 2:%2d:%2d:%2d:%3d",hour,minute,second,millisecond);
			GUI_DispStringAtCEOL(RecdBuff,0,90);
			disp_line ++;
			break;
		case 2:
			sprintf(RecdBuff,"Record 3:%2d:%2d:%2d:%3d",hour,minute,second,millisecond);
			GUI_DispStringAtCEOL(RecdBuff,0,110);
			disp_line ++;
			break;
		case 3:
			sprintf(RecdBuff,"Record 4:%2d:%2d:%2d:%3d",hour,minute,second,millisecond);
			GUI_DispStringAtCEOL(RecdBuff,0,130);	
			disp_line ++;
			break;
		case 4:
			sprintf(RecdBuff,"Record 5:%2d:%2d:%2d:%3d",hour,minute,second,millisecond);
			GUI_DispStringAtCEOL(RecdBuff,0,150);
			disp_line  = 0;
			break;
		default:
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
            if(KEY2 == 0) /* 如果按键被按下 进入确认与消抖态 */
            {
                status = kKEY_Debounce;
            }
            break;
        case kKEY_Debounce: /* 确认与消抖态 */
            if(KEY2 == 0)
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
            if(KEY2 == 1) /* 按键松开 */
            {
                gRetValue = KEY_SINGLE;
                status = kKEY_Idle;
            }
            break;
        default:
            break;
    }
}
