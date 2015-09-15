#include "gpio.h"
#include "common.h"
#include "uart.h"



#define EEP_SIZE_1K                 (0x34)
#define DFLASH_SIZE_FOR_EEP_16K     (0x02)

#define EEP_START_ADDR              (0x14000000)

int FlexNVM_EEPInit(int eeprom_size, int dflash_size)
{
      /* Test to make sure the device is not already partitioned. If it
       * is already partitioned, then return with no action performed.
       */
      if ((SIM->FCFG1 & SIM_FCFG1_DEPART(0xF)) != 0x00000F00)
      {
          printf("\nDevice is already partitioned.\r\n");
          return 1;
      }

      /* Write the FCCOB registers */
      FTFL->FCCOB0 = FTFL_FCCOB0_CCOBn(0x80); // Selects the PGMPART command
      FTFL->FCCOB1 = 0x00;
      FTFL->FCCOB2 = 0x00;
      FTFL->FCCOB3 = 0x00;

      /* FCCOB4 is written with the code for the subsystem sizes (eeprom_size define) */
      FTFL->FCCOB4 = eeprom_size;

      /* FFCOB5 is written with the code for the Dflash size (dflash_size define) */
      FTFL->FCCOB5 = dflash_size;

      /* All required FCCOBx registers are written, so launch the command */
      FTFL->FSTAT = FTFL_FSTAT_CCIF_MASK;

      /* Wait for the command to complete */
      while(!(FTFL->FSTAT & FTFL_FSTAT_CCIF_MASK));

      return 0;
}



int main(void)
{
    uint32_t clock;
    DelayInit();
    DelayMs(10);
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PA01_TX_PA02, 115200);
    
    clock = GetClock(kCoreClock);
    printf("core clock:%dHz\r\n", clock);
    clock = GetClock(kBusClock);
    printf("bus clock:%dHz\r\n", clock);
    printf("FlexMem demo!\r\n");
    
    printf("EESIZE code: 0x%X\r\n", (SIM->FCFG1 & SIM_FCFG1_EESIZE_MASK) >> SIM_FCFG1_EESIZE_SHIFT);
    
    if(FlexNVM_EEPInit(EEP_SIZE_1K, DFLASH_SIZE_FOR_EEP_16K) == 0)
    {
        printf("Dflash initialized successfully\r\n");
    }

   // EEP_SelfTest(EEP_START_ADDR, EEP_START_ADDR + 1024);
    
    volatile uint8_t *p = (uint8_t *)EEP_START_ADDR+5;
    (*p)++;
    while(!(FTFL->FCNFG & FTFL_FCNFG_EEERDY_MASK));
    printf("data:%d\r\n", *p);
    
    while(1)
    {
        /* …¡À∏–°µ∆ */
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


