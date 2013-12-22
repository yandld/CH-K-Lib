#ifndef __HMC5883_H__
#define __HMC5883_H__

#include <stdint.h>

/*发送地址，写地址在其基础上加1即 0x3d*/
#define HMC5883_SLAVE_ADDRESS	0x1E

typedef struct hmc5883_device
{
    const char* name;
    uint32_t I2CxMAP;
    uint8_t device_addr;
    uint8_t i2c_instance;
    uint8_t (*read_reg)(struct hmc5883_device * device, uint8_t reg_addr, uint8_t *data);
    uint8_t (*write_reg)(struct hmc5883_device * device, uint8_t reg_addr, uint8_t data);
    uint8_t (*read_data)(struct hmc5883_device * device, int16_t* x, int16_t* y, int16_t* z);
    uint8_t (*ctrl)(struct hmc5883_device * device, uint8_t cmd);
    uint8_t (*probe)(struct hmc5883_device * device);
}hmc5883_device;

#define HMC_CFG1		0x00		//Register ConfigA
#define HMC_CFG2		0x01		//Register ConfigB	
#define HMC_MOD			0x02		//Mode Register
#define HMC_DX_MSB		0x03		//data output x	MSB 
#define HMC_DX_LSB		0x04		//data output x LSB
#define HMC_DY_MSB		0x05		//data output y MSB
#define HMC_DY_LSB		0x06		//data output y LSB
#define HMC_DZ_MSB		0x07		//data output z MSB
#define HMC_DZ_LSB		0x08		//data output z LSB
#define HMC_STATUE		0x09		//Statue Register
#define HMC_IDTF_A		0x10		//Identification Register A
#define HMC_IDTF_B		0x11		//Identification Register B
#define HMC_IDTF_C		0x12		//Identification Register C


//!< API functions
uint8_t hmc5883_init(hmc5883_device* device, uint32_t I2CxMap, const char * name, uint32_t baudrate);

#endif

