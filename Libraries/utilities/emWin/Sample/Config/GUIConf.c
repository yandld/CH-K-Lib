/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2015  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.30 - Graphical user interface for embedded applications **
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
Licensing information

Licensor:                 SEGGER Software GmbH
Licensed to:              ARM Ltd
Licensed SEGGER software: emWin
License number:           GUI-00181
License model:            LES-SLA-20007, Agreement, effective since October 1st 2011 
Licensed product:         MDK-ARM Professional
Licensed platform:        ARM7/9, Cortex-M/R4
Licensed number of seats: -
----------------------------------------------------------------------
File        : GUIConf.c
Purpose     : Display controller initialization
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "IS61WV25616.h" //added by xuchao on 2016.01.04
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Define the available number of bytes available for the GUI
//
#define GUI_NUMBYTES  (1024*60)
#define GUI_BLOCKSIZE 0x80
/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   available memory for the GUI.
*/
void GUI_X_Config(void) 
{
#if 0
    /* use the internal memaory */
	// 32 bit aligned memory area and GUI_NUMBYTES defines the num of bytes(8bits)
    static U32 aMemory[GUI_NUMBYTES / 4];
    // Assign memory to emWin
    GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
	GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE);
#else
	/* use the external memaory */
	static U32 *aMemory;
	aMemory = (U32 *)SRAM_ADDRESS_BASE;
	/* IS61WV25616B size = 512KB ,address =  0x6000_0000*/
	GUI_ALLOC_AssignMemory(aMemory, 512*1024);
	GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE);
#endif
    // Set default font
    GUI_SetDefaultFont(GUI_FONT_6X8);
}

/*************************** End of file ****************************/
