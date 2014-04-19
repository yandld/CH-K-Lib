/**
  ******************************************************************************
  * @file    24l01.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#include "24l01.h"
#include "board.h"
#include "gpio.h"
#include "spi.h"
#include "spi_abstraction.h"

#define NRF24L01_DEBUG		0
#if ( NRF24L01_DEBUG == 1 )
#define NRF24L01_DEBUG	printf
#else
#define NRF24L01_DEBUG(...)
#endif

static struct spi_device device;


#define CE_HIGH()     GPIO_WriteBit(BOARD_NRF2401_CE_PORT, BOARD_NRF2401_CE_PIN, 1)
#define CE_LOW()      GPIO_WriteBit(BOARD_NRF2401_CE_PORT, BOARD_NRF2401_CE_PIN, 0)

static uint8_t NRF2401_WriteBuffer(uint8_t reg, uint8_t *pData, uint8_t len)
{
    uint8_t status;
    status = SPI_ReadWriteByte(HW_SPI0, HW_CTAR0, reg, BOARD_SPI_NRF2401_CSn, kSPI_PCS_KeepAsserted);
    len--;
    while(len--)
    {
        SPI_ReadWriteByte(HW_SPI0, HW_CTAR0, *pData++, BOARD_SPI_NRF2401_CSn, kSPI_PCS_KeepAsserted);
    }
    SPI_ReadWriteByte(HW_SPI0, HW_CTAR0, *pData++, BOARD_SPI_NRF2401_CSn, kSPI_PCS_ReturnInactive);
    return(status);          // return nRF24L01 status byte
}

static uint8_t NRF2401_ReadWriteReg(uint8_t reg,uint8_t value)
{
    uint8_t status;
    status = SPI_ReadWriteByte(HW_SPI0, HW_CTAR0, reg, BOARD_SPI_NRF2401_CSn, kSPI_PCS_KeepAsserted);
    SPI_ReadWriteByte(HW_SPI0, HW_CTAR0, value, BOARD_SPI_NRF2401_CSn, kSPI_PCS_ReturnInactive);
    return status;
}

const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //接收地址
unsigned char Bank0_Reg[][2]=
{
    {0,0x0F},//congig
    {1,0x0F},//en_aa
    {2,0x03},//en_rxaddr
    {3,0x03},//setup_aw
    {4,0x15},//setup_retr
    {5,40},//rf_ch
    {6,0X07},//rf_setup	//REG6,120517,0x0F or 0x2F:2Mbps; 0x07:1Mbps ; 0x27:250Kbps
    {7,0x07},//status
    {8,0x00},//observe_tx
    {9,0x00},//cd
    {12,0x11},//rx_addr_p2
    {13,0x12},//rx_addr_p3
    {14,0x13},//rx_addr_p4
    {15,0x14},//rx_addr_p5
    {17,0x20},//RX_PW_P0
    {18,0x20},//RX_PW_P1
    {19,0x20},//RX_PW_P2
    {20,0x20},//RX_PW_P3
    {21,0x20},//RX_PW_P4
    {22,0x20},//RX_PW_P5
    {23,0x00},//FIFO_STATUS,
    {28,0x3F},//DYNPD
    {29,0x04}, //FEATURE
};

static uint8_t NRF2401_ConfigStatus(void)
{
    uint8_t i;
	for(i = 22; i > 0; i--)
    {
        NRF2401_ReadWriteReg(WRITE_REG+Bank0_Reg[i][0], Bank0_Reg[i][1]);    //刷新接收数据
    }
	
	NRF2401_WriteBuffer(WRITE_REG+RX_ADDR_P0, (uint8_t*)RX_ADDRESS, RX_ADR_WIDTH);//写TX节点地址
	NRF2401_WriteBuffer(WRITE_REG+TX_ADDR, (uint8_t*)RX_ADDRESS, RX_ADR_WIDTH);//写RX节点地址
	i = NRF2401_ReadWriteReg(29,0xFF); //读取寄存器的状态 
	if(i == 0) // i!=0 showed that chip has been actived.so do not active again.
    {
        NRF2401_ReadWriteReg(0X50,0x73);// Active
    }
	for(i = 22; i >= 21; i--)
    {
        NRF2401_ReadWriteReg((WRITE_REG|Bank0_Reg[i][0]), Bank0_Reg[i][1]);
    }
	return 0;
}


static uint8_t NRF2401_ReadBuffer(uint8_t reg, uint8_t *pData, uint32_t len)
{
    uint8_t status;
    /* Select register to write to and read status byte */
    status = SPI_ReadWriteByte(HW_SPI0, HW_CTAR0, reg, BOARD_SPI_NRF2401_CSn, kSPI_PCS_KeepAsserted);
    len--;
    while(len--)
    { 
        *pData++ = SPI_ReadWriteByte(HW_SPI0, HW_CTAR0, 0x00, BOARD_SPI_NRF2401_CSn, kSPI_PCS_KeepAsserted);
    }
    *pData++ = SPI_ReadWriteByte(HW_SPI0, HW_CTAR0, 0x00, BOARD_SPI_NRF2401_CSn, kSPI_PCS_ReturnInactive);
	return(status);                    // return nRF24L01 status byte
}

static int write_register(uint8_t addr, uint8_t *buf, uint32_t len)
{
    uint8_t temp = WRITE_REG + addr;
    spi_write(&device, &temp, 1, false);
    spi_write(&device, buf, len, true);
}

static int read_register(uint8_t addr, uint8_t *buf, uint32_t len)
{
    uint8_t temp = READ_REG + addr;
    spi_write(&device, &temp, 1, false);
    spi_read(&device, buf, len, true);
}

