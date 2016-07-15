#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define F_CPU 1000000UL  // 1 MHz

#define BUFF_LENGTH 100

char IN_buff[BUFF_LENGTH] = {};
int IN_PTR_S = 0;
int IN_PTR_E = 0;
bool IN_FULL = false;
bool IN_EMPTY = true;

int position = 0;

void add_in_data(uint8_t data)
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

uint8_t access_data()
{
	uint8_t number;
	if (IN_EMPTY)
	{
		return;
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


ISR(USART_RXC_vect)
{
	uint8_t MYUDR = UDR;
	if (('1' <= MYUDR) && (MYUDR <= '9'))
	{
		if (position == 0)
		{
			position = MYUDR - '1' + 1;
			UDR = 'a' + position - 1;
			position--;
		}
		else
		{
			add_in_data(MYUDR);
		}
	}
}

ISR(USART_UDRE_vect)
{
	if ((position == 0) && (!IN_EMPTY))
	{
		position = access_data();
	}
	if (position > 0)
	{
		UDR = 'a' + position - 1;
		position--;
	}
}

void setup_io()
{
	//disable interrupts:
	cli();
	//set the baud rate:
	UBRRH = 0;
	UBRRL = 12; // for 4800 bps baud rate
	//set asynchronous mode, disable parity mode, 1 stop bit, 8-bit character:
	UCSRC |= (1 << URSEL) | (0 << UMSEL) | (0 << UPM1) | (0 << UPM0) | (0 << USBS) | (1 << UCSZ1) | (1 << UCSZ0) | (0 << UCPOL);
	//enable RX Complete, TX Complete, USART Data Register Empty Interrupts:
	UCSRA &= ~(1 << UDRE);
	UCSRB |= (1 << RXCIE) /* (1 << TXCIE) */ | (1 << UDRIE)| (0 << UCSZ2);
	//enable transmitter, receiver:
	UCSRB |= (1 << RXEN) | (1 << TXEN);

	UCSRA |= (1 << U2X);
	//enable interrupts globally:
	sei();
}



int main()
{
	setup_io();
	sleep_enable();

	while (true)
	{
		sleep_cpu();
	}

}
