/* 
 * File:   main.c
 * Author: K M Susheela
 *
 * Created on 6 October, 2023, 8:45 PM
 */

#include "main.h"

#pragma config WDTE = OFF

unsigned char sec = 0, min = 0, flag = 0;
int operation_flag = POWER_ON_SCREEN; // Power on Screen

static void init_config(void)
{
    
    // Initialization of CLCD
    init_clcd();
    
    // Initialization of Matrix Keypad
    init_matrix_keypad();
    
    // Initialize RC2 as output
    FAN_DDR = 0;
    FAN = OFF;        // Turn off Fan
    
    // Initialization of Buzzer
    BUZZER_DDR = 0;
    BUZZER = OFF;
    
    // Initialization of Timer 2
    init_timer2();
    
    // Interrupts
    PEIE = ON;
    GIE = ON;
}

void main(void)
{
    init_config();
    
    // Variable Declaration
    unsigned char key;
    int RESET_FLAG;
    
    while(1)
    {
        key = read_matrix_keypad(STATE);
        
        if(operation_flag == MENU_DISPLAY_SCREEN)
        {
            if(key == 1)
            {
                operation_flag = MICRO_MODE;
                RESET_FLAG = MODE_RESET;
                clear_screen();
                clcd_print(" Power = 900W   ", LINE2(0));
                __delay_ms(3000);
                clear_screen();
            }

            // Grill Mode
            else if(key == 2)
            {
                operation_flag = GRILL_MODE;
                RESET_FLAG = MODE_RESET;
                clear_screen();
            }
            // Convection Mode
            else if(key == 3)
            {
                operation_flag = CONVECTION_MODE;
                RESET_FLAG = MODE_RESET;
                clear_screen();
            }

            // Start Mode
            else if(key == 4)
            {
                clear_screen();
                sec = 30;
                min = 0;
                FAN = ON;
                TMR2ON = ON;
                operation_flag = TIME_DISPLAY;
                
            }
        }
        else if(operation_flag == TIME_DISPLAY)
        {
            // Start / Resume
            if(key == 4)
            {
                sec = sec + 30;
                if(sec > 59)
                {
                    min++;
                    sec = sec - 60;
                }   
            }
            
            // Pause
            else if(key == 5)
            {
                operation_flag = PAUSE;
            }
            
            // Stop
            else if(key == 6)
            {
                operation_flag = STOP;
                clear_screen();
            }
        }
        
        // Resume
        else if(operation_flag == PAUSE)
        {
            if(key == 4)
            {
                FAN = ON;
                TMR2ON = ON;
                operation_flag = TIME_DISPLAY;
            }
        }
        
        
        switch(operation_flag)
        {
            case POWER_ON_SCREEN:
                power_on_screen();
                operation_flag = MENU_DISPLAY_SCREEN;       // Menu Display Screen
                clear_screen();
                break;
                
            case MENU_DISPLAY_SCREEN:
                menu_display_screen();
                break;
                
            case MICRO_MODE:
                set_time(key, RESET_FLAG);
                break;
                
            case GRILL_MODE:
                set_time(key, RESET_FLAG);
                break;
                
            case CONVECTION_MODE:
                
                if(flag == 0)
                {
                    set_temp(key, RESET_FLAG);
                    if(flag == 1)
                    {
                        clear_screen();
                        RESET_FLAG = MODE_RESET;
                        continue;
                    }
                }
                else if(flag == 1)
                {
                    set_time(key, RESET_FLAG);
                }
                break;
                
            case TIME_DISPLAY:
                time_display_screen();
                break;
            
            case START:
                
                break;
                
            case PAUSE:
                FAN = OFF;
                TMR2ON = OFF;
                break;
                
            case STOP:
                FAN = OFF;
                TMR2ON = OFF;
                operation_flag = MENU_DISPLAY_SCREEN;
                break;
        }
       
        RESET_FLAG = RESET_NOTHING; 
    }
}

void time_display_screen(void)
{
    clcd_print(" TIME = ", LINE1(0));
    
    //Print Minutes & Seconds
    clcd_putch( min/10 + '0', LINE1(9));
    clcd_putch( min%10 + '0', LINE1(10));

    clcd_putch(':', LINE1(11));
    
    clcd_putch( sec/10 + '0', LINE1(12));
    clcd_putch( sec%10 + '0', LINE1(13));
    
    // Print Options
    clcd_print("4.START/RESUME", LINE2(0));
    clcd_print("5.PAUSE", LINE3(0));
    clcd_print("6.STOP", LINE4(0));
    
    if((sec == 0) && (min == 0))
    {
        clear_screen();
        clcd_print(" Time UP !! ", LINE2(0));
        FAN = OFF;
        BUZZER = ON;
        __delay_ms(3000);
        BUZZER = OFF;
        
        // Switch off the timer
        TMR2ON = OFF;
        
        clear_screen();
        
        // Return to Menu Screen
        operation_flag = MENU_DISPLAY_SCREEN;
        
    }
}

void clear_screen()
{
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(500);
}


