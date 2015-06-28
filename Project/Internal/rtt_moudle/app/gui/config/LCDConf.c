
#include "GUI.h"
#include "GUIDRV_FlexColor.h"
#include <stdint.h>
#include <api.h>

#define XSIZE_PHYS 240
#define YSIZE_PHYS 320



static uint32_t gLCDCode;
//
// Color conversion
//
#define COLOR_CONVERSION GUICC_565

//
// Display driver
//
#define DISPLAY_DRIVER GUIDRV_FLEXCOLOR


/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VXSIZE_PHYS
  #define VXSIZE_PHYS XSIZE_PHYS
#endif
#ifndef   VYSIZE_PHYS
  #define VYSIZE_PHYS YSIZE_PHYS
#endif

#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   COLOR_CONVERSION
  #error Color conversion not defined!
#endif
#ifndef   DISPLAY_DRIVER
  #error No display driver defined!
#endif


/********************************************************************
*
*       _SetReg
*/

#define GUI_LCD_ADDRESS_BASE        0x70000000
#define GUI_LCD_REG_ADDRESS         *(unsigned short *)0x70000000
#define GUI_LCD_DATA_ADDRESS        *(unsigned short *)0x78000000

static void LcdWriteReg(U16 Data)
{
	GUI_LCD_REG_ADDRESS = Data;
}



static inline void LcdWriteData(U16 Data)
{
	GUI_LCD_DATA_ADDRESS = Data;
}



static inline void LcdWriteDataMultiple(U16 * pData, int NumItems)
{
    while (NumItems--)
    {
        GUI_LCD_DATA_ADDRESS = *pData++;
    }
}

static void LcdReadDataMultiple(U16 * pData, int NumItems)
{
    if(gLCDCode == 0x9320)
    {
        *pData = GUI_LCD_DATA_ADDRESS; /* dummy read */
    }
  
    while (NumItems--)
    {
        *pData++=GUI_LCD_DATA_ADDRESS;
    }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*
*/


#define TOUCH_AD_TOP            1950
#define TOUCH_AD_BOTTOM         100
#define TOUCH_AD_LEFT           160
#define TOUCH_AD_RIGHT          1900


void LCD_X_Config(void)
{
    GUI_DEVICE * pDevice;
    CONFIG_FLEXCOLOR Config = {0};
    GUI_PORT_API PortAPI = {0};
    /* select different controller */
    rt_device_t lcd;
    lcd = rt_device_find("lcd0");
    if(lcd)
    {
        rt_device_init(lcd);
        rt_device_control(lcd, RTGRAPHIC_CTRL_GET_INFO, &gLCDCode);
        rt_kprintf("lcd id:0x%X\r\n", gLCDCode);
    }
    
    switch(gLCDCode)
    {
       case 0x9320:
           pDevice = GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, GUICC_565, 0, 0);
           break;
       case 0x8989:
           pDevice = GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, GUICC_M565, 0, 0);
           break;
       default:
           break;
   }
    

    LCD_SetSizeEx    (0, XSIZE_PHYS,   YSIZE_PHYS);
    LCD_SetVSizeEx   (0, VXSIZE_PHYS,  VYSIZE_PHYS);
    
    /* Horzital display */
    Config.Orientation   =  0;
    GUIDRV_FlexColor_Config(pDevice, &Config);
    unsigned Orientation = 0;
    
    GUI_TOUCH_SetOrientation(Orientation);
    GUI_TOUCH_Calibrate(GUI_COORD_X, 0, 239, TOUCH_AD_LEFT, TOUCH_AD_RIGHT);
    GUI_TOUCH_Calibrate(GUI_COORD_Y, 0, 319, TOUCH_AD_TOP, TOUCH_AD_BOTTOM);	

    #ifndef WIN32
    PortAPI.pfWrite16_A0  = LcdWriteReg;
    PortAPI.pfWrite16_A1  = LcdWriteData;
    PortAPI.pfWriteM16_A1 = LcdWriteDataMultiple;

    PortAPI.pfReadM16_A1  = LcdReadDataMultiple;
    
    switch(gLCDCode)
    {
       case 0x9320:
            GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66708, GUIDRV_FLEXCOLOR_M16C0B16);
           break;
       case 0x8989:
            GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66702, GUIDRV_FLEXCOLOR_M16C0B16);
           break;
       default:
           break;
   }

    #endif
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData)
{
    int r;
    r = -1;
    switch (Cmd)
    {
        case LCD_X_INITCONTROLLER:
            break;
    }
    return r;
}

/*************************** End of file ****************************/
