/*	Author: abazu001
 *  Partner(s) Name:
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #4
 *	Exercise Description: [ Design a system, using a bank of eight LEDs, where
            the number of LEDs illuminated is a representation of how much light
            is detected. For example, when more light is detected, more LEDs are
            illuminated. ]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

typedef enum States {init, wait, C_4, D_4, E_4} States;

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0;   // Current internal count of 1 ms ticks.

void set_PWM (double frequency) {
    static double current_frequency;

    if (frequency != current_frequency) {
        if (!frequency) { TCCR3B &= 0x08; }
        else { TCCR3B |= 0x03; }

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

void Tick(int state) {
    unsigned char tmpA = ~PINC;

    switch (state) {
        case init:
            state = wait;
            break;

        case wait:
            if (tmpA == 0x01) { state = C_4; }
            else if (tmpA == 0x02) { state = D_4; }
            else if (tmpA == 0x04) { state = E_4; }
            else { state = wait; }
            break;

        case C_4:
            state = (tmpA && 0x01) ? C_4: wait;
            break;

        case D_4:
            state = (tmpA && 0x02) ? D_4: wait;
            break;

        case E_4:
            state = (tmpA && 0x04) ? E_4: wait;
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
            set_PWM(0);
            break;

        case C_4:
            set_PWM(261.63);
            break;

        case D_4:
            set_PWM(293.66);
            break;

        case E_4:
            set_PWM(329.63);
            break;
    }

    return;
}

int main(void) {
    DDRC = 0x00; PORTC = 0xFF;
    DDRB = 0x40; PORTB = 0x00;

    PWM_on();
	
	TimerOn();
	TimerSet(600);

    States state = init;

    /* Insert your solution below */
    while (1) {
		set_PWM(261.63);

		while(!TimerFlag) {}
		TimerFlag = 0;
    }
    return 1;
}


