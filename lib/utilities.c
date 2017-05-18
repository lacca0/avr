#pragma once

#define MS_TO_CLOCKS(ms, PRES) (((ms) * F_CPU) / (1000 * (PRES)))
#define FREQ_TO_CLOCKS(freq, PRES) (F_CPU / (PRES)) / (freq)

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

void turn_number_to_array(uint8_t* array, uint16_t number, uint8_t* digits_cnt, uint8_t array_length)
{
	*digits_cnt = 0;
	uint8_t significant_digits_cnt = 0;
	for(uint8_t counter = 1; counter < (array_length + 1); counter++)
	{
		significant_digits_cnt++;
		array[array_length - counter] = (number) % 10;
		if (number < 10) break;
		number = (number) / 10;

	}
	*digits_cnt = significant_digits_cnt;
}//*The number is converted to a right-aligned array of decimal digits.
//* Count of significant digits in that array is returned via "digits_cnt".

#ifdef ERR
void panic()
{
	DDRC |= (1 << ERR);
	cli();
	while (true)
	{
		PORTC |= (1 << ERR);
		_delay_ms(1000);
		PORTC &= ~(1 << ERR);
		_delay_ms(1000);
	}
}
#endif
