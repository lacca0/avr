#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>

#define F_CPU 1000000UL  // 1 MHz

#include <util/delay.h>

#include "lib/display.c"

void setup_timers()
{
	TCCR0 |= (1 << CS00) | (1 << CS02);//prescaler 1024
	TIMSK |= (1 << TOIE0);//timer0 overflow  interrupt enable;
	sei();//interrupts enable
}


void setup_io()
{
	DDRC |= (1 << RS) | (1 << RW) | (1 << E);//вывод
}




int main()
{

	/*static uint8_t cycles_counter = 0;
	setup_io();
	while(true)
	{
		sleep_cpu();
		if (cycles_counter == 3)
		{

		}
		else
		{
			cycles_counter++;
		}
	}*/
	setup_io();
	//setup_timers;
	display_init();
	display_send_command(CMD_DISPLAY_ON_OFF_CTRL | CMD_DOOC_DISPLAY_VISIBLE | CMD_DOOC_CURSOR_VISIBLE );

	/*start_sending_command();
	PORTA = 0b10000000;//set DDRAM adress 00
	display_end_sending();*/

	/*start_sending_command();
	PORTA = 0b00010100;//cursor shift;
	display_end_sending();*/

	char symbol_array[LINE_LENGTH + 1] = "hello Lena";
	char *array_ptr = symbol_array;


	for (int i = 0; symbol_array[i] != 0; i++)
	{
		display_send_character(symbol_array[i]);
	}

	while(true)
	{
		sleep_cpu();
	}

}
