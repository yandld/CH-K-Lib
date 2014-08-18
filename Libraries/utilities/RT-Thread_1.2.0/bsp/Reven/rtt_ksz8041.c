#include <rtthread.h>
#include "rtt_ksz8041.h"
#include <rtdevice.h>
#include <netif/ethernetif.h>
#include "gpio.h"
#include "enet.h"
#include "ksz8041.h"

static struct eth_device device;
static uint8_t gCfgLoca_MAC[] = {0x00, 0xCF, 0x52, 0x35, 0x00, 0x01};
static uint8_t     gRxBuffer[1500];
__align(4) static uint8_t     gTxBuffer[1500];
uint32_t    rx_len;
static  rt_mutex_t mutex;

void ENET_ISR(void)
{
    uint32_t len;
    rx_len = 0;
    rt_interrupt_enter();
    len = ENET_MacReceiveData(gRxBuffer);
    if(len)
    {
        /* a frame has been received */
        rx_len = len;
        eth_device_ready(&device);
        //rt_kprintf("enet frame received len:%d\r\n", rx_len);
    }
    rt_interrupt_leave();
}


static rt_err_t rt_ksz8041_init(rt_device_t dev)
{
    int r;
    rt_kprintf("rt_ksz8041_init is called\r\n");
    PORT_PinMuxConfig(HW_GPIOB, 0, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOB, 1, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 5, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 12, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 13, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 14, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 15, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 16, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 17, kPinAlt4);
    
    r = ksz8041_init(0x00);
    if(r)
    {
        rt_kprintf("ksz8041 init failed! code:%d\r\n", r);
        return RT_ERROR;
    }
    if(!ksz8041_is_linked())
    {
        rt_kprintf("no wire connected\r\n");
        eth_device_linkchange(&device, false);
    }
    
    ENET_InitTypeDef ENET_InitStruct1;
    ENET_InitStruct1.pMacAddress = gCfgLoca_MAC;
    ENET_InitStruct1.is10MSpped = ksz8041_is_phy_10m_speed();
    ENET_InitStruct1.isHalfDuplex = !ksz8041_is_phy_full_dpx();

    ENET_Init(&ENET_InitStruct1);
    ENET_CallbackRxInstall(ENET_ISR);
    ENET_ITDMAConfig(kENET_IT_RXF);
    
    mutex = rt_mutex_create("enet_mutex", RT_IPC_FLAG_FIFO);
    
    return RT_EOK;
}


static rt_err_t rt_ksz8041_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t rt_ksz8041_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_size_t rt_ksz8041_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    return RT_EOK;
}

static rt_size_t rt_ksz8041_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    return RT_EOK;
}

static rt_err_t rt_ksz8041_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    switch (cmd)
    {
    case NIOCTL_GADDR:
        /* get mac address */
        if (args) rt_memcpy(args, gCfgLoca_MAC, 6);
        else return -RT_ERROR;
        break;

    default :
        break;
    }

    return RT_EOK;
}

struct pbuf *rt_ksz8041_rx(rt_device_t dev)
{
    struct pbuf* p;
    rt_uint32_t i;
    rt_uint32_t send_cnt = 0;
    /* init p pointer */
    p = RT_NULL;
    i = 0;
    if(rx_len)
    {
        p = pbuf_alloc(PBUF_LINK, rx_len, PBUF_RAM);
        if (p != RT_NULL)
        {
            struct pbuf* q;  
            for (q = p; q != RT_NULL; q = q->next)
            {
                rt_memcpy((rt_uint8_t*)q->payload, (rt_uint8_t*)&gRxBuffer[i], q->len);
                send_cnt += q->len;
            }
            rx_len = 0;
        }
        else
        {
            rt_kprintf("dm9000 rx: no pbuf\r\n");
            return NULL;
        }
    }
    return p;
}

rt_err_t rt_ksz8041_tx( rt_device_t dev, struct pbuf* p)
{

    rt_uint32_t i;
    struct pbuf *q;
    i = 0;
    
    rt_mutex_take(mutex, RT_WAITING_FOREVER);
    for (q = p; q != RT_NULL; q = q->next)
    {
        rt_memcpy((rt_uint8_t*)&gTxBuffer[i], (rt_uint8_t*)q->payload, q->len);
        i += q->len;
    }
    

    
    ENET_MacSendData(gTxBuffer, i);
    while(ENET_IsTransmitComplete() == 0);
    rt_mutex_release(mutex);
    
    /* check if still linked */
    if(!ksz8041_is_linked())
    {
        eth_device_linkchange(&device, false);
        return RT_ERROR;
    }
    else
    {
        eth_device_linkchange(&device, true);
    }
    
    return RT_EOK;
}

int rt_hw_ksz8041_init(void)
{

    device.parent.init       = rt_ksz8041_init;
    device.parent.open       = rt_ksz8041_open;
    device.parent.close      = rt_ksz8041_close;
    
    device.parent.read       = rt_ksz8041_read;
    device.parent.write      = rt_ksz8041_write;
    device.parent.control    = rt_ksz8041_control;
    device.parent.user_data    = RT_NULL;

    device.eth_rx     = rt_ksz8041_rx;
    device.eth_tx     = rt_ksz8041_tx;

    eth_device_init(&device, "e0");
    return 0;
}

