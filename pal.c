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

#define BIT_VID  3
#define BIT_SYNC 2

#define cbi(x, y) x &= ~(1 << y)
#define sbi(x, y) x |= (1 << y)

char lines[305];

void IntInitS(void);

void LongSyncPulseS(void);
void LongSyncDelayS(void);
void ShortSyncPostPulseS(void);
void ShortSyncPostDelayS(void);
void HSyncPulseS(void);
void BackPorchS(void);
void ImageDataS(void);
void ShortSyncPrePulseS(void);
void ShortSyncPreDelayS(void);

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

        // Set the sync pin and video pin as outputs
        sbi(DDRB, BIT_VID);
	sbi(DDRB, BIT_SYNC);

	// Make sure they're defaulted
	cbi(PORTB, BIT_VID);
	sbi(PORTB, BIT_SYNC);

        // Configure timer 1 for Clear on Timer Compare (CTC)
        TCCR1B |= 1 << WGM12;

	// Set up the timers
	TCNT1 = 0;
        OCR1A = 1000;

        // Start timer without prescaler
        TCCR1B |= 1 << CS10 | 0 << CS11;

	// Enable compare interrupt
	TIMSK1 |= 1 << OCIE1A;

	// Initialize interrupt variables
	IntInitS();

	// Enable interrupts
	sei();

        while (1);

        return 0;
}
