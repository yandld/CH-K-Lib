#ifndef _CHGUI_CHAR_H_
#define _CHGUI_CHAR_H_

#include "stdint.h"


#define GUI_PRINTF_MAX_CHAR_LEN          (64)

#define GUI_TEXTMODE_NORMAL              (0)
#define GUI_TEXTMODE_TRANSPARENT         (2)


//API Functions
void GUI_SetFont(uint8_t FontIndex);
uint8_t GUI_GetFont(void);
uint32_t GetCharDistX(uint8_t FontIndex);
uint32_t GetCharDistY(uint8_t FontIndex);
void GUI_SetTextMode(uint8_t TextMode);
uint8_t GUI_GetTextMode(void);
void GUI_DispChar(uint8_t c);
void GUI_DispCharAt(uint16_t x, uint16_t y, uint8_t c);
void GUI_DispString(uint8_t* pch);
void GUI_DispStringAt(uint16_t x, uint16_t y, uint8_t* pch);
int GUI_printf(const char *format,...);
void GUI_SetFontFormName(const char *pch);
uint8_t GUI_GetNumOfFonts(void);
char* GUI_GetFontNameFormIndex(uint8_t FontIndex);
uint32_t GUI_GetDeivceID(void);

#endif

