#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

#define F_CPU 1000000UL  // 1 MHz

#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>
#define PRES2 1024UL
#define MY_OCR2 MS_TO_CLOCKS(3, PRES2) - 1 // the real (actual) period between comparator trigger events is OCRn + 1, so subtract one

#define SELLS_NUMBER 3
#include "utilities.c"
#include "sound.c"

uint8_t display_current_number[SELLS_NUMBER] = {9, 9, 9}; //upside-down.

#include "7seg_display.c"

//PB3 is connected to buzzer.
//PORTB 0,1,2 pins are connected to display, all PORTA for segments. (if changed change "7seg_display.c")

ISR(TIMER1_COMPA_vect)
{
	sound_turn_off();
}

uint8_t display_zero_flag = 0;

void display_value_decrement()
{
	if (!display_current_number[0])
	{
		if(!display_current_number[1])
		{
			if(!display_current_number[2])
			{
				display_zero_flag = 1;
			}
			else
			{
				display_current_number[2]--;
				display_current_number[1] = display_current_number[0] = 9;

			}
		}
		else
		{
			display_current_number[1]--;
			display_current_number[0] = 9;
		}
	}
	else
	{
		display_current_number[0]--;
	}
}

ISR(TIMER2_COMP_vect)
{
	static uint16_t cycles_counter = 0;
	display_show_next_digit();
	cycles_counter++;
	if(cycles_counter == MS_TO_CLOCKS(1000, PRES2 * MY_OCR2))
	{
		cycles_counter = 0;
		display_value_decrement();
		if (display_zero_flag)
		{
			//stop timer2:
			TCCR2 &= ~(1 << CS22) | (1 << CS21) | (1 << CS20);
			TIMSK &= ~(1 << OCIE2);
			//buzzer!
			sound_set_frequency(300);
			sound_turn_on();
		}
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
