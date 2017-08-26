
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
//#include "user.h"


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
    //Open Communication with Display Driver
    InitDisplay();
    //Restart all programmed interrupts
    ENABLE_INTERRUPTS();
    //Start main loop
    while (1) {

        switch (mainState) {

            case POWER_ON:
                if (flag.ready) {
                    flag.ready = false;
                    ScrollMessage("READY");
                    pCount = tmrCount + 400;
                }else if (pCount < tmrCount){
                    flag.ready = true;
                }
                
                if(signal.scrollbreak){
                    signal.scrollbreak = false;
                    ClearButtons();
                    ChangeState(SET_TIMER);
                }
                break;

            case SET_TIMER:
                SetTimer();
                break;

            case READY:
                signal.blink_disp = true;
                break;

            case TIMER_ON:
                TimerOn();
                break;

            case TIMER_OVER:
                signal.blink_disp = true;
                signal.buzzer = true;
                break;

            case STOP_CALLED:
                if (flag.ready) {
                    flag.ready = false;
                    ScrollMessage("STOPPED");
                    pCount = tmrCount + 400;
                }else if (pCount < tmrCount) {
                    flag.ready = true;
                }

                if (signal.scrollbreak) {
                    signal.scrollbreak = false;
                    flag.ready = true;
                }
                break;
                
            case ON_NO_TIMER:
                //flag set only when state changes 
                if (signal.no_timer) {
                    signal.no_timer = false;
                    //Small delay loop to allow release of latched button 
                    pCount = tmrCount + 500;
                    while (pCount > tmrCount) {
                        if (ledButton.down || ledButton.latched)
                            pCount = tmrCount + 500;
                    }
                    //Clear button pressed flags once latch is released
                    ClearButtons();                
                    //set on flag for NoTimer() function
                    flag.on = true;
                }/*if(signal.no_timer)*/
                
                //Main Function for ON_NO_TIMER Case 
                NoTimer();              
                break;
            default:
                ChangeState(POWER_ON);
                break;
        }
        
        //Check Button Flags, once every main loop
        HandleButtons();

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

void NoTimer(void){
    if(flag.on){
        flag.on = false;
        RELAY_ON();
        flag.secTick = false;
        signal.blink_led = true;
        timerValue = 0;
         return;
    }
    if (flag.secTick == true){
        flag.secTick = false;
        timerValue++;
        if(timerValue > 3)
            DisplayValue(timerValue);
    }
}

/**
 End of File
 */