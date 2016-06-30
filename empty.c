#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

#define F_CPU 1000000UL  // 1 MHz

#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>


//#define SET(port, pin, value) do { if (value) { port |= (1 << pin); } else { port &= ~(1 << pin); } } while (0)

void setup_io(void)
{
	// set port A pins 0-7 as output for LED panel
	DDRA = ~0;
	DDRB = 1 << 7;

}

ISR(TIMER0_OVF_vect)
{
	static int two_seconds_counter = 0;
	if (two_seconds_counter == 5)
	{
		PORTA |= 1;
	}
	if (two_seconds_counter == 6)
	{
		PORTA = 0;
		two_seconds_counter = -1;
	}
	two_seconds_counter += 1;
}


int main (void)
{
	setup_io();


		/*for(int i = 0; i < 8; i++) {
			PORTA |= 1 << i;
			_delay_ms(300);
			PORTA &= ~(1 << i);
		}
		PORTA = 0;
		_delay_ms(500);
		*/
		/*while (true) {
		PORTD = 1 << 7;
		_delay_ms(10);
		PORTD = 0;
		_delay_ms(10);
		*/

	TCCR0 = (1 << CS00) | (0 << CS01) | (1 << CS02) | (0 << WGM00) | (0 << WGM01) | (0 << COM00) | (0 << COM01);
	TIMSK |= 1 << TOIE0;
	sei();

	sleep_enable();
	while (true)
	{
		sleep_cpu();
	}



	return 0;
}
