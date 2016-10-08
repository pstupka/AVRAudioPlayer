/*******************************************************************************

AUTHOR = Patryk Stupka
FILE = AVRAudioPlayer.c

This is a simple project for playing audio on AVR microcontrollers.

mcu = atmega328P
sampling rate = 8k or 16 - depends on sound length etc.

OCR0A:
  249 for 16 kHz
  499 for 8 kHz - need to set WGM11 and WGM12 instead of WGM10 for 10-bit res.


*******************************************************************************/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "data/ah_yes.h"

#define PWM_DDR DDRB
#define PWM_PIN PB1

#define SAMPLING_RATE 16000
#define F_CPU 8000000
#define PRESC 1
#define COUNT_OCR0A (F_CPU/(2*PRESC*SAMPLING_RATE)-1)

void init(){
    //Timer1 PWM
    PORTB = 0xFF;
    PWM_DDR |= (1<<PWM_PIN);
    TCCR1A |= (1<<COM1A1) | (1<<WGM10);   //Set 8bit non inverting PWM
    TCCR1B |= (1<<WGM12) | (1<<CS10);     //Set fast pwm to 31.25 kHz
    OCR1A = 0;                            //Set duty cycle to 0

    //Timer0 CTC - to match sampling rate
    TCCR0A |= (1<<COM0A0) | (1<<WGM01);   //Toggle OC0A, set CTC
    TIFR0 |= (1<<TOV0);                   //clear interrupt flag
    TCCR0B |= (1<<CS00);                  //Set prescaler to 0
    OCR0A = COUNT_OCR0A;
    TIMSK0 |= (1<<OCIE0A);                //Enable interrupts in CTC mode
    sei();                                //Enable interrupts
}

ISR(TIMER0_COMPA_vect){
    static uint16_t sample = 0;
    OCR1A = pgm_read_byte(&ah_yes_wav[sample])/3;
    sample++;
    sample = sample%ah_yes_wav_len;
}

int main(){
    init();

    while(1){};
    return 0;
}
