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

int16_t GetTime(uint8_t addr) {
    uint8_t read = 0;
    int16_t value = 0;
    uint8_t scale = 1;

    for (uint8_t i = 0; i < 3; i++) {
        read = EEPROM_READ((addr) + i);
        if (read == 0xFF)
            read = 0;
        value += (int16_t) (read * scale);
        scale *= 10;
    }
    return value;
}

void SaveTime(int16_t time) {
    uint8_t value;
    
    if(lastSaveAddr >= EE_LAST_SAVE_ADDR)
         lastSaveAddr = EE_FIRST_SAVE_ADDR;
    else
        lastSaveAddr += SAVE_SIZE;
        
    EEPROM_WRITE(SAVE_ADDR, lastSaveAddr);
    value = (uint8_t) (time % 10);
    EEPROM_WRITE(lastSaveAddr, value);
    value = (uint8_t) (time / 10) % 10;
    EEPROM_WRITE(lastSaveAddr + 1, value);
    value = (uint8_t) (time / 100) % 10;
    EEPROM_WRITE(lastSaveAddr + 2, value);
    
}

void ChooseOperation(void) {
    int8_t operation = 1;
    uint8_t initial = true;
   // uint16_t displayTimer = 0;
    //int16_t timeReturned = 0;
    uint8_t address = 0;

    ClearLatched();
    flag.encode = false;
    address = lastSaveAddr;

    do {

        switch (operation) {
            case 0:
                if(initial){
                    initial = false;
                    WipeLtoR();
                    flag.encode = false;
                }
                DisplayMsg(" ON");
                           
                if (ledButton.pressed) {
                    ClearButtons();
                    ChangeState(TURN_ON);
                    return;
                }
                break;
                
            case 1:
                if(initial){
                    initial = false;
                    //RotateDigits();
                    SpinCCW(0x07);
                    flag.encode = false;
                }
                 DisplayMsg("NEW");

                if (ledButton.pressed) {
                    ClearButtons();
                    ChangeState(SET_TIMER);
                    return;
                }
                break;

            case 2:
                if (initial) {
                    initial = false;
                    WipeRtoL();
                     //SpinCW(0x07);
                     flag.encode = false;
                    timerValue = GetTime(address);
                    DisplayValue(timerValue);
                }
                if (ledButton.pressed) {
                    ClearButtons();
                    //timerValue = timeReturned;
                    ChangeState(READY);
                    return;
                }
                break;

        }

        if (flag.encode) {
            flag.encode = false;
            initial = true;
            if (operation == 2) {
                if ((address == EE_FIRST_SAVE_ADDR) && (coder.direction == -1)) {
                    operation = 1;
                }else if (((address == EE_LAST_SAVE_ADDR) || (address == lastSaveAddr)) && (coder.direction == 1)) {
                    operation = 0;
                }else {
                    address += (3 * coder.direction);
                }
            }else {
                operation += coder.direction;
                if (operation < 0)
                    operation = 0;
            }

        }

    } while (1);
}

void SetTimer(void) {

    int8_t amount = 0;
    static uint16_t holdTime = 0;

    //get last timer value saved in the device eeprom
    //eeSaveAddr = EEPROM_READ(SAVE_ADDR);
    timerValue = GetTime(lastSaveAddr);

    ClearLatched();
    ScrollMessage("SET TIMER");

    //put digit user is changing in var amount
    amount = timerValue % 10;
    //digit used in switch case loop and signal for blinking dp
    dpDigit = 2;
    //signal to blink dp on digit being changed
    signal.dp_blink = true;

    do {
        //Display timer value every loop      
        DisplayValue(timerValue);

        switch (dpDigit) {
            case 2:
                //Pressing encoder button changes the increment of setting
                if (encoderButton.pressed) {
                    ClearButtons();
                    dpDigit = 1;
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
                    dpDigit = 0;
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
                    dpDigit = 2;
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
            signal.dp_blink = false;
            ClearButtons();
            DisplayValue(timerValue);
            SaveTime(timerValue);
            ChangeState(READY);
            return;
        }

        if (ledButton.latched && (holdTime < tmrCount)) {
            signal.dp_blink = false;
            flag.encode = false;
            flag.latched = true;
            ChangeState(CHOOSE_OP);
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
            flag.off = true;
            
            if(mainState == TURN_ON)
                ChangeState(TURN_OFF);
            else
                ChangeState(STOP_CALLED);
        }
    }else {
        switch (mainState) {
            case POWER_ON:
                if (ledButton.pressed) {
                    ClearButtons();
                    ChangeState(CHOOSE_OP);
                }else if(ledButton.latched && (holdCount < tmrCount)){
                    flag.latched = true;
                    
                }else if(!ledButton.latched){
                    holdCount = tmrCount +750L;
                }
                break;
            case READY:
                if (ledButton.pressed) {
                    ClearButtons();
                    signal.blink_disp = false;
                    flag.on = true;
                    ChangeState(ON_TIMED);
                }
                break;
            case TIMER_OVER:
                if (ledButton.pressed) {
                    ClearButtons();
                    signal.blink_disp = false;
                    signal.buzzer = false;
                    ChangeState(CHOOSE_OP);
                }
                break;
            case STOP_CALLED:
                if (ledButton.latched && (holdCount < tmrCount)) {
                    flag.latched = true;
                    ChangeState(POWER_ON);
                }else if (!ledButton.latched) {
                    holdCount = tmrCount + 750L;
                }
                encoderButton.pressed = false;
                break;
            case TURN_OFF:
                if (ledButton.pressed) {
                    ClearButtons();
                    signal.blink_disp = false;
                    signal.buzzer = false;
                    ChangeState(POWER_ON);
                }
                break;
                
        }
    }
}

void EraseData(void){
    
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