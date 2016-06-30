#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

#define F_CPU 1000000UL  // 1 MHz

#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>

//actually falling edge

ISR(INT1_vect)
{
	PORTA |= (1 << PA0);
	_delay_ms(1000);
	PORTA &= ~(1 << PA0);
}


void setup_io()
{
	DDRA |= (1 << PA0);
	DDRD &= ~(1 << PD3);
	PORTD |= (1 << PD3);
}


int main(void)
{
	setup_io();


	GICR |= (1 << INT1);
	MCUCR |= (1 << ISC11) | (0 << ISC10);
	sei();

	sleep_enable();
	while (true)
	{
		sleep_cpu();
	}



}
