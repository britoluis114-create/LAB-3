/*         Luis & lbrit005@ucr.edu:

            Discussion: 023

 *         Assignment: Lab #3  Exercise # 1

 *         Exercise Description: In this exercise we will be using the ADC to read an analog signal from a potentiometer and print the value to the serial monitor. The value should range from 0 to 1023, where 0 corresponds to 0V and 1023 corresponds to 5V.

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *

 *         Demo Link: <https://youtu.be/eXDnbkM_iKE >

 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "timerISR.h"
#include "serialATmega.h"


void ADC_init() {
ADMUX = 0x40;
ADCSRA = 0x87;
ADCSRB = 0x00;
}
unsigned int ADC_read(unsigned char chnl){ // used when getting 0-1023 from an analog pin
// ^^^^ unsigned char chnl selects which pin you're reading the analog signal from
ADMUX |= 0xFF & chnl;
ADCSRA = ADCSRA | 0x40;
while((ADCSRA >> 6)&0x01) {}
uint8_t low, high;
low = ADCL;
high = ADCH;
return ((high << 8) | low);
}
// Make sure to at least understand what these functions are doing
void Tick() {
// TODO: Implement your Tick Function
    enum states{start, turn} state = start; // State machine states

    switch(state) { // State machine transitions
        case start:
            state = turn;
            break;
        case turn:
            state = turn;
            break;
        default:
            state = start;
            break;
    }
    switch(state) { // State machine actions
        case start:
            break;
        case turn:
                ADC_read(2);
                serial_println(ADC_read(2));
            
            break;
        default:
            break;
    }
}
int main(void){
// TODO: Initialize your I/O pins
DDRC = 0x00;
PORTC = 0x00;
DDRB = 0x00;
PORTB = 0x00;
DDRD = 0x00;
PORTD = 0x00;
ADC_init();
serial_init(9600);
TimerSet(100);
TimerOn();
while (1){
Tick(); // Execute one synchSM tick
while (!TimerFlag){} // Wait for SM period
TimerFlag = 0;
}
}