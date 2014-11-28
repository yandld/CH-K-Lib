/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2013  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************


** emWin V5.22 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  ARM LIMITED whose registered office
is situated at  110 Fulbourn Road,  Cambridge CB1 9NJ,  England solely
for  the  purposes  of  creating  libraries  for  ARM7, ARM9, Cortex-M
series,  and   Cortex-R4   processor-based  devices,  sublicensed  and
distributed as part of the  MDK-ARM  Professional  under the terms and
conditions  of  the   End  User  License  supplied  with  the  MDK-ARM
Professional. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : LCDConf.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "GUIDRV_FlexColor.h"
#include "ili9320.h"

/*********************************************************************
*
*       Layer configuration (to be modified)
*
**********************************************************************
*/

//
// Physical display size
//
#define XSIZE_PHYS 240
#define YSIZE_PHYS 320

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

static void LcdWriteData(U16 Data)
{
  // ... TBD by user
	GUI_LCD_DATA_ADDRESS = Data;
}



static void LcdWriteDataMultiple(U16 * pData, int NumItems) {
  while (NumItems--) {
		GUI_LCD_DATA_ADDRESS = *pData++;
  }
}

static void LcdReadDataMultiple(U16 * pData, int NumItems) {
  *pData = GUI_LCD_DATA_ADDRESS;
	while (NumItems--) {
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


void LCD_X_Config(void) {
    GUI_DEVICE * pDevice;
    CONFIG_FLEXCOLOR Config = {0};
    GUI_PORT_API PortAPI = {0};
    //
    // Set display driver and color conversion
    //
    pDevice = GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
    //
    // Display driver configuration, required for Lin-driver
    //

    LCD_SetSizeEx    (0, XSIZE_PHYS,   YSIZE_PHYS);
    LCD_SetVSizeEx   (0, VXSIZE_PHYS,  VYSIZE_PHYS);
    /* Horzital display */
    Config.Orientation   =  0;
    GUIDRV_FlexColor_Config(pDevice, &Config);
    unsigned Orientation = 0;
    
    GUI_TOUCH_SetOrientation(Orientation);
    GUI_TOUCH_Calibrate(GUI_COORD_X, 0, 239, TOUCH_AD_LEFT, TOUCH_AD_RIGHT);
    GUI_TOUCH_Calibrate(GUI_COORD_Y, 0, 319, TOUCH_AD_TOP, TOUCH_AD_BOTTOM);	
    //
    // Set controller and operation mode
    //
    #ifndef WIN32
    PortAPI.pfWrite16_A0  = LcdWriteReg; //write REG
    PortAPI.pfWrite16_A1  = LcdWriteData; //write DATA
    PortAPI.pfWriteM16_A1 = LcdWriteDataMultiple; //write MUTI
    PortAPI.pfReadM16_A1  = LcdReadDataMultiple;  //READ muti
    GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66708, GUIDRV_FLEXCOLOR_M16C0B16);
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
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r;
  (void) LayerIndex;
  (void) pData;

  switch (Cmd) {
  //
  // Required
  //
  case LCD_X_INITCONTROLLER: {
    //
    // Called during the initialization process in order to set up the
    // display controller and put it into operation. If the display
    // controller is not initialized by any external routine this needs
    // to be adapted by the customer...
    //
    // ...
    #ifndef WIN32
    ili9320_init();
    #endif
    return 0;
  }
  default:
    r = -1;
  }
  return r;
}

/*************************** End of file ****************************/
