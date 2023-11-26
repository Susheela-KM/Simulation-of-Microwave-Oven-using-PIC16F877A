/* 
 * File:   isr.c
 * Author: K M Susheela
 *
 * Created on 6 October, 2023, 11:23 PM
 */

#include "main.h"

extern unsigned char sec, min;
void __interrupt() isr(void)
{
    static unsigned int count = 0;
    
    if (TMR2IF == 1)
    {    
        if (++count == 1250) // 1sec
        {
            count = 0;
            
            if(sec > 0)
            {
                sec--;
            }
            else if((sec == 0) && (min > 0))
            {
                min--;
                sec = 59;
            }
            
        }
        
        TMR2IF = 0;
    }
}
