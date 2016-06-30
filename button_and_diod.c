#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

#define F_CPU 1000000UL  // 1 MHz

#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>


void setup_io()
{
	DDRA &= ~(1 << PA0);
	PORTA |= (1 << PA0);

}

bool rising_edge()
{
	static bool button_is_on = 0;
	if (((PINA & (1 << PA0)) == 0) && (!button_is_on))
		{
		button_is_on = 1;
		return 1;
		}
	else if ((button_is_on == 1) && (PINA & (1 << PA0) == 1))
		{
		button_is_on = 0;
		return 0;
		}
	else
		{
		return 0;
		}
}

int main(void)
{
	setup_io();
	DDRA |= (1 << PA1);
	TCCR0 = (1 << CS00) | (0 << CS01) | (1 << CS02) | (1 << WGM00) | (0 << WGM01) | (0 << COM00) | (0 << COM01);
	while (true)
	{
		if (rising_edge())
		{
			PORTA |= (1 << PA1);
			_delay_ms(1000);
			PORTA &= ~(1 << PA1);
		}


	}

}
