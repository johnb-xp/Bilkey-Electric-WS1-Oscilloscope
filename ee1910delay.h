
/*
 * ee1910delay.h
 *
 *  Created on: Dec 5, 2018
 *      Author: ross
 */

#ifndef EE1910DELAY_H_
#define EE1910DELAY_H_

#include "msp.h"

volatile uint32_t ticks=0;

void delayMicroseconds(uint32_t us) {

      // Set up Timer32 for down count with default 3MHz clock
       TIMER32_1->CONTROL = TIMER32_CONTROL_SIZE |
               TIMER32_CONTROL_MODE;

       // Load Timer32 counter with period determined by us
       TIMER32_1->LOAD= us*3;

       // Enable the Timer32 interrupt in NVIC
       __enable_irq();
       NVIC->ISER[0] = 1 << ((T32_INT1_IRQn) & 31);

       // Start Timer32 with interrupt enabled
       TIMER32_1->CONTROL |= TIMER32_CONTROL_ENABLE |
                   TIMER32_CONTROL_IE;

       // Wait for desired time
       while(!ticks){
       }
       ticks = 0;

       // Turn off timer
       TIMER32_1->CONTROL &= ~(TIMER32_CONTROL_ENABLE |
                          TIMER32_CONTROL_IE);
}


void delay(uint32_t ms) {

      // Set up Timer32 for down count with default 3MHz clock
       TIMER32_1->CONTROL = TIMER32_CONTROL_SIZE |
               TIMER32_CONTROL_MODE;

       // Load Timer32 counter with period 3000
       TIMER32_1->LOAD= 3000;

       // Enable the Timer32 interrupt in NVIC
       __enable_irq();
       NVIC->ISER[0] = 1 << ((T32_INT1_IRQn) & 31);

       // Start Timer32 with interrupt enabled
       TIMER32_1->CONTROL |= TIMER32_CONTROL_ENABLE |
                   TIMER32_CONTROL_IE;

       // Wait for desired time
       while(ticks!=ms){
       }
       ticks = 0;

       // Turn off timer
       TIMER32_1->CONTROL &= ~(TIMER32_CONTROL_ENABLE |
                          TIMER32_CONTROL_IE);
}


void T32_INT1_IRQHandler(void)
{
    TIMER32_1->INTCLR |= BIT0;              // Clear Timer32 interrupt flag
    ticks++;                                // Record tick
}
#endif
