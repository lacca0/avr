#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

#define F_CPU 1000000UL  // 1 MHz

#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>
#define PRES2 1024
#define MY_OCR2 7
#define VALUE_FOR_1S 140

#define SELLS_NUMBER 3

uint8_t number_code[] =
{
	0xeb, //0
	0x28, //1
	0xb3, //2
	0xba, //3
	0x78, //4
	0xda, //5
	0xdb, //6
	0xa8, //7
	0xfb, //8
	0xfa  //9
};

uint8_t current_number[SELLS_NUMBER] = {7, 0, 0}; //upside-down.

void next_digit()
{
	static uint8_t counter = 0;
	PORTB &= ~(111);// PORTB 0, 1, 2 pins are connected to display
	PORTB |= (1 << counter);
	PORTA = number_code[current_number[counter]];
	counter++;
	if (counter == SELLS_NUMBER) counter = 0;

}

void change_number()
{
	if (!current_number[0])
	{
		if(!current_number[1])
		{
			if(!current_number[2])
			{
				//stop timer:
				TCCR2 &= (~(1 << CS22) | (1 << CS21) | (1 << CS20));
				//something else.
			}
			else
			{
				current_number[2]--;
				current_number[1] = current_number[0] = 9;

			}
		}
		else
		{
			current_number[1]--;
			current_number[0] = 9;
		}
	}
	else
	{
		current_number[0]--;
	}
}

ISR(TIMER2_COMP_vect)
{
	static uint8_t cycles_counter = 0;
	next_digit();
	cycles_counter++;
	if(cycles_counter == VALUE_FOR_1S)
	{
		cycles_counter = 0;
		change_number();
	}
}


void setup_io()
{
	//disable interrupts:
	cli();
	//timer 2 for indicator update: clear on compare mode(WGM), output pins disconnected(COM), prescaler 1024(CS):
	TCCR2 |= (1 << WGM21) | (0 << WGM20) | (0 << COM21) | (0 << COM20) | (1 << CS22) | (1 << CS21) | (1 << CS20);
	OCR2 = MY_OCR2;

	DDRA &= ~0;
	DDRB |= 111;

	//enable interrupt on timer 2 compare.
	TIMSK |=  (1 << OCIE2);

	//enable interrupts globally:
	sei();

}

int main()
{
	setup_io();
	sleep_enable();

	while (true)
	{
		sleep_cpu();
	}
}
