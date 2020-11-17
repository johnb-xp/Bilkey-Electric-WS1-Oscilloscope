#include "msp.h"
#include "msoe_lib_all.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "bitmap.h"
#include "ee1910delay.h"
#include "analog.h"


/*****************************************************************************
 * PROJECT: EE 2920 Final Project - Oscilloscope
 * ENTITY: MAIN.C
 * AUTHOR: John Bilkey
 * DATE: November 3, 2020
 * PROVIDES: Captures analog inputs, displays a plot on the LCD
 * REQUIRED HARDWARE:
 *  DISPLAY
 *   PIN 8 (backlight) - NOT CONNECTED
 *   PIN 7 (SCLK)      - P9.5
 *   PIN 6 (MISO)      - P9.7
 *   PIN 5 (D/C)       - P9.2
 *   PIN 4 (resetBAR)  - P9.3
 *   PIN 3 (STE)       - P9.4
 *   PIN 2 (GND)       - GND
 *   PIN 1 (VCC)       - 3.3V
 *
 *  BUTTONS
 *   Button 1: P 8.5 with a 10k pull-up resistor (READ)
 *   Button 2: P 8.6 with a 10k Pull-up resistor (SCALE)
 *
 *  ANALOG
 *   PIN 5.5  - Analog Input 0 V+
 *   PIN 5.4  - Analog Input 1 Potentiometer
 ******************************************************************************/

char printinterval = 1; // increment this many times when printing values in the array

int lastRead; // last read from Analog 1

#define timeBetweenReads 500 // microseconds
#define reads 253 // size of array for storing voltage measurements

// global variables for ADC output storage
volatile float results[reads];

void read(void);
void print(int hoffset, int interval);
void useTimer(void);
void timeScale(void);
int encodedButton(void);

void main(void) {

  // set unused pins to resistor enabled to avoid floating inputs
  P1 -> REN |= 0xFF;
  P2 -> REN |= 0xFF;
  P3 -> REN |= 0xFF;
  P4 -> REN |= 0xFF;
  P5 -> REN |= 0xFF;
  P6 -> REN |= 0xFF;
  P7 -> REN |= 0xFF;
  P8 -> REN |= 0xFF;
  P9 -> REN |= 0xFF;
  P10 -> REN |= 0xFF;

  P8 -> OUT |= (BIT5 | BIT6); // Set out register to 1 (input) for buttons on P 8.5

  Stop_watchdog();

  LCD_Config(); // set up ports for LCD and other stuff.

  A2Dsetup(); // prepare A2D (for reading input)

  //bilkeyelectric, grid, reading

  LCD_print_bmpArray(bilkeyelectric);

  LCD_contrast(1); // make LCD easier to read

  delay(1500); // keep that spash screen on there for a bit

  LCD_clear();

  while (1) {
    LCD_print_bmpArray(reading); // tells the user values are being recorded
    A2Dsetup();

    read();
    delay(500);

    A2Dsetup(); // sets up a2d for pot
    A1select();
    delay(500);
    print((A1read() / (4096 / (reads - 84))), printinterval); // initial display print
    while (encodedButton() != 1) { // wait until READ button is pressed
      if (encodedButton() == 2) { // if SCALE button is pressed
        timeScale();
        print((A1read() / (4096 / (reads - 84))), printinterval); // redraw LCD with input from pot for position
      }
      int lastRead = A1read();
      if ((A1read() < lastRead - 5) || (A1read() > lastRead + 5)) { // redraw display if potentiometer changes
        lastRead = A1read();
        print((A1read() / (4096 / (reads - 84))), printinterval);
      }
      delay(100);

    }
  }
}

void read(void) {

  A0select(); // switch to voltage reading pin
  delay(100);

  char index;
  for (index = 0; index < reads; index++) {
    results[index] = (A0read() * (3.3 / 4096));
    delayMicroseconds(timeBetweenReads);
  }
}

void print(int hoffset, int interval) { // displays values in array

  LCD_print_bmpArray(grid); // add grid for background

  char counter = 1; //position in array
  for (counter = 1; counter < 85; counter++) {
    if (((counter * interval) + hoffset) > reads) { // prevent reading out of array
      break;
    }
    LCD_col_exact(counter);
    LCD_row((int)(5-(results[(counter*interval)+hoffset]/0.64))); // row 0-5

    int intpart = (int)(results[(counter*interval)+hoffset]/0.64);
    float decpart = (results[(counter*interval)+hoffset]/0.64) - intpart;
    int subcolumn =  ((int)(decpart *8));
    LCD_print_column(1 << 7-subcolumn);
  }
}

void timeScale(void) { // changes time scale variable and displays on LCD
  LCD_goto_xy(5, 2);
  switch (printinterval) {

  case 1:
    printinterval = 2;
    LCD_print_str("2");
    break;
  case 2:
    printinterval = 3;
    LCD_print_str("3");
    break;
  default:
    printinterval = 1;
    LCD_print_str("1");
    break;
  }
  delay(1000);
}

int encodedButton(void) {
  int temp = 0; // no button pressed returns 0

  if (((P8 -> IN & BIT5) == 0) || ((P8 -> IN & BIT6) == 0)) {
    if (((P8 -> IN & BIT5) == 0) && ((P8 -> IN & BIT6) == 0) || ((P8 -> IN & BIT6) == 0) && ((P8 -> IN & BIT5) == 0)) {
      //printf("/nBOTH");
      temp = 3; // both buttons pressed returns 3
    } else
    if ((P8 -> IN & BIT5) == 0) {
      //printf("/n1");
      temp = 1; // only button 1 pressed returns 1
    } else
    if ((P8 -> IN & BIT6) == 0) {
      //printf("/m2");
      temp = 2; // only button 2 pressed returns 2
    }
  }
  return temp;
}
