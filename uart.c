uint16_t uart_outcoming_number = 0;

void uart_send_next_digit(uint16_t number, uint8_t* pos)
{
	uint16_t divisor = 1;
	//make 10^(*pos - 1):
	for (uint8_t i = *pos - 1; i > 0; i--)
	{
		divisor *= 10;
	}
	UDR = '0' + (number / divisor) % 10;
	(*pos)--;
}

void uart_send_number_iter()
{
	static uint8_t position = 0;
	static uint8_t state = 1;
	if (state == 1)
	{
		position = decim_digits_num(uart_outcoming_number);
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

