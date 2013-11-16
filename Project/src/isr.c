#include "isr.h"


void PORTC_IRQHandler(void)
{
	GPIO_ClearITPendingBit(PTC,kGPIO_Pin_18);
	UART_printf("!!\r\n");
}

uint16_t i = 0;
void UART4_RX_TX_IRQHandler(void)
{
    uint8_t ch;
    if(UART_ReceiveByte_Async(UART4, &ch) == TRUE)
		{
			UART_printf("%d\r\n", ch);
		}
//	GPIO_ToggleBit(PTD, kGPIO_Pin_7);
//	if(i<10)
//	{
		//UART4->D = i+'0';
//	}
   //  UART4->D = i+'0';
	//i++;
}



void PIT0_IRQHandler(void)
{
    UART_printf("PIT%d ISR\r\n", 0);
    PIT_ClearITPendingBit(PIT0);
}

void PIT1_IRQHandler(void)
{
    UART_printf("PIT%d ISR\r\n", 1);
    PIT_ClearITPendingBit(PIT1);
}

void PIT2_IRQHandler(void)
{
    UART_printf("PIT%d ISR\r\n", 2);
    PIT_ClearITPendingBit(PIT2);
}

void PIT3_IRQHandler(void)
{
    UART_printf("PIT%d ISR\r\n", 3);
    PIT_ClearITPendingBit(PIT3);
}

void Watchdog_IRQHandler(void)
{
    WDOG_ClearAllITPendingBit();
    UART_printf("Watchdog_IRQHandler\r\n");
}



