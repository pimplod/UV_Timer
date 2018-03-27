/**************************************************************************//**
 *  @File:
 *
 *     display.h
 *
 *  @Project: 
 *
 *     TODO -> Project Name
 *
 *  @Description:
 *
 *     Target       : TODO -> Provide MCU
 *     Compiler     : XC8
 *     IDE          : MPLABX v3.35
 *     Created      : July 4, 2017
 *
 *  R Hanlen <implod@gmail.com>
 * ***************************************************************************/

#ifndef DISPLAY_H
#define	DISPLAY_H

/**
        @Section: Included Files
 */

#include "system.h"

/**
        @Section: Macro Declarations
 */
#define NUMBER_OF_DIGITS    3
#define USE_DECODE          0
#define INITIAL_INTENSITY   12
#define BUFFER_MAX          2
#define DISP_ONES           0x01
#define DISP_TENS           0x02
#define DISP_HUND           0x04

#define PRNTMSG(msg)    DisplayMsg(#msg)
#define SCROLLMSG(msg)  ScrollMessage(#msg)
#define SYNC_DISP()    DisplaySync()
#define BLANK   32
#define DISP_ON()       DisplayOn()
#define DISP_OFF()      DisplayOff()
#define CLEAR_DISP()    DisplayClear()
/**
        @Section: System APIs
 */
void InitDisplay(void);
void DisplayDP(uint8_t digit);
void DisplayValue(int16_t number);
void DisplayNumber(uint8_t digit, uint8_t value);
void DisplayChar(uint8_t digit, uint8_t value);
void DisplayMsg(const char *msg);
void ScrollMessage(const char *string);
void ShiftDisplayLeft(void);
void DisplayOn(void);
void DisplayOff(void);
void DisplayClear(void);
void DisplaySync(void);
void SpinCCW(uint8_t digits);
void SpinCW(uint8_t digits);
void RotateDigits(void);
void WipeRtoL(void);
void WipeLtoR(void);
#endif	/* DISPLAY_H */

