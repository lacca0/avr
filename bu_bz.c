#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

#define F_CPU 1000000UL  // 1 MHz

#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>


bool rising_edge()
{
	//PA0 is connected with button
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


ISR(TIMER1_OVF_vect)
{
	static uint8_t frequency_const[] = {8, 8, 8, 11, 6, 8, 11, 6, 8, 6, 6, 6, 5, 6, 9, 11, 6, 8};
	static uint8_t fq_len = 18;
	static uint8_t counter = 0;


	if(rising_edge())
	{
		OCR0 = frequency_const[counter % fq_len] - 5;
		counter++;
	}
}


void setup_io()
{
	//PA0 - input, PullUp
	DDRA &= ~(1 << PA0);
	PORTA |= (1 << PA0);
	//PB3 - output
	DDRB |= (1 << PB3);
	PORTB |= (1 << PB3);

}


int main(void)
{
	setup_io();

	TCCR0 = (1 << CS00) | (0 << CS01) | (1 << CS02) | (0 << WGM00) | (1 << WGM01) | (1 << COM00) | (0 << COM01);
	TCCR1A = (0 << COM1A1) | (0 << COM1A0) | (0 << COM1B1) | (0 << COM1B0) | (0 << WGM11) | (0 << WGM10);
	TCCR1B = (0 << WGM13) | (0 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10);
	TIMSK |=  (1 << TOIE1);

	sei();

	sleep_enable();
	while (true)
	{
		sleep_cpu();
	}



}
