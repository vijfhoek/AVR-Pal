#define F_CPU 25000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "/usr/include/simavr/avr/avr_mcu_section.h"
AVR_MCU(F_CPU, "atmega328p");

const struct avr_mmcu_vcd_trace_t _mytrace[]  _MMCU_ = {
        { AVR_MCU_VCD_SYMBOL("PORTB"), .what = (void*)&PORTB, },
};

#define SYNC_PIN 7
#define VID_PIN 6

#define U_SYNC  0b00000000
#define U_BLACK 0b10000000
#define U_GREY  0b01000000
#define U_WHITE 0b11000000

#define BIT_VID  3
#define BIT_SYNC 2

#define cbi(x, y) x &= ~(1 << y)
#define sbi(x, y) x |= (1 << y)

volatile uint16_t scanline;
volatile uint8_t syncsleft = 5;
volatile uint8_t field = 0; // 0 if odd (313 lines), 1 if even (312 lines)
volatile uint16_t bp_size = (8UL*F_CPU)/1E6; // Size of the back porch, in cycles

char lines[305];

void (*int_func)(void);


void LongSyncPulseS(void);
void LongSyncDelayS(void);
void ShortSyncPostPulseS(void);
void ShortSyncPostDelayS(void);
void HSyncPulseS(void);
void BackPorchS(void);
void ImageDataS(void);
void ShortSyncPrePulseS(void);
void ShortSyncPreDelayS(void);


ISR (TIMER1_COMPA_vect) {
	(*int_func)();
}
/*
void LongSyncPulse(void) {
	cbi(PORTB, BIT_SYNC);
	OCR1A = 750;
	//TIFR1 |= (1 << OCF1A);
	int_func = &LongSyncDelay;
}

void LongSyncDelay(void) {
	sbi(PORTB, BIT_SYNC);
	OCR1A = 30;
	//TIFR1 |= (1 << OCF1A);
	if (--syncsleft > 0) {
		int_func = &LongSyncPulse;
	} else {
		syncsleft = 5 - field;
		int_func = &ShortSyncPostPulse;
	}
}

void ShortSyncPostPulse(void) {
	cbi(PORTB, BIT_SYNC);
	OCR1A = 50;
	//TIFR1 |= (1 << OCF1A);
	int_func = &ShortSyncPostDelay;
}

void ShortSyncPostDelay(void) {
	sbi(PORTB, BIT_SYNC);
	OCR1A = 750;
	//TIFR1 |= (1 << OCF1A);
	if (--syncsleft > 0) {
		int_func = &ShortSyncPostPulse;
	} else {
		int_func = &HSyncPulse;
	}
}

void HSyncPulse(void) {
	cbi(PORTB, BIT_SYNC);
	cbi(PORTB, BIT_VID);
	//TIFR1 |= (1 << OCF1A);
	OCR1A = 100;
	int_func = &BackPorch;
}

void BackPorch(void) {
	sbi(PORTB, BIT_SYNC);
	//TIFR1 |= (1 << OCF1A);
	OCR1A = 200;
	int_func = &ImageData;
}

void ImageData(void) {
	//PORTB = (cbi(PORTB, BIT_VID)) | lines[scanline] << BIT_VID;
	//TIFR1 |= (1 << OCF1A);
	OCR1A = 1300;
	int_func = HSyncPulse;
	if (++scanline == 305) {
		scanline = 0;
		syncsleft = 5 + field;
		int_func = &ShortSyncPrePulse;
	}
}

void ShortSyncPrePulse(void) {
	cbi(PORTB, BIT_VID);
	cbi(PORTB, BIT_SYNC);
	//TIFR1 |= (1 << OCF1A);
	OCR1A = 50;
	int_func = &ShortSyncPreDelay;
}

void ShortSyncPreDelay(void) {
	sbi(PORTB, BIT_SYNC);
	//TIFR1 |= (1 << OCF1A);
	OCR1A = 750;
	if (--syncsleft) {
		int_func = &ShortSyncPrePulse; 
	} else {
		field = !field;
		syncsleft = 5;
		int_func = &LongSyncPulse;
	}
}
*/


int main(void) {
        int i;

        // Fill the lines!
        for (i = 0; i < 40; i++) lines[i] = 1;
        for (; i < 80; i++)  lines[i] = 0;
        for (; i < 120; i++) lines[i] = 1;
        for (; i < 140; i++) lines[i] = 0;
        for (; i < 160; i++) lines[i] = 1;
        for (; i < 180; i++) lines[i] = 0;
        for (; i < 190; i++) lines[i] = 1;
        for (; i < 200; i++) lines[i] = 0;
        for (; i < 210; i++) lines[i] = 1;
        for (; i < 220; i++) lines[i] = 0;
        for (; i < 230; i++) lines[i] = 1;
        for (; i < 250; i++) lines[i] = 0;
	for (; i < 270; i++) lines[i] = 1;
	for (; i < 290; i++) lines[i] = 0;
	for (; i < 305; i++) lines[i] = 1;

	int_func = &LongSyncPulseS;

        // Set the sync pin and video pin as outputs
        sbi(DDRB, BIT_VID);
	sbi(DDRB, BIT_SYNC);

	// Make sure they're defaulted
	cbi(PORTB, BIT_VID);
	sbi(PORTB, BIT_SYNC);

	syncsleft = 5;

        // Configure timer 1 for Clear on Timer Compare (CTC)
        TCCR1B |= 1 << WGM12;

	// Set up the timers
	TCNT1 = 0;
        OCR1A = 1000;

        // Start timer without prescaler
        TCCR1B |= 1 << CS10 | 0 << CS11;

	// Enable compare interrupt
	TIMSK1 |= 1 << OCIE1A;

	// Enable interrupts
	sei();

        while (1);

        return 0;
}
