#include <stdio.h>
#define TO_OCR0(freq) (1000000 / 64) / freq //64 = prescaler 0

int main()
{
	printf("%u\n", TO_OCR0(78));
}
