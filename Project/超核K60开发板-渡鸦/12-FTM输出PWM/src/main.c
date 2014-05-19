#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "ftm.h"

/* CH Kinetis¹Ì¼ş¿â V2.50 °æ±¾ */
/* ĞŞ¸ÄÖ÷Æµ ÇëĞŞ¸Ä CMSIS±ê×¼ÎÄ¼ş system_MKxxxx.c ÖĞµÄ CLOCK_SETUP ºê */


/* ¿ÉÓÃµÄFTMÍ¨µÀÓĞ: */
/*
 FTM0_CH4_PB12   // ftm0 Ä£¿é 4Í¨µÀ PB12Òı½Å
 FTM0_CH5_PB13   
 FTM0_CH5_PA00   
 FTM0_CH6_PA01   
 FTM0_CH7_PA02   
 FTM0_CH0_PA03
 FTM0_CH1_PA04   
 FTM0_CH2_PA05   
 FTM0_CH3_PA06   
 FTM0_CH4_PA07   
 FTM0_CH0_PC01   
 FTM0_CH1_PC02   
 FTM0_CH2_PC03   
 FTM0_CH3_PC04   
 FTM0_CH4_PD04   
 FTM0_CH5_PD05   
 FTM0_CH6_PD06   
 FTM0_CH7_PD07   
 FTM1_CH0_PB12   
 FTM1_CH1_PB13   
 FTM1_CH0_PA08   
 FTM1_CH1_PA09   
 FTM1_CH0_PA12   
 FTM1_CH1_PA13   
 FTM1_CH0_PB00   
 FTM1_CH1_PB01   
 FTM2_CH0_PA10   
 FTM2_CH1_PA11   
 FTM2_CH0_PB18   
 FTM2_CH1_PB19   
*/

/*
     ÊµÑéÃû³Æ£ºFTMÊä³öPWM
     ÊµÑéÆ½Ì¨£º¶ÉÑ»¿ª·¢°å
     °åÔØĞ¾Æ¬£ºMK60DN512ZVQ10
 ÊµÑéĞ§¹û£º¿ØÖÆPTA¶Ë¿ÚµÄ6Òı½ÅÊÇPWMÄ£Ê½
      Êä³öÕ¼¿Õ±ÈÎª50%£¬ÆµÂÊÎª3KHzµÄ·½²¨  
*/
int main(void)
{
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP); /* LED */
    
    /* ³õÊ¼»¯Ò»¸öÄ£¿éµÄÒ»°ãÄ£Ê½: ³õÊ¼»¯Ä£¿é±¾Éí->¸ù¾İĞ¾Æ¬ÊÖ²á ³õÊ¼»¯¶ÔÓ¦µÄ¸´ÓÃÒı½Å->Ê¹ÓÃÄ£¿é */
    UART_InitTypeDef UART_InitStruct1 = {0};
    UART_InitStruct1.instance = HW_UART0;
    UART_InitStruct1.baudrate = 115200;
    UART_Init(&UART_InitStruct1);
    
    /* ³õÊ¼»¯´®¿Ú0¶ÔÓ¦µÄÒı½Å D6 D7*/
    PORT_PinMuxConfig(HW_GPIOD, 6, kPinAlt3);
    PORT_PinMuxConfig(HW_GPIOD, 7, kPinAlt3);
    
    printf("ftm test, pwm will be generated on PA06\r\n");
    
    /* Ê¹ÓÃ¿ìËÙ³õÊ¼»¯ °ïÖú³õÑ§ÕßÍê³É±ØÒªÅäÖÃ */
    FTM_PWM_QuickInit(FTM0_CH3_PA06, kPWM_EdgeAligned, 3000);
    
    
    /* ÉèÖÃFTM0Ä£¿é3Í¨µÀµÄÕ¼¿Õ±È */
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH3, 5000); //º0-10000 ¶ÔÓ¦ 0-100%
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6); //¿ØÖÆĞ¡µÆÉÁË¸
        DelayMs(500);
    }
}


