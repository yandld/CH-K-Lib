#include "gpio.h"
#include "common.h"
#include "CHZT02.h"
#include "CH9250.h"


int8_t CHZT02_Init(void) {
	/**Switch mosfet startup*/
	GPIO_QuickInit(CHZT02_SensorEN_Group, CHZT02_SensorEN_Pin, CHZT02_SensorEN_OutputMode);
	GPIO_WriteBit(CHZT02_SensorEN_Group, CHZT02_SensorEN_Pin, CHZT02_SensorEnable);
	/**MPU9250 Init*/
	CH9250_Init();
	return 0;
}

