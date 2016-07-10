#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define F_CPU 1000000UL  // 1 MHz

#define BUFF_LENGTH 100
//input buffer:
//while testing use char, after that use 16 bit:
char IN_buff[BUFF_LENGTH] = {};
int IN_PTR_S = 0;
int IN_PTR_E = 0;
bool IN_FULL = false;
bool IN_EMPTY = true;


void add_in_data()
{
	if ((IN_PTR_E + 1) != IN_PTR_S)
	{
		if ((IN_PTR_E + 1) < BUFF_LENGTH)
		{
			IN_buff[IN_PTR_E + 1] = UDR;
			IN_PTR_E ++;
		}
		else
		{
			IN_buff[0] = UDR;
			IN_PTR_E = 0;
		}

	}
	else
	{
		IN_FULL = true;

	}
	IN_EMPTY = false;
}


void send_out_data()
{
	UDR = IN_buff[IN_PTR_S];
	IN_PTR_S++;
	if((IN_PTR_S) == BUFF_LENGTH)
	{
		IN_PTR_S = 0;
	}
	if (IN_PTR_S = IN_PTR_E)
	{
		IN_EMPTY = true;
	}
}


ISR(USART_RXC_vect)
{
	add_in_data();
	/*UCSRB |= (1 << UDRIE);*/
	send_out_data();
}
/*
ISR(USART_UDRE_vect)
{
	if (!IN_EMPTY)
	{
		send_out_data();
	}
	UCSRB &= ~(1 << UDRIE);
}
*/




void setup_io()
{
	//disable interrupts:
	cli();
	//set the baud rate:
	UBRRL |= 6; // for 9600 bps baud rate
	//set asynchronous mode, disable parity mode, 1 stop bit, 8-bit character:
	UCSRC |= (1 << URSEL) | (0 << UMSEL) | (0 << UPM1) | (0 << UPM0) | (0 << USBS) | (0 << UCSZ2) | (1 << UCSZ1) | (1 << UCSZ0) | (0 << UCPOL);
	//enable RX Complete, TX Complete, USART Data Register Empty Interrupts:
	UCSRB |= (1 << RXCIE) /*| (1 << TXCIE) | (1 << UDRIE)*/;
	//enable transmitter, receiver:
	UCSRB |= (1 << RXEN) | (1 << TXEN);
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
