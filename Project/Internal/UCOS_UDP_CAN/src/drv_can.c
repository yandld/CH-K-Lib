#include "includes.h"
#include "can.h"
#include "drv_can.h"

#define CAN_TX_MB       (0x02)
#define CAN_RX_MB       (0x03)

#define CAN_RX_ID       0x56

void CAN_ISR(void)
{
    OSIntEnter();   

    uint8_t buf[8];
    uint8_t len;
    uint32_t id;
    if(CAN_ReadData(HW_CAN1, CAN_RX_MB, &id, buf, &len) == 0)
    {
        printf("CAN id:0x%X ", id>>CAN_ID_STD_SHIFT);
        while(len--)
        {
            printf("0x%x ", buf[len]);
        }
        printf("\r\n");
    }
    OSIntExit();
}

static OS_EVENT* CAN_Mutex;

uint32_t OSCAN_Init(void)
{
    uint8_t err;
    CAN_Mutex = OSMutexCreate(OS_PRIO_MUTEX_CEIL_DIS, &err);
    
    CAN_QuickInit(CAN1_TX_PE24_RX_PE25, 20*1000);
    CAN_CallbackInstall(HW_CAN1, CAN_ISR);
    CAN_ITDMAConfig(HW_CAN1, CAN_RX_MB, kCAN_IT_RX);
    CAN_SetRxMB(HW_CAN1, CAN_RX_MB, CAN_RX_ID);
    
    return 0;
}

uint32_t OSCAN_Send(uint32_t instance, uint32_t id, uint8_t *buf, uint32_t len)
{
    uint8_t err;
    OSMutexPend(CAN_Mutex, 0, &err);
    CAN_WriteData(instance, CAN_TX_MB, id, buf, len);
    OSMutexPost(CAN_Mutex);
    return 0;
}
















