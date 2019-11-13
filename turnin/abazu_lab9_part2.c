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
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

typedef enum States {init, wait, power, upScale, downScale, uScaleWait,
                     dScaleWait} States;

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
    unsigned char A1 = ~PINA & 0x02;
    unsigned char A2 = ~PINA & 0x04;

    double eightNotes[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00,
                          493.88, 523.25}; //C, D, E, F, G, A, B, C

    unsigned char noteArrSize = sizeof eightNotes / sizeof eightNotes[0];

    static unsigned char powerFlag;
    static unsigned char index;


    switch (state) {
        case init:
            state = wait;
            break;

        case wait:
            if (A0 && !A1 && !A2) { state = power; }
            else if (!A0 && A1 && !A2) { state = upScale; }
            else if (!A0 && !A1 && A2) { state = downScale; }
            else { state = wait; }
            break;

        case power:
            state = wait;
            break;

        case upScale:
            if (!A0 && A1 && !A2) { state = uScaleWait; }
            else if (A0 && !A1 && !A2) { state = power; }
            else if (!A0 && !A1 && A2) { state = downScale; }
            else if (!A0 && !A1 && !A2) { state = wait; }
            break;

        case downScale:
            if (!A0 && !A1 && A2) { state = dScaleWait; }
            else if (A0 && !A1 && !A2) { state = power; }
            else if (!A0 && A1 && !A2) { state = upScale; }
            else if (!A0 && !A1 && !A2) { state = wait; }
            break;

        case uScaleWait:
            if (!A0 && A1 && !A2) { state = uScaleWait; }
            else {state = wait; }
            break;

        case dScaleWait:
            if (!A0 && !A1 && A2) { state = dScaleWait; }
            else { state = wait ;}
            break;

        default:
            state = init;
            break;
    }

    switch (state) {
        case init:
            set_PWM(0);
            break;

        case wait: break;

        case power:
            if (powerFlag) {
                powerFlag = 0;
                index = 0;
                set_PWM(0);
            }
            else {
                powerFlag = 1;
                set_PWM(eightNotes[index]);
            }
            break;

        case upScale:
            if (index < 7) {
                ++index;
                set_PWM(eightNotes[index]);
            }
            else { set_PWM(eightNotes[index]); }
            break;

        case downScale:
            if (index > 0) {
                --index;
                set_PWM(eightNotes[index]);
            }
            else { set_PWM(eightNotes[index]); }
            break;

        case uScaleWait:
            set_PWM(eightNotes[index]);
            break;

        case dScaleWait:
            set_PWM(eightNotes[index]);
            break;

    }

    return state;
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0x40; PORTB = 0x00;

	TimerOn();
	TimerSet(250);
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
