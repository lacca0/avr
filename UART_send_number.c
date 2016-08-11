void send_element(uint16_t number, uint8_t* pos)
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

uint8_t decim_digits_num(uint16_t number)
{
	uint8_t i = 1;
	while ((number / 10) > 0)
	{
		number = number / 10;
		i++;
	}
	return i;
}

void send_number_iter()
{
	static uint8_t position = 0;
	static uint8_t state = 1;
	if (state == 1)
	{
		position = decim_digits_num(output_in_progress);
		state = 2;
	}
	else if (state == 2)
	{
		if (position > 0)
		{
			send_element(output_in_progress, &position);
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

