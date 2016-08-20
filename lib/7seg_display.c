#pragma once

uint8_t display_number_code[] =
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

void display_show_next_digit()
{
	static uint8_t counter = 0;
	PORTB &= ~(111);// PORTB 0, 1, 2 pins are connected to display
	PORTB |= (1 << counter);
	PORTA = display_number_code[display_current_number[CELLS_NUMBER - counter - 1]]; //PORTA for segments
	counter++;
	if (counter == CELLS_NUMBER) counter = 0;
}

uint8_t display_zero_flag = 0;

void display_value_decrement()
{
	if (!display_current_number[2])
	{
		if(!display_current_number[1])
		{
			if(!display_current_number[0])
			{
				display_zero_flag = 1;
			}
			else
			{
				display_current_number[0]--;
				display_current_number[1] = display_current_number[2] = 9;

			}
		}
		else
		{
			display_current_number[1]--;
			display_current_number[2] = 9;
		}
	}
	else
	{
		display_current_number[2]--;
	}
}
