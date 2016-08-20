#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define F_CPU 1000000UL  // 1 MHz

#include "lib/sound.c"

// PB3 is connected with buzzer, PD3 with button
uint8_t array_len = 72;
uint8_t frequency_array[] = {
	TO_OCR0(98),  TO_OCR0(98),  TO_OCR0(98),  TO_OCR0(78),  TO_OCR0(117),
	TO_OCR0(98),  TO_OCR0(78),  TO_OCR0(117), TO_OCR0(98),  0,
	TO_OCR0(147), TO_OCR0(147), TO_OCR0(147), TO_OCR0(156), TO_OCR0(117),
	TO_OCR0(93),  TO_OCR0(78),  TO_OCR0(117), TO_OCR0(98),
	TO_OCR0(196), TO_OCR0(98),  TO_OCR0(98),  TO_OCR0(196), TO_OCR0(185), TO_OCR0(175),
	TO_OCR0(165), TO_OCR0(156), TO_OCR0(165), 0,            TO_OCR0(104), TO_OCR0(139), TO_OCR0(131), TO_OCR0(123),
	TO_OCR0(117), TO_OCR0(110), TO_OCR0(117), 0,            TO_OCR0(78),  TO_OCR0(93),  TO_OCR0(78),  TO_OCR0(93),
	TO_OCR0(117), TO_OCR0(98),  TO_OCR0(117), TO_OCR0(147),
	TO_OCR0(196), TO_OCR0(98),  TO_OCR0(98),  TO_OCR0(196), TO_OCR0(139), TO_OCR0(175),
	TO_OCR0(165), TO_OCR0(78),  TO_OCR0(165), 0,            TO_OCR0(104), TO_OCR0(139), TO_OCR0(131), TO_OCR0(123),
	TO_OCR0(117), TO_OCR0(110), TO_OCR0(117), 0,            TO_OCR0(78),  TO_OCR0(93),  TO_OCR0(78),  TO_OCR0(117),
	TO_OCR0(98),  TO_OCR0(78),  TO_OCR0(117), TO_OCR0(98),  0,
};

uint16_t timing_array[] = {
	TO_OCR1(4), TO_OCR1(4), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(4),  TO_OCR1(4),
	TO_OCR1(4), TO_OCR1(4), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(16),
	TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(1), TO_OCR1(1), TO_OCR1(2),  TO_OCR1(2),
	TO_OCR1(2), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(1), TO_OCR1(1), TO_OCR1(2), TO_OCR1(2),
	TO_OCR1(2), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(8),
	TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(1), TO_OCR1(1), TO_OCR1(2),  TO_OCR1(2),
	TO_OCR1(2), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(1), TO_OCR1(1), TO_OCR1(2), TO_OCR1(2),
	TO_OCR1(2), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(8), TO_OCR1(32),
};


void setup_io()
{
	//PD3 - input, PullUp
	DDRD &= ~(1 << PD3);
	PORTD |= (1 << PD3);
	//PB3 - output
	DDRB |= (1 << PB3);
	PORTB |= (1 << PB3);
}

void change_note()
{

	static uint8_t counter = 0;

	if (frequency_array[counter] == 0)
	{
		sound_mute();
	}
	else
	{
		//change frequency:
		OCR0 = frequency_array[counter];
		sound_unmute();
	}

	//change current duration of note:
	OCR1A = timing_array[counter];

	counter++;
	if (counter == array_len)
	{
		counter = 0;
	}
}

ISR(INT1_vect)
{
	//prescaler 1024:
	TCCR1B |= (1 << CS12) | (0 << CS11) | (1 << CS10);
	change_note();
}

ISR(TIMER1_COMPA_vect)
{
	static bool counter = 0;

	if (counter == 0)
	{
		change_note();
		counter = 1;
	}
	else
	{
		sound_pause();
		counter = 0;
	}
}

int main(void)
{
	setup_io();

	//timer 0 for sound: clear on compare mode, toggle int1 on compare, timer stopped:
	TCCR0 = (0 << CS02) | (0 << CS01) | (0 << CS00) | (0 << WGM00) | (1 << WGM01) | (1 << COM00) | (0 << COM01);
	//timer 1 for timing_array: clear on compare(WGM), output pins disconnected(COM), timer stopped(CS):
	TCCR1A = (0 << COM1A1) | (0 << COM1A0) | (0 << COM1B1) | (0 << COM1B0) | (0 << WGM11) | (0 << WGM10);
	TCCR1B = (0 << WGM13) | (1 << WGM12) | (0 << CS12) | (0 << CS11) | (0 << CS10);
	//enable interrupt on timer 1 compare.
	TIMSK |=  (1 << OCIE1A);

	//enable external interrupts, falling edge generates a request:
	MCUCR |= (1 << ISC11) | (0 << ISC10);
	GICR |= (1 << INT1);

	sei();

	sleep_enable();

	while (true)
	{
		sleep_cpu();
	}
}
