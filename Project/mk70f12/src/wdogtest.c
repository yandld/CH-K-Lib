#include "wdog.h"



int CommandFun_WDOGInitTest(int argc, char *argv[])
{
    WDOG_InitTypeDef WDOGInitStruct1;
    WDOGInitStruct1.WDOG_Mode = kWDOG_Mode_WINDOW;
    WDOGInitStruct1.WDOG_TimeOutInUs = 500*1000;
    WDOGInitStruct1.WDOG_WindowTimeInUs = 1000*100;
    WDOG_Init(&WDOGInitStruct1);
}


int CommandFun_WDOGInitTest2(void)
{
    WDOG_InitTypeDef WDOGInitStruct1;
    WDOGInitStruct1.WDOG_Mode = kWDOG_Mode_WINDOW;
    WDOGInitStruct1.WDOG_TimeOutInUs = 500*1000;
	  WDOGInitStruct1.WDOG_WindowTimeInUs = 5*1000;
    WDOG_Init(&WDOGInitStruct1);
}