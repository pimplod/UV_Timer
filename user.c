/* ************************************************************************//**
 *  @File:
 *
 *     user.c
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
 *     Created      : July 17, 2017
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
#include <math.h>

//Project
#include "user.h"

/* ***********************************************************************//**
 *              @Section: GLOBAL VARIABLES
 *****************************************************************************/

/* ***********************************************************************//**
 *              @Section: FUNCTION DEFINITIONS
 *****************************************************************************/

//void ButtonCheck(volatile t_button * button) {
//    //static uint16_t debounce = 0;
//    bool test = (*(button->port) & (1UL << (button->pin)));
//    button->down = !(test);
//
//    if ((button->debounce) > tmrCount)
//        return;
//    if (button->down == true && button->latched == false) {
//        button->latched = true;
//        button->debounce = (tmrCount + 30);
//    }
//    if (button->down == false && button->latched == true) {
//        button->pressed = true;
//        button->latched = false;
//    }
//}

void ButtonCheck(void) {
    buttons[0]->down = !(ENC_BUTTON);
    buttons[1]->down = !(LED_BUTTON);

    //        for (uint8_t i = 0; i < BUTTONS_LEN; i++) {
    //            buts[i]->down = ((*(buts[i]->port) & (1 << (buts[i]->pin))) != 0);
    //        }

    for (uint8_t i = 0; i < BUTTONS_LEN; i++) {
        if (buttons[i]->debounce > tmrCount)
            continue;
        if (buttons[i]->down == true && buttons[i]->latched == false) {
            buttons[i]->latched = true;
            buttons[i]->debounce = tmrCount + 30;
        }
        if (buttons[i]->down == false && buttons[i]->latched == true) {
            buttons[i]->pressed = true;
            buttons[i]->latched = false;
        }
    }
}

int16_t GetTime(void) {
    uint8_t read = 0;
    int16_t value = 0;
    uint8_t scale = 1;

    for (uint8_t i = 0; i < 3; i++) {
        read = EEPROM_READ((LAST_ONES) + i);
        if (read == 0xFF)
            read = 0;
        value += (int16_t) (read * scale);
        scale *= 10;
    }
    return value;
}

void SaveTime(int16_t time) {
    uint8_t value;

    value = (uint8_t) (time % 10);
    EEPROM_WRITE(LAST_ONES, value);
    value = (uint8_t) (time / 10) % 10;
    EEPROM_WRITE(LAST_TENS, value);
    value = (uint8_t) (time / 100) % 10;
    EEPROM_WRITE(LAST_HUND, value);


}

void SetTimer(void) {
    // switch variable that determines the increment value
    uint8_t switchState = 2;
    int8_t amount = 0;
    uint16_t wait;
    static uint16_t holdTime = 0;
    //get timer value saved in eeprom
    timerValue = GetTime();

    wait = tmrCount + 100;
    while (wait > tmrCount) {
        if (ledButton.latched)
            wait = tmrCount + 100;
    }
    ScrollMessage("SET");
    amount = timerValue % 10;
    //clear all button struct variables
    ClearButtons();

    do {
        //Display timer value every loop      
        DisplayValue(timerValue);
        //flash dp of unit being changed
        if (flag.halfsec)
            DisplayDP(switchState);

        switch (switchState) {
            case 2:
                //Pressing encoder button changes the increment of setting
                if (encoderButton.pressed) {
                    ClearButtons();
                    switchState = 1;
                    amount = (timerValue / 10) % 10;
                    break;
                }
                //total changes with rotation of encoder
                if (flag.encode) {
                    flag.encode = false;
                    amount += coder.direction;
                    if (amount < 0) {
                        amount = 9;
                        timerValue += 9;
                    }else if (amount > 9) {
                        amount = 0;
                        timerValue -= 9;
                    }else {
                        timerValue += (int16_t) (coder.direction);
                    }
                }
                break;
            case 1:
                //Pressing encoder button changes the increment of setting
                if (encoderButton.pressed) {
                    ClearButtons();
                    switchState = 0;
                    amount = (timerValue / 100) % 10;
                    break;
                }
                //total changes with rotation of encoder
                if (flag.encode) {
                    flag.encode = false;
                    amount += coder.direction;
                    if (amount < 0) {
                        amount = 9;
                        timerValue += 90;
                    }else if (amount > 9) {
                        amount = 0;
                        timerValue -= 90;
                    }else {
                        timerValue += (int16_t) (10 * coder.direction);
                    }
                }
                break;
            case 0:
                //Pressing encoder button changes the increment of setting
                if (encoderButton.pressed) {
                    ClearButtons();
                    switchState = 2;
                    amount = timerValue % 10;
                    break;
                }
                //total changes with rotation of encoder 
                if (flag.encode) {
                    flag.encode = false;
                    amount += coder.direction;
                    if (amount < 0) {
                        amount = 9;
                        timerValue += 900;
                    }else if (amount > 9) {
                        amount = 0;
                        timerValue -= 900;
                    }else {
                        timerValue += (int16_t) (100 * coder.direction);
                    }
                }
                break;
        }//switch
        //The ledButton signals end of timer setting
        if (ledButton.pressed) {
            DisplayValue(timerValue);
            SaveTime(timerValue);
            ClearButtons();
            ChangeState(READY);
            return;
        }

        if (ledButton.latched && (holdTime < tmrCount)) {
            DisplayMsg(" ON");
            signal.no_timer = true;
            ChangeState(ON_NO_TIMER);
            return;
        }else if (!ledButton.latched) {
            holdTime = tmrCount + 1500L;
        }

    } while (1);
}//SetTimer()

