#include <stdio.h>
#include <stdint.h>


int main()
{
	int divisor;
	int number;
	int number_len;

	scanf("%u %u", divisor, number);

	for (int i = number_len + 1; i > 0; i--)
	{
		divisor = divisor * 10;
	}
	float output;
	output = (number - (number % divisor)) / divisor % 10;

	printf("%f", output)
;}
