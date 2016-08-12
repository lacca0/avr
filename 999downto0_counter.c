#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#define PRES0 64UL
#define PRES1 1024UL
#define F_CPU 1000000UL  // 1 MHz

#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>
#define PRES2 1024
#define MY_OCR2 7
#define VALUE_FOR_1S 140

#define SELLS_NUMBER 3
#include "music.c"

//PB3 is connected to buzzer.

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

uint8_t current_number[SELLS_NUMBER] = {3, 0, 0}; //upside-down.

void next_digit()
{
	static uint8_t counter = 0;
	PORTB &= ~(111);// PORTB 0, 1, 2 pins are connected to display
	PORTB |= (1 << counter);
	PORTA = number_code[current_number[counter]]; //PORTA for segments
	counter++;
	if (counter == SELLS_NUMBER) counter = 0;

}

ISR(TIMER1_COMPA_vect)
{
	turn_off_sound();
}

void change_number()
{
//	PORTD ^= (1 << 7);
	if (!current_number[0])
	{
		if(!current_number[1])
		{
			if(!current_number[2])
			{
				//stop timer2:
				TCCR2 &= ~(1 << CS22) | (1 << CS21) | (1 << CS20);
				TIMSK &= ~(1 << OCIE2);
				//buzzer!
				set_frequency(300);
				turn_on_sound();
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

	//enable interrupt on timer 2 compare.
	TIMSK |=  (1 << OCIE2);

	//enable interrupts globally:
	sei();

	DDRD |= (1 << 7);

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
