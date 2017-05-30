#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>

#define F_CPU 1000000UL  // 1 MHz
#include <util/delay.h>

#include <avr/cpufunc.h>

#define RS 0
#define RW 1
#define E 2
#define ERR 3 //PC3 is error signal

#include "../lib/utilities.c"
#include "../lib/display.c"

#define FIELD_HEIGHT 8
#define FIELD_WIDTH 8
#define SCREEN_HEIGHT 2

#define PRES 1024L
#define MY_OCR0 MS_TO_CLOCKS(50, PRES) - 1// the real (actual) period between comparator trigger events is OCRn + 1, so subtract one;


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

bool previous_step_was_up = 0;

uint8_t my_location[2] = {6, 2};
uint8_t key_location[2] = {1, 1};
uint8_t door_location[2] = {1, 2};

bool key_taken = 0;
bool door_opened = 1;

bool pressed_row_and_col(uint8_t row, uint8_t col);
void move_YX(int8_t Y, int8_t X);


void setup_io()
{
	DDRC |= (1 << RS) | (1 << RW) | (1 << E);//вывод
	DDRD = 0b111000;
	PORTD = 0b000111;
}

void setup_timers()
{
	TCCR0 = (1 << WGM01) | (0 << WGM00) | (1 << CS02) | (0 << CS01) | (1 << CS00);//prescaler 1024, CTC mode
	OCR0 = MY_OCR0;
	TIMSK |= (1 << OCIE0);//allow compare match interrupt
}

ISR(TIMER0_COMP_vect)
{
	if (pressed_row_and_col(3, 2))
		{
			move_YX(1 , 0);
		}
	if (pressed_row_and_col(2, 1))
		{
			move_YX(0 , -1);
		}
	if (pressed_row_and_col(2, 3))
		{
			move_YX(0 , 1);
		}
	if (pressed_row_and_col(1, 2))
		{
			move_YX(-1, 0);
		}


}

void fill_row_by_number(uint8_t row)
{
	for (uint8_t i = 0; i < FIELD_WIDTH; i++)
	{
		if (field[row][i] != 0)
		{
			display_send_character(DISPLAY_SQUARE);
		}
		else if ((my_location[0] == row) && (my_location[1] == i))
		{
			display_send_character('I');
		}
		else if (!key_taken && (key_location[0] == row) && (key_location[1] == i))
		{
			display_send_character('P');
		}
		else if ((door_location[0] == row) && (door_location[1] == i))
		{
			if (!door_opened)
			{
				display_send_character('D');
			}
			else
			{
				display_send_character('d');
			}
		}
		else
		{
			display_send_character(DISPLAY_EMPTY);
		}
	}
}

void fill_screen()
{
	display_send_command(CMD_CLEAR_DISPLAY);
	if (previous_step_was_up && (my_location[0] > 0))
	{
		fill_row_by_number(my_location[0] - 1);
	}
	else
	{
		fill_row_by_number(my_location[0]);
	}
	display_send_command(CMD_SET_DDRAM_ADDRESS | DDRAM_2ND_LINE);
	if (!previous_step_was_up && (my_location[0] < FIELD_HEIGHT - 1))
	{
		fill_row_by_number(my_location[0] + 1);
	}
	else
	{
		fill_row_by_number(my_location[0]);
	}
}

void move_YX(int8_t Y, int8_t X)
{
	bool can_move = 1;

	if (field[my_location[0] + Y][my_location[1] + X] == 1) can_move = 0;
	if ((door_location[0] == my_location[0] + Y) &&
	    (door_location[1] == my_location[1] + X) &&
            !door_opened)
        {
        	can_move = 0;
        }
        if (((my_location[0] + Y) > (FIELD_HEIGHT - 1)) ||
            ((my_location[0] + Y) < 0) ||
            ((my_location[1] + X) > (FIELD_WIDTH - 1)) ||
            ((my_location[1] + X) < 0))
        {
		can_move = 0;
        }

	if (Y > 0) previous_step_was_up = 0;
        else if (Y < 0) previous_step_was_up = 1;

	if (can_move)
        {
        	my_location[1] += X;
        	my_location[0] += Y;
        }

        fill_screen();
}

#define FIRST_ROW_PIN_NUM 0
#define FIRST_COL_PIN_NUM 3
#define MAX_ROW 3
#define MAX_COL 3

bool pressed_row_and_col(uint8_t row, uint8_t col)
{
	static bool button_is_pressed[MAX_ROW][MAX_COL] = {};
	row--;
	col--;
	PORTD = 0b111111;
	PORTD &= ~(1 << FIRST_COL_PIN_NUM + col);
	_NOP();
	if (~PIND & (1 << FIRST_ROW_PIN_NUM + row))
	{
		if (!button_is_pressed[row][col])
		{
			button_is_pressed[row][col] = 1;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		button_is_pressed[row][col] = 0;
		return 0;
	}
}

int main()
{
	setup_io();
	display_init();
	display_send_command(CMD_DISPLAY_ON_OFF_CTRL | CMD_DOOC_DISPLAY_VISIBLE | CMD_DOOC_CURSOR_VISIBLE );
	fill_screen();
	setup_timers();
	sei();
	sleep_enable();
	while(true)
	{
		sleep_cpu();
	}

}
