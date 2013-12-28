/**
  ******************************************************************************
  * @file    i2c.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "i2c.h"
#include "gpio.h"
#include "clock.h"

//!< Leagacy Support for Kineis Z Version(Inital Version)
#if (!defined(I2C_BASES))

    #if (defined(MK60DZ10))
        #define I2C_BASES {I2C0, I2C1}
    #endif
    #if (defined(MK10D5))
        #define I2C_BASES {I2C0}
    #endif 
#endif

//!< Gloabl Const Table Defination
#define I2C_IRQn_OFFSET 1
static IRQn_Type   const I2C_IRQBase = I2C0_IRQn;
static I2C_Type * const I2C_InstanceTable[] = I2C_BASES;
static I2C_CallBackType I2C_CallBackTable[ARRAY_SIZE(I2C_InstanceTable)] = {NULL};
#if (defined(MK60DZ10) || defined(MK40D10) || defined(MK60D10)|| defined(MK10D10))
static const uint32_t SIM_I2CClockGateTable[] =
{
    SIM_SCGC4_I2C0_MASK,
    SIM_SCGC4_I2C1_MASK,
};
#elif (defined(MK10D5))
static const uint32_t SIM_I2CClockGateTable[] =
{
    SIM_SCGC4_I2C0_MASK,
};
#endif

//!< clock deiver struct (internal)
typedef struct 
{
    uint8_t icr;            //!< F register ICR value.
    uint16_t sclDivider;    //!< SCL clock divider.
}_I2C_Divider_Type;



//!< @brief I2C divider values.
const _I2C_Divider_Type I2C_DiverTable[] =
{
    // ICR  Divider
    { 0x00, 20 },
    { 0x01, 22 },
    { 0x02, 24 },
    { 0x03, 26 },
    { 0x04, 28 },
    { 0x05, 30 },
    { 0x09, 32 },
    { 0x06, 34 },
    { 0x0a, 36 },
    { 0x07, 40 },
    { 0x0c, 44 },
    { 0x0d, 48 },
    { 0x0e, 56 },
    { 0x12, 64 },
    { 0x0f, 68 },
    { 0x13, 72 },
    { 0x14, 80 },
    { 0x15, 88 },
    { 0x19, 96 },
    { 0x16, 104 },
    { 0x1a, 112 },
    { 0x17, 128 },
    { 0x1c, 144 },
    { 0x1d, 160 },
    { 0x1e, 192 },
    { 0x22, 224 },
    { 0x1f, 240 },
    { 0x23, 256 },
    { 0x24, 288 },
    { 0x25, 320 },
    { 0x26, 384 },
    { 0x2a, 448 },
    { 0x27, 480 },
    { 0x2b, 512 },
    { 0x2c, 576 },
    { 0x2d, 640 },
    { 0x2e, 768 },
    { 0x32, 896 },
    { 0x2f, 960 },
    { 0x33, 1024 },
    { 0x34, 1152 },
    { 0x35, 1280 },
    { 0x36, 1536 },
    { 0x3a, 1792 },
    { 0x37, 1920 },
    { 0x3b, 2048 },
    { 0x3c, 2304 },
    { 0x3d, 2560 },
    { 0x3e, 3072 },
    { 0x3f, 3840 }
};

//!< set i2c baudrate
void I2C_SetBaudrate(uint8_t instance, uint32_t sourceClockInHz, uint32_t baudrate)
{
    // Check if the requested frequency is greater than the max supported baud.
    if (baudrate > (sourceClockInHz / (1U * 20U)))
    {
        return;
    }
    uint32_t mult;
    uint32_t hz = baudrate;
    uint32_t bestError = 0xffffffffu;
    uint32_t bestMult = 0u;
    uint32_t bestIcr = 0u;
    // Search for the settings with the lowest error.
    // mult is the MULT field of the I2C_F register, and ranges from 0-2. It selects the
    // multiplier factor for the divider.
    for (mult = 0u; (mult <= 2u) && (bestError != 0); ++mult)
    {
        uint32_t multiplier = 1u << mult;
        // Scan table to find best match.
        uint32_t i;
        for (i = 0u; i < ARRAY_SIZE(I2C_DiverTable); ++i)
        {
            uint32_t computedRate = sourceClockInHz / (multiplier * I2C_DiverTable[i].sclDivider);
            uint32_t absError = hz > computedRate ? hz - computedRate : computedRate - hz;
            if (absError < bestError)
            {
                bestMult = mult;
                bestIcr = I2C_DiverTable[i].icr;
                bestError = absError;
                // If the error is 0, then we can stop searching because we won't find a
                // better match.
                if (absError == 0)
                {
                    break;
                }
            }
        }
    }
    I2C_InstanceTable[instance]->F = (I2C_F_ICR(bestIcr)|I2C_F_MULT(bestMult));
}


//! @defgroup CHKinetis-K
//! @{

//! @defgroup I2C-K
//! @brief I2C-K driver modules
//! @{

//! @defgroup I2C-K_API_Functions
//! @{

 /**
 * @brief  Quick init for user, do not need init struct
 * @code
 *      // init I2C1 pin: SCL:PC10 SDA:PC11 baudrate:47000Hz
 *      I2C_QuickInit(I2C1_SCL_PC10_SDA_PC11, 47000);
 * @endcode
 * @param  I2CxMAP: QuickInit Select
 *         @arg I2C1_SCL_PE01_SDA_PE00
 *         @arg I2C0_SCL_PB00_SDA_PB01
 *         @arg I2C0_SCL_PB02_SDA_PB03
 *         @arg I2C1_SCL_PC10_SDA_PC11
 * @param  baudrate: better to be 48000Hz 76000Hz 96000Hz 376000Hz
 * @retval moudle instance
 */
