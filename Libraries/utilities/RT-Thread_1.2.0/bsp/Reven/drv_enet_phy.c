#include <rtthread.h>
#include "chlib_k.h"
#include "rtt_drv.h"
#include <rtdevice.h>
#include <netif/ethernetif.h>
#include "enet_phy.h"

static struct eth_device device;
static uint8_t gCfgLoca_MAC[] = {0x00, 0xCF, 0x52, 0x35, 0x00, 0x01};
static rt_uint8_t *gTxBuf;



void ENET_ISR(void)
{
    rt_interrupt_enter();
    eth_device_ready(&device);
    rt_interrupt_leave();
}


static rt_err_t rt_enet_phy_init(rt_device_t dev)
{
    /* init driver */
    ENET_InitTypeDef ENET_InitStruct1;
    ENET_InitStruct1.pMacAddress = gCfgLoca_MAC;
    ENET_InitStruct1.is10MSpped = false;
    ENET_InitStruct1.isHalfDuplex = false;
    ENET_Init(&ENET_InitStruct1);
    
    PORT_PinMuxConfig(HW_GPIOB, 0, kPinAlt4);
    PORT_PinPullConfig(HW_GPIOB, 0, kPullUp);
    PORT_PinOpenDrainConfig(HW_GPIOB, 0, ENABLE);
    
    PORT_PinMuxConfig(HW_GPIOB, 1, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 5, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 12, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 13, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 14, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 15, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 16, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOA, 17, kPinAlt4);
    
    enet_phy_init();

//    if(!enet_phy_is_linked())
//    {
//        eth_device_linkchange(&device, false);
//        return RT_EIO;
//    }
    
    ENET_CallbackRxInstall(ENET_ISR);
    ENET_ITDMAConfig(kENET_IT_RXF);
    
    return RT_EOK;
}


static rt_err_t rt_enet_phy_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t rt_enet_phy_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_size_t rt_enet_phy_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    return RT_EOK;
}

static rt_size_t rt_enet_phy_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    return RT_EOK;
}

static rt_err_t rt_enet_phy_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    switch (cmd)
    {
    case NIOCTL_GADDR:
        /* get mac address */
        if (args) rt_memcpy(args, gCfgLoca_MAC, 6);
        else return -RT_ERROR;
        break;
    case NIOCTL_GET_PHY_DATA:
//        if (args) rt_memcpy(args, &phy_data, sizeof(phy_data));
        break;
    default :
        break;
    }
    return RT_EOK;
}

struct pbuf *rt_enet_phy_rx(rt_device_t dev)
{
    struct pbuf* p = RT_NULL;
    rt_uint32_t len;
    
    len = ENET_GetReceiveLen();
//    len = ENET_MacReceiveData(gRxBuf);
    if(len)
    {
        p = pbuf_alloc(PBUF_LINK, len, PBUF_RAM);
        ENET_MacReceiveData(p->payload);
//        if (p != RT_NULL)
//        {
//            struct pbuf* q;  
//            for (q = p; q != RT_NULL; q = q->next)
//            {
//                rt_memcpy((rt_uint8_t*)q->payload, (rt_uint8_t*)&gRxBuf[i], q->len);
//            }
//            len = 0;
//        }
//        else
//        {
//            return NULL;
//        }
    }
    return p;
}

rt_err_t rt_enet_phy_tx( rt_device_t dev, struct pbuf* p)
{
    bool islink;
    rt_uint32_t len;
    struct pbuf *q;
    
    islink = enet_phy_is_linked();
    eth_device_linkchange(&device, islink);
    
    for (q = p; q != RT_NULL; q = q->next)
    {
       // rt_uint8_t *pData = rt_malloc(q->len);
        if(islink)
        {
            rt_memcpy(gTxBuf, q->payload, q->len);
            ENET_MacSendData(gTxBuf, q->len);
        }
       // rt_free(pData);
    }
    
    if(islink)
    {
        return RT_EOK;
    }
    return RT_EIO;
}


int rt_hw_enet_phy_init(void)
{
    device.parent.init       = rt_enet_phy_init;
    device.parent.open       = rt_enet_phy_open;
    device.parent.close      = rt_enet_phy_close;
    
    device.parent.read       = rt_enet_phy_read;
    device.parent.write      = rt_enet_phy_write;
    device.parent.control    = rt_enet_phy_control;
    device.parent.user_data    = RT_NULL;

    device.eth_rx     = rt_enet_phy_rx;
    device.eth_tx     = rt_enet_phy_tx;
    
    gTxBuf = rt_malloc(CFG_ENET_BUFFER_SIZE+16);
    gTxBuf = (rt_uint8_t*)(uint32_t)RT_ALIGN((uint32_t)gTxBuf, 16);
    if(!gTxBuf)
    {
        return RT_ENOMEM;
    }
    
    eth_device_init(&device, "e0");
    return 0;
}

