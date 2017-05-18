#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>

#define F_CPU 1000000UL  // 1 MHz
#include <util/delay.h>

#define RS 0
#define RW 1
#define E 2
#define ERR 3 //PC3 is error signal

#include "../lib/utilities.c"
#include "../lib/display.c"

#define FIELD_HEIGHT 8
#define FIELD_WIDTH 8
#define SCREEN_HEIGHT 2

uint8_t field[FIELD_HEIGHT][FIELD_WIDTH] = {
{1, 1, 1, 1, 1, 1, 1, 1},
{1, 0, 0, 0, 0, 1, 0, 0},
{1, 1, 1, 1, 0, 0, 0, 1},
{1, 0, 0, 0, 0, 1, 0, 1},
{1, 1, 1, 1, 0, 1, 0, 1},
{1, 0, 1, 0, 0, 1, 0, 1},
{0, 0, 0, 0, 1, 1, 0, 1},
{1, 1, 1, 1, 1, 1, 1, 1},
};

bool previous_step_was_up = 0;//обработать заход сверху

uint8_t my_location[2] = {6, 1};

void setup_io()
{
	DDRC |= (1 << RS) | (1 << RW) | (1 << E);//вывод
}

void fill_screen()
{
	display_send_command(CMD_CLEAR_DISPLAY);
	for (uint8_t i = 0; i < FIELD_WIDTH; i++)
	{
	//i - номер в строке
		if (field[my_location[0] - previous_step_was_up + 1 - 1][i] != 0)
		{
			display_send_character(DISPLAY_SQUARE);
		}
		else if ((my_location[0] == (my_location[0] - previous_step_was_up + 1 - 1)) && (my_location[1] == i))
		{
			display_send_character('I');
		}
		else
		{
			display_send_character(DISPLAY_EMPTY);
		}

	}
	display_send_command(CMD_SET_DDRAM_ADDRESS | DDRAM_2ND_LINE);
	for (uint8_t i = 0; i < FIELD_WIDTH; i++)
	{
		if (field[my_location[0] - previous_step_was_up + 1][i] != 0)
		{
			display_send_character(DISPLAY_SQUARE);
		}
		else if ((my_location[0] == my_location[0] - previous_step_was_up + 1) && (my_location[1] == i))
		{
			display_send_character('I');
		}
		else
		{
			display_send_character(DISPLAY_EMPTY);
		}

	}
}

int main()
{
	setup_io();
	display_init();
	display_send_command(CMD_DISPLAY_ON_OFF_CTRL | CMD_DOOC_DISPLAY_VISIBLE | CMD_DOOC_CURSOR_VISIBLE );
	fill_screen();
	while(true)
	{
		sleep_cpu();
	}

}