uint8_t I2C_QuickInit(uint32_t I2CxMAP, uint32_t baudrate)
{
    uint8_t i;
    I2C_InitTypeDef I2C_InitStruct1;
    QuickInit_Type * pI2CxMap = (QuickInit_Type*)&(I2CxMAP);
    I2C_InitStruct1.baudrate = baudrate;
    I2C_InitStruct1.instance = pI2CxMap->ip_instance;
    I2C_Init(&I2C_InitStruct1);
    // init pinmux and  open drain and pull up
    for(i = 0; i < pI2CxMap->io_offset; i++)
    {
        PORT_PinMuxConfig(pI2CxMap->io_instance, pI2CxMap->io_base + i, (PORT_PinMux_Type)pI2CxMap->mux);
        PORT_PinConfig(pI2CxMap->io_instance, pI2CxMap->io_base + i, kPullUp, ENABLE); 
    }
    return pI2CxMap->ip_instance;
}

/**
 * @brief  init I2C moudle
 * @param  I2C_InitStruct: init struct of I2C
 * @retval None
 */
void I2C_Init(I2C_InitTypeDef* I2C_InitStruct)
{
	//param check
    assert_param(IS_I2C_ALL_INSTANCE(I2C_InitStruct->instance));
    uint32_t freq;
    SIM->SCGC4 |= SIM_I2CClockGateTable[I2C_InitStruct->instance];
    // disable first
    I2C_InstanceTable[I2C_InitStruct->instance]->C1 &= ~I2C_C1_IICEN_MASK;
    // set baudrate
    CLOCK_GetClockFrequency(kBusClock, &freq);
    I2C_SetBaudrate(I2C_InitStruct->instance, freq, I2C_InitStruct->baudrate);
    // enable i2c
    I2C_InstanceTable[I2C_InitStruct->instance]->C1 |= I2C_C1_IICEN_MASK;
}

/**
 * @brief  generate start signal
 * @param  instance:
 *         @arg HW_I2C0
 *         @arg HW_I2C1
 * @retval None
 */
void I2C_GenerateSTART(uint8_t instance)
{
	//param check
    assert_param(IS_I2C_ALL_INSTANCE(instance));
    I2C_InstanceTable[instance]->C1 |= I2C_C1_TX_MASK;
    I2C_InstanceTable[instance]->C1 |= I2C_C1_MST_MASK;
}

/**
 * @brief  generate restart signal
 * @param  instance:
 *         @arg HW_I2C0
 *         @arg HW_I2C1
 * @retval None
 */
void I2C_GenerateRESTART(uint8_t instance)
{
	//param check
    assert_param(IS_I2C_ALL_INSTANCE(instance));
    I2C_InstanceTable[instance]->C1 |= I2C_C1_RSTA_MASK;
}

/**
 * @brief  generate stop signal
 * @param  instance:
 *         @arg HW_I2C0
 *         @arg HW_I2C1
 * @retval None
 */
