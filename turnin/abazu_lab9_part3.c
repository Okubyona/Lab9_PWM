/*	Author: abazu001
 *  Partner(s) Name:
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #2
 *	Exercise Description: [ Using the ATmega1284’s PWM functionality, design a
        system where the notes: C4, D, E, F, G, A, B, and C5,  from the table
        at the top of the lab, can be generated on the speaker by scaling up or
        down the eight note scale. Three buttons are used to control the system.
        One button toggles sound on/off. The other two buttons scale up, or down,
        the eight note scale. ]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *  Demo Link:
 *  https://drive.google.com/file/d/1q0SReTN8EO-zTv82Ys79sg6sZ5tTPLDu/view?usp=sharing
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#define A 440.0
#define B 493.88
#define C 523.25
#define D 587.33
#define D_sharp 622.25
#define E 659.25

typedef enum States {init, wait, showTime, holdTime} States;

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0;   // Current internal count of 1 ms ticks.

void set_PWM (double frequency) {
    static double current_frequency;

    if (frequency != current_frequency) {
        if (!frequency) { TCCR3B &= 0x08; }
        else {TCCR3B |= 0x03; }

        if (frequency < 0.954) { OCR3A = 0xFFFF; }

        else if (frequency > 31250) { OCR3A = 0x0000; }

        else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

        TCNT3 = 0;
        current_frequency = frequency;

    }

}

void PWM_on() {
    TCCR3A = (1 << COM3A0);

    TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);

    set_PWM(0);
}

void PWM_off() {
    TCCR3A = 0x00;
    TCCR3B = 0x00;
}

void TimerOn() {
    TCCR1B = 0x0B;

    OCR1A = 125;

    TIMSK1 = 0x02;

    TCNT1=0;

    _avr_timer_cntcurr = _avr_timer_M;

    SREG |= 0x80;

}

void TimerOff() {
    TCCR1B = 0x00;
}

void TimerISR() {
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
    // CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
    _avr_timer_cntcurr--;   // Count down to 0 rather than up to TOP
    if (_avr_timer_cntcurr == 0) {
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }

}

void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}


int Tick(int state) {
    unsigned char A0 = ~PINA & 0x01;

    double luigisMansion[] = {E, 0, E, 0, E, 0, E, C, E, D_sharp, B, 0, D, 0,
        D, 0, D, 0, D, C, D, E, D, C, B, A, 0}; // song array

    char noteLength[] = {3, 1, 3, 1, 3, 1, 8, 4, 4, 8, 24, 4, 3, 1, 3, 1, 3, 1, 8, 4, 4, 4, 4, 4,
        4, 8, 16};

    static unsigned char index;
    static unsigned char count;
    unsigned char songArrSize = sizeof luigisMansion / sizeof luigisMansion[0];

    switch (state) {
        case init:
            state = wait;
            index = 0;
            count = 0;
            break;

        case wait:
            state = A0 ? showTime : wait;
            break;

        case showTime:
            if (index >= songArrSize) {
                state = A0 ? holdTime: showTime;
            }
            break;

        case holdTime:
            state = A0 ? holdTime: wait;
            break;

        default:
            state = init;
            break;
    }

    switch (state) {
        case init:
            set_PWM(0);
            break;

        case wait:
            index = 0;
            count = 0;
            break;

        case showTime:
            if (index >= songArrSize) {
                index = 0;
                count = 0;
            }
            else if (count < noteLength[index]) {
                set_PWM(luigisMansion[index]);
                ++count;
            }
            else {
                count = 0;
                ++index;
            }
            break;

        case holdTime: break;
    }

    return state;
}


int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0x40; PORTB = 0x00;

	TimerOn();
	TimerSet(66); // 1/32 note of time for 85 BPM
    PWM_on();

    States state = init;

    /* Insert your solution below */
    while (1) {
        state = Tick(state);

        while(!TimerFlag) {}
        TimerFlag = 0;
    }
    return 1;
}
