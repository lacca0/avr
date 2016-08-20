#pragma once
//timer 0 for timing, timer 1 for sound
#include "utilities.c"
#define PRES0 64UL
#define PRES1 1024UL

#define TO_OCR0(freq) FREQ_TO_CLOCKS(freq, PRES0) / 2 // 2 is magic number for frequency change
#define MS_TO_OCR1(ms) MS_TO_CLOCKS(ms, PRES1)

#define TO_OCR1(n) MS_TO_OCR1(n * ONE_MUSICAL_SXTNTH_LENGTH) // n is the number of musical sxtnths in note

#define ONE_MUSICAL_SXTNTH_LENGTH 143 // in ms

bool sound_enabled = 0;

void sound_set(uint16_t freq, uint16_t length_in_ms)
{
	OCR0 = TO_OCR0(freq);
	OCR1A = MS_TO_OCR1(length_in_ms);
}

void sound_turn_on()
{
	sound_enabled = 1;
	//run timer1 with prescaler 1024:
	TCCR1B |= (1 << CS12) | (0 << CS11) | (1 << CS10);
	//run timer0 with prescaler 64:
	TCCR0 |= (0 << CS02) | (1 << CS01) | (1 << CS00);
}

void sound_turn_off()
{
	TCCR0 &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
	TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
	sound_enabled = 0;
}

void sound_mute()
{
	TCCR0 &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
}

void sound_unmute()
{
	sound_mute();
	//turn on the sound with prescaler 64:
	TCCR0 |= (0 << CS02) | (1 << CS01) | (1 << CS00);
}

void sound_pause()
{
	sound_mute();
	//change current duration of sound_pause:
	OCR1A = ONE_MUSICAL_SXTNTH_LENGTH / 2;
}