void I2C_GenerateSTOP(uint8_t instance)
{
	//param check
    assert_param(IS_I2C_ALL_INSTANCE(instance));
	I2C_InstanceTable[instance]->C1 &= ~I2C_C1_MST_MASK;
    I2C_InstanceTable[instance]->C1 &= ~I2C_C1_TX_MASK;
}

/**
 * @brief  i2c send a byte
 * @param  instance:
 *         @arg HW_I2C0
 *         @arg HW_I2C1
 * @retval None
 */
void I2C_SendData(uint8_t instance, uint8_t data)
{
	//param check
    assert_param(IS_I2C_ALL_INSTANCE(instance));
	I2C_InstanceTable[instance]->D = data;
}

/**
 * @brief  i2c read a byte
 * @param  instance:
 *         @arg HW_I2C0
 *         @arg HW_I2C1
 * @retval None
 */
uint8_t I2C_ReadData(uint8_t instance)
{
	//param check
    assert_param(IS_I2C_ALL_INSTANCE(instance));
	return (I2C_InstanceTable[instance]->D);
}
/**
 * @brief  i2c send 7 bit address
 * @param  instance:
 *         @arg HW_I2C0
 *         @arg HW_I2C1
 * @param  address: address of slave device should be 0-127
 * @param  direction:
 *         @arg kI2C_Read  : Master read
 *         @arg kI2C_Write : Master write
 * @retval None
 */
void I2C_Send7bitAddress(uint8_t instance, uint8_t address, I2C_Direction_Type direction)
{
	//param check
    assert_param(IS_I2C_ALL_INSTANCE(instance));
    address <<= 1;
	(kI2C_Write == direction)?((address &= 0xFE)):(address |= 0x01);
	I2C_InstanceTable[instance]->D = address;
}

/**
 * @brief  i2c wait ack. this function should be follow by senddata when using polling mode
 * @param  instance:
 *         @arg HW_I2C0
 *         @arg HW_I2C1
 * @retval 0:received ack 1:no ack  2:timeout
 */
uint8_t I2C_WaitAck(uint8_t instance)
{
    uint32_t timeout = 0;
	//param check
    assert_param(IS_I2C_ALL_INSTANCE(instance));
    //wait for transfer complete
    timeout = 0;
    while (((I2C_InstanceTable[instance]->S & I2C_S_TCF_MASK) == 0) && (timeout < 1000))
    {
        timeout++;
    }
    //both TCF and IICIF indicate one byte trasnfer complete
    timeout = 0;
    while (((I2C_InstanceTable[instance]->S & I2C_S_IICIF_MASK) == 0) && (timeout < 1000))
    {
        timeout++;
    }
    //IICIF is a W1C Reg, so clear it!
    I2C_InstanceTable[instance]->S |= I2C_S_IICIF_MASK;
    if(timeout > 999)
    {
        return 2;
    }
    //see if we receive the ACK signal
    if (I2C_InstanceTable[instance]->S & I2C_S_RXAK_MASK)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief  I2C set read or write data when in Master mode
 * @param  instance:
 *         @arg HW_I2C0
 *         @arg HW_I2C1
 * @param  direction:
 *         @arg kI2C_Read  : Master read
 *         @arg kI2C_Write : Master write
 * @retval None
 */
void I2C_SetMasterMode(uint8_t instance, I2C_Direction_Type direction)
{
	//param check
    assert_param(IS_I2C_ALL_INSTANCE(instance));
	(direction == kI2C_Write)?(I2C_InstanceTable[instance]->C1 |= I2C_C1_TX_MASK):(I2C_InstanceTable[instance]->C1 &= ~I2C_C1_TX_MASK);
}
/**
 * @brief  Generate NACK signal
 * @param  instance:
 *         @arg HW_I2C0
 *         @arg HW_I2C1
 * @retval None
 */
void I2C_GenerateNAck(uint8_t instance)
{
	//param check
    assert_param(IS_I2C_ALL_INSTANCE(instance));
	I2C_InstanceTable[instance]->C1 |= I2C_C1_TXAK_MASK;
}
/**
 * @brief  Generate ACK signal
 * @param  instance:
 *         @arg HW_I2C0
 *         @arg HW_I2C1
 * @retval None
 */
void I2C_GenerateAck(uint8_t instance)
{
	//param check
    assert_param(IS_I2C_ALL_INSTANCE(instance));
	I2C_InstanceTable[instance]->C1 &= ~I2C_C1_TXAK_MASK;
}
/**
 * @brief  config I2C dma or Intrrupt settings
 * @param  instance:
 *         @arg HW_I2C0
 *         @arg HW_I2C1
 * @param  config:
 *         @arg kI2C_ITDMA_Disable: disable DMA and interrupt
 *         @arg kI2C_IT_BTC  : trigger interrupt when byte transfer complete
 *         @arg kI2C_DMA_BTC : trigger dma when byte transfer complete
 * @retval None
 */
void I2C_ITDMAConfig(uint8_t instance, I2C_ITDMAConfig_Type config, FunctionalState newState)
{
	//param check
    assert_param(IS_I2C_ALL_INSTANCE(instance));
    switch(config)
    {
        case kI2C_ITDMA_Disable:
            NVIC_DisableIRQ((IRQn_Type)(I2C_IRQBase + instance*I2C_IRQn_OFFSET));
            I2C_InstanceTable[instance]->C1 &= ~I2C_C1_IICIE_MASK;
            I2C_InstanceTable[instance]->C1 &= ~I2C_C1_DMAEN_MASK;
            break;
        case kI2C_IT_BTC:
            (ENABLE == newState)?(I2C_InstanceTable[instance]->C1 |= I2C_C1_IICIE_MASK):(I2C_InstanceTable[instance]->C1 &= ~I2C_C1_IICIE_MASK);
            (ENABLE == newState)?(NVIC_EnableIRQ((IRQn_Type)(I2C_IRQBase + instance*I2C_IRQn_OFFSET))):(NVIC_DisableIRQ((IRQn_Type)(I2C_IRQBase + instance*I2C_IRQn_OFFSET)));
            break;
        case kI2C_DMA_BTC:
            (ENABLE == newState)?(I2C_InstanceTable[instance]->C1 |= I2C_C1_DMAEN_MASK):(I2C_InstanceTable[instance]->C1 &= ~I2C_C1_DMAEN_MASK);
            break;
        default:
            break;
    }
}

/**
 * @brief  install ISR callback
 * @param  instance:
 *         @arg HW_I2C0
 *         @arg HW_I2C1
 * @param  AppCBFun: user callback function
 * @retval None
 */
void I2C_CallbackInstall(uint8_t instance, I2C_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        I2C_CallBackTable[instance] = AppCBFun;
    }
}

