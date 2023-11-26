/* 
 * File:   main.h
 * Author: K M Susheela
 *
 * Created on 6 October, 2023, 8:56 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#include "clcd.h"
#include "matrix_keypad.h"
#include "timers.h"
#include <string.h>

#define POWER_ON_SCREEN         0x01
#define MENU_DISPLAY_SCREEN     0x02
#define MICRO_MODE              0x03
#define GRILL_MODE              0x04
#define CONVECTION_MODE         0x05
#define START_MODE              0x06
#define TIME_DISPLAY            0x07
#define START                   0x08
#define PAUSE                   0x09
#define STOP                    0x0A

#define MODE_RESET              0x10              
#define RESET_NOTHING           0x0F
#define RESET_TEMP              0x11

#define FAN                     RC2
#define FAN_DDR                 TRISC2
#define BUZZER                  RC1                     
#define BUZZER_DDR              TRISC1
#define DOOR                    RB3
#define DOOR_DDR                TRISB3

#define ON                      1
#define OFF                     0

void clear_screen(void);
void power_on_screen(void);
void menu_display_screen(void);
void set_time(unsigned char key, int RESET_FLAG);
void time_display_screen(void);
void set_temp(unsigned char key, int RESET_FLAG);

#endif	/* MAIN_H */

