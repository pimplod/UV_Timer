
/* ************************************************************************//**
 *  @File:
 *
 *     main.c
 *
 *  @Project: 
 *      TimerControl
 *     
 *
 *  @Description:
 *
 *     Target       : PIC18F14K22
 *     Compiler     : XC8
 *     IDE          : MPLABX v3.35
 *     Created      : 8/7/17 
 *
 *  R Hanlen <implod@gmail.com>
 * ***************************************************************************/



#include <xc.h>
#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>      /* For true/false definition */
#include <stdlib.h>

#include "system.h"


void TimerOn(void);
void NoTimer(void);
uint16_t pCount = 0;

/*
                         Main application
 */

void main(void) {

    //Disable for Initializing Beginning State of Hardware
    INTERRUPT_DISABLE();
    // Initialize the uC (i.e. Oscillator,Periphials, Ports)
    InitSystem();
    // Initialize Buttons, Encoder, Buzzer
    InitHardware();
    //Restart all programmed interrupts
    ENABLE_INTERRUPTS();
    //Start main loop
    while (1) {
        switch (mainState) {
            case POWER_ON:
                if (flag.stateChange) {
                    flag.stateChange = false;
                    ScrollMessage("READY");
                    DisplayMsg("RDY");
                    pCount = t3Count + 400;
                }else if (pCount < t3Count) {
                    flag.stateChange = true;
                }

                if (signal.scrollbreak) {
                    signal.scrollbreak = false;
                    flag.stateChange = true;

                }
                break;
            case CHOOSE_OP:
                ChooseOperation();
                break;
            case SET_TIMER:
                SetTimer();
                break;
            case READY:
                signal.blink_disp = true;
                break;
            case ON_TIMED:
                TimerOn();
                break;
            case TIMER_OVER:
                signal.blink_disp = true;
                signal.buzzer = true;
                break;
            case STOP_CALLED:
                if (flag.stateChange) {
                    flag.stateChange = false;
                    ScrollMessage("STOPPED");
                    pCount = tmrCount + 400;
                }else if (pCount < tmrCount) {
                    flag.stateChange = true;
                }

                if (signal.scrollbreak) {
                    signal.scrollbreak = false;
                    ClearButtons();
                    flag.stateChange = true;
                }
                break;
            case TURN_ON:
                //flag set only when state changes 
                if (flag.stateChange) {
                    flag.stateChange = false;
                    ClearLatched();
                    //set on flag for NoTimer() function
                    flag.on = true;
                }/*if(signal.no_timer)*/
                NoTimer();
                break;
            case TURN_OFF:
                if (flag.stateChange) {
                    flag.stateChange = false;
                    signal.blink_disp = true;
                    signal.buzzer = true;
                    SaveTime(timerValue);
                }
                break;
            case ERASE_DATA:
                EraseData();
                break;
            default:
                ChangeState(POWER_ON);
                break;
        }
        //Check Button Flags, once every main loop
        HandleButtons();
        
        if (flag.latched)
            ClearLatched();
    }//while(1)
}

void TimerOn(void) {
    if (flag.on) {
        flag.on = false;
        flag.secTick = false;
        while (flag.secTick == false);
        RELAY_ON();
        flag.secTick = false;
        signal.blink_led = true;
        return;
    }

    if (flag.secTick == true) {
        flag.secTick = false;
        timerValue -= 1;
        DisplayValue(timerValue);
    }
    if (timerValue <= 0) {
        RELAY_OFF();
        signal.blink_led = false;
        signal.buzzer = true;
        DisplayMsg("FIN");
        ChangeState(TIMER_OVER);
    }

}

void NoTimer(void) {
    if (flag.on) {
        flag.on = false;
        flag.secTick = false;
        while (flag.secTick == false);
        RELAY_ON();
        flag.secTick = false;
        signal.blink_led = true;
        timerValue = 0;
        return;
    }
    if (flag.secTick == true) {
        flag.secTick = false;
        timerValue++;
        if (timerValue > 2)
            DisplayValue(timerValue);
    }
}

/**
 End of File
 */