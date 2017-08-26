/* ************************************************************************//**
 *  @File:
 *
 *     globals.c
 *
 *  @Project: 
 *
 *     TODO -> Project Name %<%PROJECT_NAME%>% %<%DEFAULT_NAME%>%
 *
 *  @Description:
 *
 *     Target       : TODO -> Provide MCU
 *     Compiler     : XC8
 *     IDE          : MPLABX v3.35
 *     Created      : July 3, 2017
 *
 *  R Hanlen <implod@gmail.com>
 * ***************************************************************************/

/* ************************************************************************//**
 *              @Section: INCLUDED FILES
 *****************************************************************************/

//Compiler-Standard
#include <xc.h>
#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

//Project
#include "globals.h"
/* ***********************************************************************//**
 *              @Section: GLOBAL VARIABLES
 *****************************************************************************/

volatile uint16_t tmrCount = 0;
volatile uint16_t t3Count = 0;
volatile int16_t timerValue = 0;

volatile enum mStates mainState = POWER_ON;
volatile enum mStates prevState = POWER_ON;

volatile t_flags flag = {0, 0, 0, 0, 0, 0};

volatile t_signal signal = {0, 0, 0, 0, 0, 0};

volatile t_hwflags hwflag = {0, 0, 0, 0, 0};

volatile t_time time = {0, 0, 0};

volatile t_button encoderButton;

volatile t_button ledButton;

volatile t_button* buttons[BUTTONS_LEN];

volatile t_coder coder;

/* ***********************************************************************//**
 *              @Section: FUNCTION DEFINITIONS
 *****************************************************************************/


void InitStuctures(void){
	encoderButton.down = false;
    encoderButton.latched = false;
    encoderButton.pressed = false;
    encoderButton.port = &PORTB;
    encoderButton.pin = 6;
    encoderButton.debounce = 0;
    
    ledButton.down = false;
    ledButton.latched = false;
    ledButton.pressed = false;
    ledButton.port = &PORTC;
    ledButton.pin = 0;
    ledButton.debounce = 0;
       
    buttons[0] = &encoderButton;
    buttons[1] = &ledButton;
}

//void ClearFlags(void){
//    flag.encode = false;
//    flag.on = false;
//    flag.secTick = false;
//    flag.main = false;
//}

void ClearButtons(void){

    for(uint8_t i = 0; i < BUTTONS_LEN; i++){
        buttons[i]->pressed = false;
        buttons[i]->debounce = 0;
    }
}



void ChangeState(enum mStates newState){
    prevState = mainState;
    mainState = newState;
}