/**
 * @brief  return 0 if line is busy
 * @param  instance:
 *         @arg HW_I2C0
 *         @arg HW_I2C1
 * @retval 0:busy 1:idle
 */

uint8_t I2C_IsBusy(uint8_t instance)
{
	if(I2C_InstanceTable[instance]->S & I2C_S_BUSY_MASK)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/**
 * @brief  I2C genereal write bytes to slave
 * @param  instance:
 *         @arg HW_I2C0
 *         @arg HW_I2C1
 * @param  DeviceAddress: device address 0-127
 * @param  pBuffer: send data pointer
 * @param  len: len of data to send
 * @retval 0:succ  1:address send fail 2:data send fail
 */
uint8_t I2C_WriteByte(uint8_t instance ,uint8_t DeviceAddress, uint8_t *pBuffer, uint32_t len)
{
    uint32_t time_out = 0;
    //Generate START signal
    I2C_GenerateSTART(instance);
    //Send 7bit Data with WRITE operation
    I2C_Send7bitAddress(instance, DeviceAddress, kI2C_Write);
    if(I2C_WaitAck(instance))
    {
        I2C_GenerateSTOP(instance);
        time_out = 0;
        while(!I2C_IsBusy(instance) && (time_out < 1000))
        {
            time_out++;
        }
        return 1;
    }
    //Send All Data
    while(len--)
    {
        I2C_SendData(instance, *(pBuffer++));
        if(I2C_WaitAck(instance))
        {
            I2C_GenerateSTOP(instance);
            time_out = 0;
            while(!I2C_IsBusy(instance) && (time_out < 1000))
            {
                time_out++;
            }
            return 2;
        }
    }
    //Generate stop and wait for line idle
    I2C_GenerateSTOP(instance);
    time_out = 0;
    while(!I2C_IsBusy(instance));
    return 0;
}

/**
 * @brief  I2C genereal write a single register to slave
 *         general to use for write a single register to slave
 * @param  instance:
 *         @arg HW_I2C0
 *         @arg HW_I2C1
 * @param  DeviceAddress: device address 0-127
 * @param  RegisterAddress: register address offest in slave
 * @param  Data: data
 * @retval 0:succ  1:address send fail 2:data send fail
 */
uint8_t I2C_WriteSingleRegister(uint8_t instance, uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t Data)
{
    uint8_t ret;
    uint8_t buf[2];
    buf[0] = RegisterAddress;
    buf[1] = Data;
    ret = I2C_WriteByte(instance, DeviceAddress, buf, sizeof(buf));
    return ret;
}

/**
 * @brief  I2C genereal read a single register to slave
 *         general to use for read a single register to slave
 * @param  instance:
 *         @arg HW_I2C0
 *         @arg HW_I2C1
 * @param  DeviceAddress: device address 0-127
 * @param  RegisterAddress: register address offest in slave
 * @param  pData: pointer of read data
 * @retval 0:succ  1:address send fail 2:data send fail
 */
uint8_t I2C_ReadSingleRegister(uint8_t instance, uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t* pData)
{
    uint32_t time_out = 0;
    uint8_t data;
    //Generate START signal
    I2C_GenerateSTART(instance);
    //Send 7bit Data with WRITE operation
    I2C_Send7bitAddress(instance, DeviceAddress, kI2C_Write);
    if(I2C_WaitAck(instance))
    {
        I2C_GenerateSTOP(instance);
        time_out = 0;
        while(!I2C_IsBusy(instance) && (time_out < 1000))
        {
            time_out++;
        }
        return 1;
    }
    //Send Reg Address
    I2C_SendData(instance, RegisterAddress);
    if(I2C_WaitAck(instance))
    {
        I2C_GenerateSTOP(instance);
        time_out = 0;
        while(!I2C_IsBusy(instance));
        return 2;
    }
    //Generate RESTART Signal
    I2C_GenerateRESTART(instance);
    //Resend 7bit Address, This time we use READ Command
    I2C_Send7bitAddress(instance, DeviceAddress, kI2C_Read);
    if(I2C_WaitAck(instance))
    {
        I2C_GenerateSTOP(instance);
        time_out = 0;
        while(!I2C_IsBusy(instance));
        return 3;
    }
    //Set Master in slave mode
    I2C_SetMasterMode(instance,kI2C_Read);
    //Dummy Read in order to generate SCL clock
    data = I2C_InstanceTable[instance]->D;
    I2C_GenerateNAck(instance);
    //This time, We just wait for masters receive byte transfer complete
    I2C_WaitAck(instance);
    //Generate stop and wait for line idle
    I2C_GenerateSTOP(instance);
    time_out = 0;
    while(!I2C_IsBusy(instance));
    //actual read
    data = I2C_InstanceTable[instance]->D;
    *pData = data;
    return 0;
}

//! @}

//! @defgroup I2C-K_Internal_Functions
//! @{

void I2C0_IRQHandler(void)
{
    // clear pending bit
    I2C_InstanceTable[HW_I2C0]->S |= I2C_S_IICIF_MASK;
    if(I2C_CallBackTable[HW_I2C0])
    {
        I2C_CallBackTable[HW_I2C0]();
    }
    
}

#if (!defined(MK10D5))
void I2C1_IRQHandler(void)
{
    // clear pending bit
    I2C_InstanceTable[HW_I2C1]->S |= I2C_S_IICIF_MASK;
    if(I2C_CallBackTable[HW_I2C1])
    {
        I2C_CallBackTable[HW_I2C1]();
    }
}
#endif

//! @}

//! @}

//! @}

/*
static const QuickInit_Type I2C_QuickInitTable[] =
{
    { 1, 4, 6, 0, 2, 0}, //I2C1_SCL_PE01_SDA_PE00 6
    { 0, 4, 4,18, 2, 0}, //I2C0_SCL_PE19_SDA_PE18 4
    { 0, 5, 2,22, 2, 0}, //I2C0_SCL_PF22_SDA_PF23 2
    { 0, 1, 2, 0, 2, 0}, //I2C0_SCL_PB00_SDA_PB01 2
    { 0, 1, 2, 2, 2, 0}, //I2C0_SCL_PB02_SDA_PB03 2
    { 1, 2, 2,10, 2, 0}, //I2C1_SCL_PC10_SDA_PC11 2
    { 0, 3, 2, 8, 2, 0}, //I2C0_SCL_PD08_SDA_PD09 2
};
*/
