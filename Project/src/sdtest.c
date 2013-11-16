#include "sdhc.h"
#include "test_common.h"



int CommandFun_SDInitTest(int argc, char *argv[])
{
    uint16_t retryTime = 0;
    SD_InitTypeDef SD_InitStruct1;
    SD_InitStruct1.SD_BaudRate = 2500000;
   // CHRONOSCOPE_StartTimeCount(2);
    while(SD_Init(&SD_InitStruct1) != ESDHC_OK && retryTime < 10)
		{
			retryTime++;	
		}
		UART_printf("SD_Size:%dMB RetryTime:%d\r\n", SD_InitStruct1.SD_Size, retryTime);
    //UART_printf("SD_Size:%dMB RetryTime:%d Time:%dUS\r\n", SD_InitStruct1.SD_Size, retryTime, CHRONOSCOPE_GetTime(2));
    return 0;
}


//! @brief User define function
extern int CommandFun_SDReadSingleBlockTest(int argc, char *argv[])
{
    uint16_t i,j;
    uint8_t buffer[512];
    uint32_t startSector;
	  uint32_t totalBlock;
	  CHS_Str2Num((char*)argv[1], &startSector);
	  UART_printf("START SECTOR:%d\r\n",startSector);
	  CHS_Str2Num((char*)argv[2], &totalBlock);
	  if(totalBlock == 0) totalBlock = 1;
	  UART_printf("CNT:%d\r\n",totalBlock);
	  for(j=0;j< totalBlock;j++)
	  {
        SD_ReadSingleBlock(startSector+j, buffer);
        UART_printf("SECTOR:%d\r\n", j+startSector);
		    for(i=0;i<sizeof(buffer);i++)
		    {
            UART_printf("%x ", buffer[i]);
	    	}
	    	UART_printf("\r\n"); 
		}
		//CHRONOSCOPE_StartTimeCount(2);
	//	UART_printf("SD_ReadSingleBlock:%dUS\r\n", CHRONOSCOPE_GetTime(2));

    return 0;
}