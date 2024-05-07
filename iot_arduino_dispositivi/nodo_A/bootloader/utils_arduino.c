/*#include <stdio.h>
void gen(uint8_t block[],int size){
    for (int i = 0; i < size; i++) {
        block[i] = rand() % 256;
    }
}*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>

volatile unsigned long timer_millis = 0;

ISR(TIMER1_COMPA_vect) {
    timer_millis++;
}

void initMillisTimer() {
    // Configure Timer/Counter 1 for generating millis
    TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTC mode, prescaler 64
    OCR1A = F_CPU / 64 / 1000 - 1; // Set compare match value for 1ms intervals
    //TIMSK1 |= (1 << OCIE1A);  // Enable compare match interrupt
    sei();                    // Enable global interrupts
}

unsigned long millis() {
    unsigned long millis_value;
    cli();                      // Disable interrupts
    millis_value = timer_millis;
    sei();                      // Enable interrupts
    return millis_value;
}

void initRandomSeed() {
    // Use the millis() value as a seed
    srand((unsigned int) millis());
}

uint8_t getRandomNumber() {
    // Use the timer count as a source of randomness
    return TCNT1;
}

void gen(uint8_t block[], int size) {
    for (int i = 0; i < size; i++) {
        block[i] = (getRandomNumber() + i*41)%256;
        //_delay_ms(500);
    }
}