static uint8_t NRF2401_Check(void)
{
	uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	uint8_t i = sizeof(buf);
    write_register(TX_ADDR, buf, sizeof(buf));
	for(i = 0; i < 5; i++)
	{
        buf[i] = 0;
	}
    read_register(TX_ADDR, buf, sizeof(buf));
	for(i = 0; i < 5; i++)
	{
        if(buf[i] != 0xA5) return 1;
	}
	return 0;
}	

int nrf24l01_init(spi_bus_t bus, uint32_t cs)
{
    uint32_t ret;
    device.csn = cs;
    device.config.baudrate = 1*1000*1000;
    device.config.data_width = 8;
    device.config.mode = SPI_MODE_0 | SPI_MASTER | SPI_MSB;
    ret = spi_bus_attach_device(bus, &device);
    if(ret)
    {
        return ret;
    }
    else
    {
        ret = spi_config(&device);
    }
    return ret;
}

uint8_t NRF2401_Init(void)
{
    
    SPI_QuickInit(SPI0_SCK_PC05_SOUT_PC06_SIN_PC07, kSPI_CPOL0_CPHA0, 1000 * 1000);
	GPIO_QuickInit(BOARD_NRF2401_CE_PORT, BOARD_NRF2401_CE_PIN , kGPIO_Mode_OPP);
    GPIO_QuickInit(BOARD_NRF2401_IRQ_PORT, BOARD_NRF2401_IRQ_PIN , kGPIO_Mode_IPU);
    PORT_PinMuxConfig(HW_GPIOC, BOARD_SPI_CS_PIN, kPinAlt2);
    
    CE_LOW();
    if(NRF2401_Check())
    {
        return 1;
    }
  //  NRF2401_ReadWriteReg(WRITE_REG + 0x01, 0x3F);
  //  NRF2401_ReadWriteReg(WRITE_REG + 0x1C, 0x3F);
  //  NRF2401_ReadWriteReg(WRITE_REG + 0x1C, 0x3F);
 //   NRF2401_ReadWriteReg(WRITE_REG + 0x1D, 0x07);
    NRF2401_ConfigStatus();
  //  NRF2401_ReadWriteReg(WRITE_REG + 0x01, 0x3F);
  //  NRF2401_ReadWriteReg(WRITE_REG + 0x1C, 0x3F);
  //  NRF2401_ReadWriteReg(WRITE_REG + 0x1C, 0x3F);
  //  NRF2401_ReadWriteReg(WRITE_REG + 0x1D, 0x07);
    return 0;
}



//该函数初始化NRF24L01到TX模式
//设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
//PWR_UP,CRC使能
//当CE变高后,即进入RX模式,并可以接收数据了		   
//CE为高大于10us,则启动发送.	 
void NRF2401_SetTXMode(void)
{
    uint8_t value;
	NRF2401_ReadWriteReg(FLUSH_TX,  0x00);    //刷新接收数据 
    CE_LOW();// chip enable
    value=NRF2401_ReadWriteReg(NCONFIG, 0xFF); //读取寄存器的状态 
    value |= 0xFE;//set bit 1
    NRF2401_ReadWriteReg(WRITE_REG+NCONFIG, value);// Set PWR_UP bit, enable CRC(2 length) & Prim:RX. RX_DR enabled..
    CE_HIGH(); //CE为高,进入接收模式 
}

//该函数初始化NRF24L01到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了		   
void NRF2401_SetRXMode(void)
{
	uint8_t value;
	NRF2401_ReadWriteReg(FLUSH_RX, 0x00);    //刷新接收数据
	value=NRF2401_ReadWriteReg(NRF2401_STATUS, 0xFF); //读取寄存器的状态 
	NRF2401_ReadWriteReg(WRITE_REG+NRF2401_STATUS, value);  //清除状态标志
	CE_LOW();
	value = NRF2401_ReadWriteReg(NCONFIG,0xFF); //读取寄存器的状态 
	value |= 0x01;//set bit 1
	NRF2401_ReadWriteReg(WRITE_REG+NCONFIG, value);// Set PWR_UP bit, enable CRC(2 length) & Prim:RX. RX_DR enabled..
	CE_HIGH(); //CE为高,进入接收模式 
}

//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:发送完成状况
uint8_t NRF2401_SendPacket(uint8_t *txbuf, uint8_t len)
{
    NRF2401_WriteBuffer(WR_TX_PLOAD,txbuf, len);//写数据到TX BUF  32个字节
    return 0;
}

//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:0，接收完成；其他，错误代码
uint32_t NRF2401_RecPacket(uint8_t *rxbuf)
{
	uint32_t sta,len;
	sta = NRF2401_ReadWriteReg(NRF2401_STATUS, 0xFF);  //读取状态寄存器的值  
	if(sta & RX_OK)//接收到数据
	{
        NRF2401_ReadWriteReg(WRITE_REG + NRF2401_STATUS, sta); //清除TX_DS或MAX_RT中断标志
        SPI_ReadWriteByte(HW_SPI0, HW_CTAR0, R_RX_PL_WID, BOARD_SPI_NRF2401_CSn, kSPI_PCS_KeepAsserted);
        len = SPI_ReadWriteByte(HW_SPI0, HW_CTAR0, R_RX_PL_WID, BOARD_SPI_NRF2401_CSn, kSPI_PCS_ReturnInactive);
        NRF2401_ReadBuffer(RD_RX_PLOAD, rxbuf, len);// read receive payload from RX_FIFO buffer
        if(len > 32)
        {
            NRF2401_ReadWriteReg(FLUSH_RX,0xff);//清除TX FIFO寄存器 
        }
        return len;			
	}	
	return 0;//没收到任何数据
}	



 












