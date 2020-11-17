#ifndef ANALOG_H_
#define ANALOG_H_

#include "msp.h"

#include <stdint.h>

/*****************************************************************************
 * PROJECT: EE 2920 Final Project - Oscilloscope
 * ENTITY: ANALOG.H
 * AUTHOR: John Bilkey
 * DATE: November 15, 2020
 * PROVIDES: Returns analog to digital converter value, 0 to 4096
 *
 * REQUIRED HARDWARE:
 *   PIN 5.5  - Analog Input 0 Potentiometer
 *   PIN 5.4  - Analog Input 1 V+
 *   PIN 5.2  - Analog Input 3 V-
 *
 *  Includes modified code from Dr. Widder for EE 2920,
 *  Dr. Ross for EE 1910, Wei Zhao at Texas Instruments Inc
 ******************************************************************************/

void A2Dsetup(void) {
  // ADC Setup
  // You must enable the Analog 0 pin...

  ADC14 -> CTL0 = 0x04000210; // S/H timer, 16clk S/H, ADC ON
  ADC14 -> CTL1 = 0x00000020; // 12-bit conversion
}

void A0select(void) {
  // Function to setup Analog input A0
  // for use in A/D conversion
  // Setup ADC Input 0
  // Pin 30 --> P5.5
  P5 -> SEL0 |= BIT5; // Select alternate mode 11
  P5 -> SEL1 |= BIT5;

  P5 -> DIR &= ~BIT5; // input
  P5 -> REN &= ~BIT5; // No pull u/d

  P5 -> SEL0 &= ~BIT4; // Disable other pin
  P5 -> SEL1 &= ~BIT4;
  P5 -> REN |= BIT4;
  P5 -> DIR |= BIT4;

  ADC14 -> MCTL[0] = 0x00000000;
}

void A1select(void) {
  // Function to setup Analog input A1
  // for use in A/D conversion
  // Setup ADC Input 1
  // P5.4

  P5 -> SEL0 |= BIT4; // Select alternate mode 11
  P5 -> SEL1 |= BIT4;

  P5 -> DIR &= ~BIT4; // input
  P5 -> REN &= ~BIT4; // No pull u/d

  P5 -> SEL0 &= ~BIT5; // Disable other pin
  P5 -> SEL1 &= ~BIT5;
  P5 -> DIR |= BIT5;
  P5 -> REN |= BIT5;

  ADC14 -> MCTL[0] |= ADC14_MCTLN_INCH_1;

}

int A0read(void) {

  // Function to perform a single A/D conversion on Analog input 0, P 5.5

  // enable ADC and start conv
  ADC14 -> CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;

  // Wait for conversion to complete
  // Conversion is complete when ADC0 flag is set
  while (!ADC14 -> IFGR0) {}

  // returning value
  return ADC14 -> MEM[0];
}

int A1read(void) {
  // Function to perform a single
  // A/D conversion on Analog input 1
  // Start sampling/conversion
  ADC14 -> CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;

  // Wait for conversion to complete
  // Conversion is complete when ADC0 flag is set
  while (!ADC14 -> IFGR0) {
    ;
  }
  // returning a full int instead of a uint16_t for simplicity
  return ADC14 -> MEM[0];
}

#endif

/*
 * Experimental code for 3 simultaneous reads and single setup
 * Would be used with full bridge rectifier and op-amp to measure AC voltage with no DC offset,
 *
//   PIN 5.5  - Analog Input 0 V+
//   PIN 5.4  - Analog Input 1 V-
//   PIN 5.3  - Analog Input 2 Potentiometer

void analogSetup(void) {

    //Configure GPIO

    P5->DIR &= ~(BIT3|BIT4|BIT5); // make input
    P5->REN &= ~(BIT3|BIT4|BIT5); // No pull up/down resistor

    P5 -> SEL1 |= (BIT3|BIT4|BIT5);  //Enable A/D channel A0-A2
    P5 -> SEL0 |= (BIT3|BIT4|BIT5);

    P5 -> REN |= BIT2; // connect analog 3 to GND with pulldown

    __enable_interrupt();
    NVIC_ISER0 = 1 << ((INT_ADC14 - 16) & 31);
    //Enable ADC interrupt in NVIC module, Turn on ADC14, extend sampling time

    // SIMULTTANEOUS MULTI-SAMPLE MODE
    ADC14 -> CTL0 |= (ADC14ON | ADC14MSC | ADC14SHT0__192 | ADC14SHP | ADC14CONSEQ_3);

    //to avoid overflow of results
    ADC14MCTL0 = ADC14INCH_0; //ref+=AVcc, channel = A0
    ADC14MCTL1 = ADC14INCH_1; //ref+=AVcc, channel = A1
    ADC14MCTL2 = ADC14INCH_2+ADC14EOS; //ref+=AVcc, channel = A2, end seq.

    ADC14IER0 = ADC14IE3; //Enable ADC14IFG.3

    SCB_SCR &= ~SCB_SCR_SLEEPONEXIT; //Wake up on exit from ISR

    while(1) {
     ADC14CTL0 |= ADC14ENC | ADC14SC; //Start conv-software trigger
     }
   }

}

void analogRead(int index){
    // Function to perform a single
    // A/D conversion on Analog input 0

    // Start sampling/conversion
    ADC14->CTL0 |= 0x00000003; // enable ADC, start conversion
    // Wait for conversion to complete
    // Conversion is complete when ADC0 flag is set
    while (!ADC14->IFGR0){}

    A0results[index] = ADC14->MEM[0]; //Move A0 results, IFG is cleared
    A1results[index] = ADC14->MEM[1]; //Move A1 results, IFG is cleared
    A2results = ADC14->MEM[2]; //Store A2, IFG is cleared

    }
}
*/
