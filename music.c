#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define F_CPU 1000000UL  // 1 MHz

#include "lib/sound.c"

// PB3 is connected with buzzer, PD3 with button

void setup_io()
{
	//PD3 - input, PullUp
	DDRD &= ~(1 << PD3);
	PORTD |= (1 << PD3);
	//PB3 - output
	DDRB |= (1 << PB3);
	PORTB |= (1 << PB3);
	//timer 0 for sound: clear on compare mode, toggle int1 on compare, timer stopped:
	TCCR0 = (0 << CS02) | (0 << CS01) | (0 << CS00) | (0 << WGM00) | (1 << WGM01) | (1 << COM00) | (0 << COM01);
	//timer 1 for timing: clear on compare(WGM), output pins disconnected(COM), timer stopped(CS):
	TCCR1A = (0 << COM1A1) | (0 << COM1A0) | (0 << COM1B1) | (0 << COM1B0) | (0 << WGM11) | (0 << WGM10);
	TCCR1B = (0 << WGM13) | (1 << WGM12) | (0 << CS12) | (0 << CS11) | (0 << CS10);
	//enable interrupt on timer 1 compare.
	TIMSK |=  (1 << OCIE1A);

	//enable external interrupts, falling edge generates a request:
	MCUCR |= (1 << ISC11) | (0 << ISC10);
	GICR |= (1 << INT1);

	sei();

}

ISR(INT1_vect)
{
	//prescaler 1024:
	TCCR1B |= (1 << CS12) | (0 << CS11) | (1 << CS10);
	sound_change_note();
}

ISR(TIMER1_COMPA_vect)
{
	static bool counter = 0;

	if (counter == 0)
	{
		sound_change_note();
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

	sleep_enable();

	while (true)
	{
		sleep_cpu();
	}
}
