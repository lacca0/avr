#pragma once
#define UART_ARRAY_LEN 6

uint8_t uart_outcoming_number[UART_ARRAY_LEN];
uint8_t uart_outcoming_digits_cnt; // количество значимых цифр в массиве

void uart_send_next_digit(uint8_t* digits, uint8_t* count)
{
	UDR = *(digits + UART_ARRAY_LEN - *count) + '0';
	(*count)--;
	//с нулевого элемента до энного.
}

void uart_send_number_iter(uint8_t starting_position)
{
	static uint8_t position = 0;
	static uint8_t state = 1;
	if (state == 1)
	{
		position = starting_position;
		state = 2;
	}
	else if (state == 2)
	{
		if (position > 0)
		{
			uart_send_next_digit(uart_outcoming_number, &position);
		}
		else
		{
			UDR = '\n';
			state = 3;
		}
	}
	else if (state == 3)
	{
		UDR = '\r';
		state = 1;
		UCSRB &= ~(1 << UDRIE);
	}
}

