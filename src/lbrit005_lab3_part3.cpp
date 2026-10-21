/*         Luis & lbrit005@ucr.edu:

            Discussion: 023

 *         Assignment: Lab #3  Exercise # 3

 *         Exercise Description: In this lab we will be doing the same thing but when i reaches
            4 for am and e for fm, the buzzer will turn on and the 
            red led will turn on. When the count is 
            not at those values, the buzzer and the red led will be off. 

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *

 *         Demo Link: <https://youtu.be/7RNsBJpPVNk>
 >

 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "timerISR.h"

unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b)
{
  return (b ? (x | (0x01 << k)) : (x & ~(0x01 << k)));
  // Set bit to 1 Set bit to 0
}

unsigned char GetBit(unsigned char x, unsigned char k)
{
  return ((x & (0x01 << k)) != 0);
}

// TODO: Copy and paste your nums[] and outNum[] from the previous lab. The wiring
//       is still the same, so it should work instantly

// TODO: complete nums
// the index is also the digit it represents
// eg: nums[0]="0", nums[9]="9", nums[15]="f"
// bits layout:   gfedcba   (REVERSE ORDER!)
int nums[16] = {
    0b0111111, // 0
    0b0000110, // 1
    0b1011011, // 2
    0b1001111, // 3
    0b1100110, // 4
    0b1101101, // 5
    0b1111101, // 6
    0b0000111, // 7
    0b1111111, // 8
    0b1101111, // 9
    0b1110111, // a
    0b1111100, // b
    0b0111001, // c
    0b1011110, // d
    0b1111001, // e
    0b1110001  // f
};

// Provided ADC functions
void ADC_init()
{
  ADMUX = 0x40;
  ADCSRA = 0x87;
  ADCSRB = 0x00;
}

unsigned int ADC_read(unsigned char chnl)
{ // used when getting 0-1023 from an analog pin
  //                                  ^^^^ unsigned char chnl selects which pin you're reading the analog signal from
  ADMUX |= 0xFF & chnl;
  ADCSRA = ADCSRA | 0x40;
  while ((ADCSRA >> 6) & 0x01)
  {
  }

  uint8_t low, high;

  low = ADCL;
  high = ADCH;

  return ((high << 8) | low);
}

// Provided map()
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

enum states
{start, am_mode, fm_mode, wait} state; // TODO: finish the enum for the SM

void Tick()
{
  // TODO: declare your static variables here or declare it globally
  unsigned int mode = 0; // 0 for fm, 1 for am
  unsigned int digit = 0; // to get the map for the buzzer
  
  switch (state)
  {
  case start:
    state = am_mode;
    break;
  case am_mode:
    if (GetBit(PINC, 0))
    { // button 0  is on
      state = wait;
      mode = 0;
    }
    break;
  case fm_mode:
    if (GetBit(PINC, 0))
    { // button 0  is on
      mode = 1;
      state = wait;
    }
    break;
  case wait:
    if (GetBit(PINC, 0))
    { // button 0 is released
      if (mode == 0)
      {
        state = fm_mode;
      }
      else
      {
        state = am_mode;
      }
    }
    break;
  default:
    state = start;
    break;
  }
  // State Transistions

  // State Actions

  // TODO: complete transitions
  switch (state)
  {
    case start:
      break;
    case am_mode:
      digit = (int)map(ADC_read(1), 0, 1000, 0, 9);
      PORTD = (PORTD & 0xeF) | 0x10; // turn on the am led
      PORTB = nums[digit] & 0x1F;
      PORTD = (PORTD & 0x3F) | ((nums[digit] & 0x60) << 1);
      // turn on the am led
      if(digit == 4){
        PORTD &= ~(1 << 4); // turn off the am led
        PORTD |= (1 << 2); // buzzer on
        PORTD |= (1 << 5); // red led on
      }
        else{
          PORTD |= (1 << 4); // turn on the am led
          PORTD &= ~(1 << 2); // buzzer off
          PORTD &= ~(1 << 5); // red led off
        }
      break;
    case fm_mode:
      PORTD = (PORTD & 0xeF) | 0x08; // turn on the fm led
      digit = (int)map(ADC_read(1), 0, 1023, 10, 15); // we want to map the potentiometer value to 10-15 since those are the digits for a-f
      PORTB = nums[digit] & 0x1F; // display the digit on the 7-seg
      PORTD = (PORTD & 0x3F) | ((nums[digit] & 0x60) << 1);
      if(digit == 14){
        PORTD &= ~(1 << 3); // turn off the fm led
        PORTD |= (1 << 2); // buzzer on
        PORTD |= (1 << 5); // red led on
      }
        else{
          PORTD |= (1 << 3); // turn on the fm led
          PORTD &= ~(1 << 2); // buzzer off 
          PORTD &= ~(1 << 5); // red led off 

        }
      break;
    case wait:
      break;
    default:
      break;
  }
  PORTB = nums[digit] & 0x1F;
  PORTD = (PORTD & 0x3F) | ((nums[digit] & 0x60) << 1);
}

int main(void) {
  // TODO: initialize all outputs and inputs
  DDRB = 0x1F;  // the rest of the bits are for the 7-seg, so we set them to output
  PORTB = 0x00; // off for now

  DDRC = 0x00; // PC0 is the button and the PC1 is the potentiometer
  PORTC = 0x01;

  DDRD = 0xFC;  //PD2 is the buzzer, PD3-PD5 are the RGB LEDs, PD6-7 is the display
  PORTD = 0x10; // the blue rgb led is on for now

  state = start; // initialize the state variable
  ADC_init();
  TimerSet(50); // TODO: Set your timer
  TimerOn();

  while (1)
  {
    Tick(); // Execute one synchSM tick
    while (!TimerFlag)
    {
    }              // Wait for SM period
    TimerFlag = 0; // Lower flag
  }

  return 0;
}