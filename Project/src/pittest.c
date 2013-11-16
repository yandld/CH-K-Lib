#include "pit.h"



int CommandFun_PITInitTest(int argc, char *argv[])
{
    uint8_t i = 0;
    uint32_t param[5];
    PIT_InitTypeDef PIT_InitStruct1;
    UART_printf("PIT Init Test PITInit <PIT_CHL> <PeriodTimeInUs> <IT>\r\n");

    for(i = 0; i < argc-1; i++)
    {
        CHS_Str2Num((char*)argv[i+1], &param[i]);
        UART_printf("param:%d\r\n", param[i]);
    }
    PIT_InitStruct1.PITxMap = param[0];
    PIT_InitStruct1.PIT_PeriodInUs = param[1];
		PIT_Init(&PIT_InitStruct1);
		if(param[2] == 1)
		{
			PIT_ITConfig(PIT_InitStruct1.PITxMap, ENABLE);
		}
		else if(param[2] == 0)
		{
			PIT_ITConfig(PIT_InitStruct1.PITxMap, DISABLE);
		}
		
}