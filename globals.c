/* ************************************************************************//**
 *  @File:
 *
 *     globals.c
 *
 *  @Project: 
 *
 *     UV_Timer
 *
 *  @Description:
 *
 *     Target       : PIC18F14K22
 *     Compiler     : XC8
 *     IDE          : MPLABX v3.35
 *     Created      : 8/27/17 4:22 AM
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
volatile uint8_t lastSaveAddr = 0;
volatile uint8_t dpDigit = 0;

volatile enum mStates mainState = POWER_ON;
volatile enum mStates prevState = POWER_ON;

volatile t_flags flag = {0, 0, 0, 0, 0, 0, 0, 0};

volatile t_signal signal = {0, 0, 0, 0, 0, 0, 0};

volatile t_hwflags hwflag = {0, 0, 0, 0, 0};

volatile t_time time = {0, 0, 0};

volatile t_button encoderButton;

volatile t_button ledButton;

volatile t_button* buttons[BUTTONS_LEN];

volatile t_coder coder;

/* ***********************************************************************//**
 *              @Section: FUNCTION DEFINITIONS
 *****************************************************************************/

void InitHardwareVars(void) {

    //initialize variables within the encoder struct
    coder.newstate = READ_ENCODER();
    coder.oldstate = coder.newstate;
    coder.direction = 0;
    coder.count = 0;
    coder.sign = 1;

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

    lastSaveAddr = EEPROM_READ(SAVE_ADDR);
}

void ClearButtons(void) {

    for (uint8_t i = 0; i < BUTTONS_LEN; i++) {
        buttons[i]->pressed = false;
        buttons[i]->debounce = 0;
    }
}

void ChangeState(enum mStates newState) {
    prevState = mainState;
    mainState = newState;
    flag.stateChange = true;
}

void ClearLatched(void) {
    uint16_t wait = 0x00;
    //    wait = tmrCount+100;
    //    while (wait > tmrCount) {
    //        if (ledButton.latched)
    //            wait = tmrCount + 100;
    //    }
    do {
        if(flag.latched)
            SpinCW(0x07);
        if (ledButton.latched)
            wait = tmrCount + 100;
    } while (wait > tmrCount);
    flag.latched = false;
    ClearButtons();
}