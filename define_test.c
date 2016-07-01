#include <stdbool.h>
#include <stdint.h>

#include <stdio.h>

#define F_CPU 1000000UL  // 1 MHz

#define multiply(a, b) a*b


int main()
{
	printf("%u \n %u \n", multiply(8, 3), 5/3);
}

