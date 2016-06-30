#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

#define F_CPU 1000000UL  // 1 MHz

#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>



ISR(INT0_vect)
{
	_delay_ms(2000);
	PORTA ^= (1 << PA0);

//	PORTA &= ~(1 << PA0);

}

void setup_io()
{
	DDRA |= (1 << PA0);
}


int main(void)
{

	DDRD &= ~(1 << PD2);
	PORTD |= (1 << PD2);
	//разрешаем внешнее прерывание:
	GICR |= (1 << INT0);
	//настраиваем прерывание по falling edge:
	MCUCR |= (1 << ISC01) | (1 << ISC00);
	setup_io();
	//разрешаем прерывания:
	sei();
	sleep_enable();
	while (true)
	{
		sleep_cpu();
	}

	/*TCCR0 = (1 << CS00) | (0 << CS01) | (1 << CS02) | (0 << WGM00) | (0 << WGM01) | (0 << COM00) | (0 << COM01);
	TIMSK |= 1 << TOIE0;
	sei();

	sleep_enable();
	while (true) */

}
