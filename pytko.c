#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

#define F_CPU 1000000UL  // 1 MHz

#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>

void setup_io()
{
	DDRB |= (1 << PB3);
}

int main(void)
{
	setup_io();

	// 1MHz/8 = 128k, CTC, toggle OC0
	TCCR0 = (0 << CS00) | (1 << CS01) | (0 << CS02) |
	        (0 << WGM00) | (1 << WGM01) |
	        (1 << COM00) | (0 << COM01);

	// 128k/4 = 32k
	OCR0 = 3;

	sei();

	sleep_enable();
	while (true)
	{
		sleep_cpu();
	}



}
