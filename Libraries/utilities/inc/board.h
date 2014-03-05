#ifndef __BOARDS_H__
#define __BOARDS_H__


#ifdef CHK60EVB2

#define BOARD_LED_GPIO_BASES {HW_GPIOE, HW_GPIOE, HW_GPIOE, HW_GPIOE}
#define BOARD_LED_PIN_BASES  {6, 7, 11, 12}

#define BOARD_UART_DEBUG_MAP  UART0_RX_PD06_TX_PD07
#define BOARD_I2C_MAP         I2C0_SCL_PB02_SDA_PB03


#elif CHK60EVB1

#define BOARD_UART_DEBUG_MAP  UART4_RX_PC14_TX_PC15

#elif URANUS

#define BOARD_UART_DEBUG_MAP  UART1_RX_PC03_TX_PC04

#else

#error  "Please Define a Board"

#endif



#endif

