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

void turn_number_to_array(uint8_t* array, uint16_t* number, uint8_t* digits_cnt, uint8_t array_length)
{
	*digits_cnt = 0;
	uint8_t null_cnt = 0;
	for(uint8_t counter = 1; counter < (array_length + 1); counter++)
	{
		if (*number == 0)
		{
			null_cnt++;
		}
		array[array_length - counter] = (*number) % 10;
		*number = (*number) / 10;

	}
	*digits_cnt = array_length - null_cnt;
}//creates array like 001234 from uint16_t number = 1234, counts number of valuable digits.
