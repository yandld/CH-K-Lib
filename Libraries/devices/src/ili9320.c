#include "ili9320.h"
#include "gpio.h"
#include "flexbus.h"
#include "systick.h"

 void LCD_WriteRegister(uint16_t RegisterIndex, uint16_t Data)
{
    WMLCDCOM(RegisterIndex);
    WMLCDDATA(Data);
}

 uint16_t LCD_ReadRegister(uint16_t RegisterIndex)
{
    WMLCDCOM(RegisterIndex);
    return (*(uint32_t*)(&LCD_DATA_ADDRESS));
}

uint32_t ILI9320_GetDeivceID(void)
{
    return LCD_ReadRegister(0x00);
}

void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
#ifdef LCD_USE_HORIZONTAL
	Xpos = LCD_X_MAX - 1 -Xpos;
    LCD_WriteRegister(0x21, Xpos);
    LCD_WriteRegister(0x20, Ypos);
	
#else
    LCD_WriteRegister(0x20, Xpos);
    LCD_WriteRegister(0x21, Ypos);
#endif
}


void LCD_Clear(uint16_t color)
{
	uint32_t index=0;      
	uint32_t totalpoint = LCD_X_MAX*LCD_Y_MAX;
	LCD_SetCursor(0,0);	//设置光标位置 
	WMLCDCOM(0x22);     //开始写入GRAM	 	  
	for(index = 0; index < totalpoint; index++)
	{
		WMLCDDATA(color);	   
	}
}  


