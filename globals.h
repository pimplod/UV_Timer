/**************************************************************************//**
 *  @File:
 *
 *     globals.h
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
 *     Created      : 8/27/17 4:21 AM
 *
 *  R Hanlen <implod@gmail.com>
 * ***************************************************************************/

#ifndef GLOBALS_H
#define	GLOBALS_H

/**
        @Section: Included Files
 */

#include "system.h"

/**
        @Section: Variables and Datat Structures
 */

extern volatile uint16_t tmrCount;
extern volatile uint16_t t3Count;
extern volatile int16_t timerValue;
extern volatile uint8_t eeSaveAddr;
extern volatile uint8_t dpDigit;

enum mStates {POWER_ON = 0 , CHOOSE_OP, SET_TIMER, READY, ON_TIMED, TIMER_OVER, STOP_CALLED, ON_NT};

extern volatile enum mStates mainState; 
extern volatile enum mStates prevState;

/*      GENERAL SYSTEM bitFLAGS        */
typedef struct flagStruct {
    unsigned secTick : 1;
    unsigned encode : 1;
    unsigned halfsec : 1;
    unsigned on : 1;
    unsigned main : 1;
    unsigned ready : 1;
    unsigned no_timer :1;
    unsigned : 1;
} t_flags;

extern volatile t_flags flag;
 
/*       HARDWARE SIGNAL BITFLAGS           */
typedef struct signalStruct{
    unsigned buzzer : 1;
    unsigned blink_led : 1;
    unsigned relay :1;
    unsigned blink_disp :1;
    unsigned scrollbreak :1;
    unsigned no_timer :1;
    unsigned dp_blink :1;
    unsigned :1;    
}t_signal;

extern volatile t_signal signal;


/*       HARDWARE STATUS BITFLAGS          */
typedef struct hwflagStruct{
    unsigned disp_on        : 1;
    unsigned buzzer         : 1;
    unsigned relay          : 1;
    unsigned bled           : 1;
    unsigned dp_on          : 1;
    unsigned : 3;
} t_hwflags;

extern volatile t_hwflags hwflag;

/*      TIME STRUCTURE for external watch crystal    */
typedef struct timeStruct {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
} t_time;

extern volatile t_time time;

/*      BUTTON STRUCT               */
typedef struct buttonStruct {
    struct {
        unsigned down : 1; //button pressed down
        unsigned latched : 1; //button held down
        unsigned pressed : 1; //button pressed/held/and released
        unsigned : 5;
    };
    volatile unsigned char * port;
    uint8_t pin;
    int16_t debounce;
} t_button;

extern volatile t_button encoderButton;
extern volatile t_button ledButton;
extern volatile t_button* buttons[];

/*              ENCODER STRUCT          */
typedef struct encoderStruct{
    uint8_t newstate;
    uint8_t oldstate;
    uint8_t fullstate;
    int8_t direction;
    int16_t count;
    int16_t sign;
}t_coder;

extern volatile t_coder coder;

/*               FUNCTION DECLARATIONS            */

void InitHardwareVars(void);
void ClearButtons(void);
void ClearLatched(void);
void ChangeState(enum mStates newState);
#endif	/* GLOBALS_H */