void HandleButtons(void) {
    static uint16_t holdCount = 0;

    if (RELAY_LAT == true || hwflag.relay == true) {
        if (ledButton.pressed || encoderButton.pressed) {
            RELAY_OFF();
            ClearButtons();
            signal.blink_led = false;
            signal.buzzer = false;
            flag.ready = true;
            ChangeState(STOP_CALLED);
        }
    }else {
        switch (mainState) {
            case POWER_ON:
                if (ledButton.pressed) {
                    ClearButtons();
                    flag.ready = false;
                    ChangeState(SET_TIMER);
                }
                break;
            case READY:
                if (ledButton.pressed) {
                    ClearButtons();
                    signal.blink_disp = false;                 
                    flag.on = true;
                    ChangeState(TIMER_ON);
                }
                break;
            case TIMER_OVER:
                if (ledButton.pressed) {
                    ClearButtons();
                    signal.blink_disp = false;
                    signal.buzzer = false;                 
                    ChangeState(SET_TIMER);
                }
                break;
            case STOP_CALLED:
                if (ledButton.latched && (holdCount < tmrCount)) {
                    DisplayClear();
                    uint16_t wait = tmrCount + 300;
                    while (wait > tmrCount) {
                        if (ledButton.latched || ledButton.down)
                            wait = tmrCount + 300;
                    }
                    ClearButtons();
                    flag.ready = false;
                    ChangeState(POWER_ON);
                }else if (!ledButton.latched) {
                    holdCount = tmrCount + 750L;
                }
                break;
        }
    }
}
// <editor-fold defaultstate="collapsed" desc="New Set Timer Attempt">

/*
void NewSet(void) {

    int8_t place = 2;
    int8_t amount = 0;
    uint16_t wait;
    int16_t div[3] = {1, 10, 100};
    int16_t increment[3] = {100, 10, 1};
    static uint16_t holdTime = 0;
    //get timer value saved in eeprom
    timerValue = GetTime();

    wait = tmrCount + 100;
    while (wait > tmrCount) {
        if (ledButton.latched)
            wait = tmrCount + 100;
    }

    ScrollMessage("SET");
    amount = timerValue % 10;

    //clear all button struct variables
    ClearButtons();
    flag.encode = false;

    do {

        //Display timer value every loop      
        DisplayValue(timerValue);
        //flash dp of unit being changed
        if (flag.halfsec)
            DisplayDP(place);

        //Pressing encoder button changes the increment of setting
        if (encoderButton.pressed) {
            ClearButtons();
            if (place == 0)
                place = 2;
            else
                place -= 1;
            //            place--;
            //            if (place < 0)
            //                place = 2;
            //divisor = (uint8_t) pow(10, place);
            amount = (timerValue / div[place]) % 10;
        }

        //total changes with rotation of encoder
        if (flag.encode) {
            flag.encode = false;
            amount += coder.sign;
            if (amount < 0) {
                amount = 9;
                timerValue += (int16_t) (9l * increment[place]);
                //timerValue += (int16_t)(900L / div[place]);
            }else if (amount > 9) {
                amount = 0;
                timerValue -= (int16_t) (9l * increment[place]);
                //timerValue -= (int16_t)(900L / div[place]);
            }else {
                timerValue += (int16_t) ((increment[place]) * coder.sign);
                // timerValue +=(int16_t)((100L/div[place])* coder.sign);
                //timerValue += (int16_t) ((100 * coder.direction) / div[place]);
            }
        }

        //The ledButton signals end of timer setting
        if (ledButton.pressed) {
            DisplayValue(timerValue);
            SaveTime(timerValue);
            ClearButtons();
            ChangeState(READY);
            return;
        }

        if (ledButton.latched && (holdTime < tmrCount)) {
            DisplayMsg(" ON");
            signal.no_timer = true;
            ChangeState(ON_NO_TIMER);
            return;
        }else if (!ledButton.latched) {
            holdTime = tmrCount + 1500L;
        }
    } while (1);
}
 */
// </editor-fold>