void ili9320_Init(void)
{
    uint32_t gpio_instance;
    #if 0
    //Flexbus Init
    SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK);
    //control signals
    PORTB->PCR[19] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;          // FB_OE
    PORTD->PCR[1]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;          // CS0
    PORTA->PCR[26] = PORT_PCR_MUX(6)|PORT_PCR_DSE_MASK;          // A27
    PORTC->PCR[16] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;          // FB_BE_23_16
    
    /*
    PORTB->PCR[18] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD15
    PORTC->PCR[0]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD14
    PORTC->PCR[1]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD13
    PORTC->PCR[2]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD12
    PORTC->PCR[4]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD11
    PORTC->PCR[5]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD10
    PORTC->PCR[6]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD9
    PORTC->PCR[7]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD8
    PORTC->PCR[8]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD7
    PORTC->PCR[9]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD6
    PORTC->PCR[10] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD5
    PORTD->PCR[2]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD4
    PORTD->PCR[3]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD3
    PORTD->PCR[4]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD2
    PORTD->PCR[5]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD1
    PORTD->PCR[6]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD0
    */
    
    PORTB->PCR[17]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD16
    PORTB->PCR[16]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD17
    PORTB->PCR[11]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD18
    PORTB->PCR[10]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD19
    PORTB->PCR[9]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD20
    PORTB->PCR[8]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD21
    PORTB->PCR[7]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD22
    PORTB->PCR[6]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD23
    PORTC->PCR[15]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD24
    PORTC->PCR[14]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD25
    PORTC->PCR[13]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD26
    PORTC->PCR[12]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD27
    PORTB->PCR[23]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD28
    PORTB->PCR[22]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD29
    PORTB->PCR[21]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD30
    PORTB->PCR[20]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  FB_AD31 
    
    FLEXBUS_InitTypeDef FLEXBUS_InitStruct;
    FLEXBUS_InitStruct.ADSpaceMask = 0x800;
    FLEXBUS_InitStruct.autoAckMode = kFLEXBUS_AutoAckEnable;
    FLEXBUS_InitStruct.CSn = kFLEXBUS_CS0;
    FLEXBUS_InitStruct.dataAlignMode = kFLEXBUS_DataLeftAligned;
    FLEXBUS_InitStruct.dataWidth = kFLEXBUS_PortSize_16Bit;
    FLEXBUS_InitStruct.baseAddress = FLEXBUS_BASE_ADDRESS;
    FLEXBUS_InitStruct.ByteEnableMode = kFLEXBUS_BE_AssertedWrite;
    FLEXBUS_InitStruct.CSPortMultiplexingCotrol = FB_CSPMCR_GROUP3(kFLEXBUS_CSPMCR_GROUP3_BE_23_16);
    FLEXBUS_Init(&FLEXBUS_InitStruct);
    #endif
    // Back light
    gpio_instance = GPIO_QuickInit(HW_GPIOC, 3, kGPIO_Mode_OPP);
    GPIO_WriteBit(gpio_instance, 3, 1); 
    // reset 
    gpio_instance = GPIO_QuickInit(HW_GPIOC, 19, kGPIO_Mode_OPP);
    GPIO_WriteBit(gpio_instance, 19, 0); 
    DelayMs(10);
    GPIO_WriteBit(gpio_instance, 19, 1);
    DelayMs(10); 
   //initializing funciton 1    
    //LCD_WR_REG(0xe5,0x8000);  // Set the internal vcore voltage    
    LCD_WriteRegister(0x00,0x0001);  // start OSC    
    LCD_WriteRegister(0x2b,0x0010);  //Set the frame rate as 80 when the internal resistor is used for oscillator circuit    
    LCD_WriteRegister(0x01,0x0100);  //s720  to  s1 ; G1 to G320    
    LCD_WriteRegister(0x02,0x0700);  //set the line inversion    
    //LCD_WR_REG(0x03,0x1018);  //65536 colors     
    LCD_WriteRegister(0x03,0x1030);    
    //横屏
    #ifdef LCD_USE_HORIZONTAL
    LCD_WriteRegister(0x03,(0<<5)|(0<<4)|(1<<3)|(1<<12));
    #else
    LCD_WriteRegister(0x03,(1<<5)|(1<<4)|(0<<3)|(1<<12));
    #endif

    LCD_WriteRegister(0x04,0x0000);   
    LCD_WriteRegister(0x08,0x0202);  //specify the line number of front and back porch periods respectively    
    LCD_WriteRegister(0x09,0x0000);   
    LCD_WriteRegister(0x0a,0x0000);   
    LCD_WriteRegister(0x0c,0x0000);  //select  internal system clock    
    LCD_WriteRegister(0x0d,0x0000);   
    LCD_WriteRegister(0x0f,0x0000);    
    LCD_WriteRegister(0x50,0x0000);  //0x50 -->0x53 set windows adress    
    LCD_WriteRegister(0x51,0x00ef);   
    LCD_WriteRegister(0x52,0x0000);   
    LCD_WriteRegister(0x53,0x013f);   
    LCD_WriteRegister(0x60,0x2700);   
    LCD_WriteRegister(0x61,0x0001);   
    LCD_WriteRegister(0x6a,0x0000);   
    LCD_WriteRegister(0x80,0x0000);   
    LCD_WriteRegister(0x81,0x0000);   
    LCD_WriteRegister(0x82,0x0000);   
    LCD_WriteRegister(0x83,0x0000);   
    LCD_WriteRegister(0x84,0x0000);   
    LCD_WriteRegister(0x85,0x0000);   
    LCD_WriteRegister(0x90,0x0010);   
    LCD_WriteRegister(0x92,0x0000);   
    LCD_WriteRegister(0x93,0x0003);   
    LCD_WriteRegister(0x95,0x0110);   
    LCD_WriteRegister(0x97,0x0000);   
    LCD_WriteRegister(0x98,0x0000);    
 
    //power setting function    
    LCD_WriteRegister(0x10,0x0000);   
    LCD_WriteRegister(0x11,0x0000);   
    LCD_WriteRegister(0x12,0x0000);   
    LCD_WriteRegister(0x13,0x0000);   
    DelayMs(20);   
    LCD_WriteRegister(0x10,0x17b0);   
    LCD_WriteRegister(0x11,0x0004);   
    DelayMs(5);   
    LCD_WriteRegister(0x12,0x013e);   
    DelayMs(5);   
    LCD_WriteRegister(0x13,0x1f00);   
    LCD_WriteRegister(0x29,0x000f);   
    DelayMs(5);   
    LCD_WriteRegister(0x20,0x0000);   
    LCD_WriteRegister(0x21,0x0000);   
 
    //initializing function 2    
    LCD_WriteRegister(0x30,0x0204);   
    LCD_WriteRegister(0x31,0x0001);   
    LCD_WriteRegister(0x32,0x0000);   
    LCD_WriteRegister(0x35,0x0206);   
    LCD_WriteRegister(0x36,0x0600);   
    LCD_WriteRegister(0x37,0x0500);   
    LCD_WriteRegister(0x38,0x0505);   
    LCD_WriteRegister(0x39,0x0407);   
    LCD_WriteRegister(0x3c,0x0500);   
    LCD_WriteRegister(0x3d,0x0503);   

    //开启显示   
    LCD_WriteRegister(0x07,0x0173);
   
    LCD_Clear(BLUE);
}





