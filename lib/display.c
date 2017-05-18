#pragma once

#define LINE_LENGTH 16

#define CMD_CLEAR_DISPLAY 0b1
#define CMD_RETURN_HOME 0b10

#define CMD_ENTRY_MODE_SET 0b100
#define CMD_EMS_FORWARD 0b10
#define CMD_EMS_BACKWARD 0
#define CMD_EMS_SHIFT 0
#define CMD_EMS_NO_SHIFT 0b1

#define CMD_DISPLAY_ON_OFF_CTRL 0b1000
#define CMD_DOOC_DISPLAY_VISIBLE 0b100
#define CMD_DOOC_DISPLAY_NOT_VISIBLE 0
#define CMD_DOOC_CURSOR_VISIBLE 0b10
#define CMD_DOOC_CURSOR_NOT_VISIBLE 0
#define CMD_DOOC_BLINKING_CURSOR_ON 0b1




//PORTA is DB0..7
//PORTC for RS, RW, E

bool display_receive_busy_flag()
{
	DDRA = 0;//ввод
	PORTA = ~0;
	PORTC &= ~(1 << RS);
	PORTC |= (1 << RW);
	PORTC |= (1 << E);
	_delay_us(1);
	bool busy_flag = !!(PINA & (1 << 7));
	PORTC &= ~(1 << E);
	return busy_flag;
}

void display_start_sending_command()
{
	while (display_receive_busy_flag()) {};
	DDRA = ~0;//вывод
	PORTA = 0;
	PORTC &= ~((1 << RS) | (1 << RW));
	PORTC |= (1 << E);
}

void display_start_sending_command_init()
{
	DDRA = ~0;//вывод
	PORTA = 0;
	PORTC &= ~((1 << RS) | (1 << RW));
	PORTC |= (1 << E);
}

void display_end_sending()
{
	PORTC &= ~(1 << E);
	PORTA = 0;
}

void display_send_command(uint8_t command)
{
	display_start_sending_command();
	PORTA = command;
	display_end_sending();
}


void display_start_sending_data()
{
	while (display_receive_busy_flag()) {};
	DDRA = ~0;//вывод
	PORTA = 0;
	PORTC |= (1 << RS);
	PORTC &= ~(1 << RW);
	PORTC |= (1 << E);
}

void display_init()
{
	_delay_us(15);

	display_start_sending_command_init();
	PORTA = 0b00110000;
	display_end_sending();
	_delay_ms(5);

	display_start_sending_command_init();
	PORTA = 0b00110000;
	display_end_sending();
	_delay_us(100);

	display_start_sending_command_init();
	PORTA = 0b00110000;
	display_end_sending();
	_delay_ms(1);

	display_start_sending_command_init();
	PORTA = 0b00111000;//8-bit interface, 2 lines,5x8dots
	display_end_sending();
	_delay_ms(1);

	display_start_sending_command_init();
	PORTA = CMD_DISPLAY_ON_OFF_CTRL |  CMD_DOOC_DISPLAY_NOT_VISIBLE;//display off
	display_end_sending();
	_delay_ms(1);

	display_start_sending_command_init();
	PORTA = CMD_CLEAR_DISPLAY;
	display_end_sending();
	_delay_ms(1);

	display_start_sending_command_init();
	PORTA = 0b00000110;//increment adress counter, zero shift
	display_end_sending();
	_delay_ms(1);

}

uint8_t display_get_character_code(char ch)
{
	if ((ch >= 'A') && (ch <= 'Z'))
	{
		return (0b01000001 + ch - 'A');
	}
	else if ((ch >= '0') && (ch <= '9'))
	{
		return (0b00110000 + ch - '0');
	}
	else if ((ch >= 'a') && (ch <= 'z'))
	{
		return (0b01000001 + ch - 'A');
	}
	else if (ch == ' ')
	{
		return 0b00100000;
	}
	else if (ch == '_')
	{
		return 0b01011111;
	}
	else if (ch = '-')
	{
		return 0b00101101;
	}
	else
	{
		panic();
	}

}

void display_send_character(char ch)
{
	display_start_sending_data();//write data
	PORTA = display_get_character_code(ch);
	display_end_sending();
}

void display_send_string(const char *symbol_array)
{
	for (int i = 0; symbol_array[i] != 0; i++)
	{
		display_send_character(symbol_array[i]);
	}
}
