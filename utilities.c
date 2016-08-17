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
