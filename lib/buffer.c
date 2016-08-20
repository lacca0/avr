#pragma once

uint16_t IN_buff[BUFF_LENGTH] = {};
int IN_PTR_S = 0;
int IN_PTR_E = 0;
bool IN_FULL = false;
bool IN_EMPTY = true;

void buffer_add_in_data(uint16_t data)
{
	if (IN_FULL)
	{
		return;
	}

	IN_buff[IN_PTR_E] = data;
	IN_PTR_E++;
	if (IN_PTR_E == BUFF_LENGTH)
	{
		IN_PTR_E = 0;
	}

	if (IN_PTR_E == IN_PTR_S)
	{
		IN_FULL = true;
	}

	IN_EMPTY = false;
}

uint16_t buffer_access_data()
{
	uint16_t number;
	if (IN_EMPTY)
	{
		return 0;
	}

	number = IN_buff[IN_PTR_S];
	IN_PTR_S++;
	if(IN_PTR_S == BUFF_LENGTH)
	{
		IN_PTR_S = 0;
	}

	if (IN_PTR_S == IN_PTR_E)
	{
		IN_EMPTY = true;
	}

	IN_FULL = false;
	return number;
}