void power_on_screen(void)
{
    unsigned char i;
    
    // Line 1
    for(i = 0; i < 16; i++)
    {
        clcd_putch(BAR, LINE1(i));
        __delay_us(1000);
    }
    
    // Line 2
    clcd_print("  POWERING ON   ", LINE2(0));
    
    // Line 3
    clcd_print(" MICROWAVE OVEN ", LINE3(0));
    
    // Line 4
    for(i = 0; i < 16; i++)
    {
        clcd_putch(BAR, LINE4(i));
        __delay_us(1000);
    }
    
    // Delay
    __delay_ms(3000);       // 3 Seconds Delay
}

void menu_display_screen(void)
{
    //Line 1
    clcd_print("1.Micro", LINE1(0));
    clcd_print("2.Grill", LINE2(0));
    clcd_print("3.Convection", LINE3(0));
    clcd_print("4.Start", LINE4(0));
}


void set_temp(unsigned char key, int RESET_FLAG)
{
    static unsigned char key_count, blink;
    static unsigned char temp;
    static int wait;
    
    if(RESET_FLAG == MODE_RESET)
    {
        key_count = 0;
        wait = 0;
        blink = 0;
        temp = 0;
        flag = 0;
        key = ALL_RELEASED;
        
        clcd_print("SET TEMP(oC)  ", LINE1(0));
        clcd_print(" TEMP = ", LINE2(0));

        // Seconds -> 0 to 59

        clcd_print("*:CLEAR  #:ENTER", LINE4(0));
    }
    
    if((key != '*') && (key != '#') && (key != ALL_RELEASED))
    {
        key_count++;
        
        // Reading number  of seconds
        if(key_count <= 3)
        {
            temp = temp*10 + key;
        }
    }
    
    else if(key == '*')
    {
        temp = 0;
        key_count = 0;
    }
    
    else if(key == '#')
    {
        if(temp <= 10)
        {
            clear_screen();
            clcd_print("  Pre-Heating  ", LINE1(0));
            clcd_print("Time Rem. = ", LINE3(0));
            TMR2ON = 1;
            sec = 9;

            while(sec != 0)
            {
                //clcd_putch((sec/100) + '0', LINE3(11));
                clcd_putch((sec/10)%10 + '0', LINE3(11));
                clcd_putch((sec%100) + '0', LINE3(12));
            }

            if(sec == 0)
            {
                flag = 1;
                TMR2ON = 0;
                // Set Time Screen exactly like Grill mode
                //operation_flag = GRILL_MODE;
                
            }
        }
        else
        {
            clcd_print("Invalid Range", LINE2(0));
            __delay_ms(3000);
            operation_flag = MENU_DISPLAY_SCREEN;
            clear_screen();
        }
    }
    
    if(wait++ == 15)
    {
        wait = 0;
        blink = !blink;
        
        // Printing Temperature on Set Temp Screen
        clcd_putch((temp/100) + '0', LINE2(8));
        clcd_putch((temp/10)%10 + '0', LINE2(9));
        clcd_putch(temp%100 + '0', LINE2(10));
    }
    
    if(blink)
    {
        clcd_print("   ", LINE2(8));
    }

}

void set_time(unsigned char key, int RESET_FLAG)
{   
    static unsigned char key_count, blink_pos, blink;
    static int wait;
    
    if(RESET_FLAG == MODE_RESET)
    {
        key_count = 0;
        sec = 0;
        min = 0;
        blink_pos = 0;  // Seconds
        wait = 0;
        blink = 0;
        key = ALL_RELEASED;
        
        clcd_print("SET TIME (MM:SS)", LINE1(0));
        clcd_print("TIME- ", LINE2(0));

        // Seconds -> 0 to 59

        clcd_print("*:CLEAR  #:ENTER", LINE4(0));

    }
    
    if((key != '*') && (key != '#') && (key != ALL_RELEASED))
    {
        key_count++;
        
        // Reading number of seconds
        if(key_count <= 2)
        {
            sec = sec * 10 + key;
            blink_pos = 0;
        }
        else if ((key_count > 2) && (key_count <= 4))
        {
            min = min * 10 + key;
            blink_pos = 1;
        }
    }
    
    else if(key == '*')
    {
        // To clear seconds
        if(blink_pos == 0)
        {
            sec = 0;
            key_count = 0;
        }
        
        // To clear Minutes
        else if(blink_pos == 1)
        {
            min = 0;
            key_count = 2;
        }
    }
    
    // Enter Key
    else if(key == '#')
    {
        clear_screen();
        operation_flag = TIME_DISPLAY;
        FAN = ON;
        
        // Switching Timer 2 ON
        TMR2ON= ON;
        
    }
    
    if(wait++ == 20)
    {
        wait = 0;
        blink = !blink;
        
        clcd_putch( min/10 + '0', LINE2(6));
        clcd_putch( min%10 + '0', LINE2(7));

        clcd_putch(':', LINE2(8));

        clcd_putch( sec/10 + '0', LINE2(9));
        clcd_putch( sec%10 + '0', LINE2(10));
    }
    
    if(blink)
    {
        switch(blink_pos)
        {
            // For Seconds
            case 0:
                clcd_print("  ", LINE2(9));
                break;
            
            // For Minutes
            case 1:
                clcd_print("  ", LINE2(6));
                break;
        }
    }
    
}
