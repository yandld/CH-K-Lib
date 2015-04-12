#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"

#include "swd.h"
#include "rl_usb.h"
#include "FlashOS.h"
#include "iflash.h"

#include <stdio.h>

#define SCS_BASE            (0xE000E000UL)                            /*!< System Control Space Base Address  */
#define ITM_BASE            (0xE0000000UL)                            /*!< ITM Base Address                   */
#define DWT_BASE            (0xE0001000UL)                            /*!< DWT Base Address                   */
#define TPI_BASE            (0xE0040000UL)                            /*!< TPI Base Address                   */
#define CoreDebug_BASE      (0xE000EDF0UL)                            /*!< Core Debug Base Address            */
#define SysTick_BASE        (SCS_BASE +  0x0010UL)                    /*!< SysTick Base Address               */
#define NVIC_BASE           (SCS_BASE +  0x0100UL)                    /*!< NVIC Base Address                  */
#define SCB_BASE            (SCS_BASE +  0x0D00UL)                    /*!< System Control Block Base Address  */
#define DBG_Addr     (0xe000edf0)
// Core Debug Register Addresses
#define DBG_HCSR       (DBG_Addr + DBG_HCSR_OFS)
#define DBG_CRSR       (DBG_Addr + DBG_CRSR_OFS)
#define DBG_CRDR       (DBG_Addr + DBG_CRDR_OFS)
#define DBG_EMCR       (DBG_Addr + DBG_EMCR_OFS)

#define SIM_SDID_BASE       (0x40048024)

int main(void)
{   
    DelayInit();
    GPIO_QuickInit(HW_GPIOC, 10, kGPIO_Mode_OPP);
    
    #ifdef MK22F25612
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
    #elif  MK20D5
    UART_QuickInit(UART1_RX_PC03_TX_PC04, 115200);
    #endif
    
    printf("program for Manley\r\n");
    printf("this program manly display 3 features:\r\n");

    uint32_t val,err, DP_ID;
    
    swd_io_init();

    TRST_LOW();
    DelayMs(20);
    TRST_HIGH();
//    
    SWJ_InitDebug();
  //  swd_init_debug();
    
    SWJ_ReadDP(DP_IDCODE, &DP_ID);
    printf("DP-IDR:0x%X\r\n", DP_ID);

    SWJ_ReadAP(MDM_IDR, &DP_ID);
    printf("AHB_AP_IDR:0x%X\r\n", DP_ID);
    
    SWJ_ReadAP(0x000000FC, &DP_ID);
    printf("APB_AP_IDR:0x%X\r\n", DP_ID);
    
    val = 0;
    
    SWJ_ReadMem32(SIM_SDID_BASE, &val);
    printf("mem:0x%X\r\n", val);

    SWJ_ReadMem32(SCB_BASE, &val);
    printf("mem:0x%X\r\n", val);
     
    err += SWJ_WriteMem8(0x20000013, 0x55);
    printf("MDM_IDR:0x%X %d\r\n", DP_ID, err);
    
    DP_ID = 0;
    err += SWJ_ReadMem8(0x20000013, &DP_ID);
    printf("MDM_IDR:0x%X %d\r\n", DP_ID, err);
    
  //  SWJ_WriteMem32((uint32_t)&PTD->PDDR, 0x00000000);
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOC, 10);
        DelayMs(500);
    }
    
}










