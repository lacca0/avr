#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define BUFF_LENGTH 10
#include "uart_buffer.c"

#define F_CPU 1000000UL  // 1 MHz

uint16_t in_frequency = 0;
uint16_t out_frequency = 0;

uint8_t position = 0;

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

ISR(USART_UDRE_vect)
{
	static uint8_t next_string_flag = 1;
	if (position == 0)
	{
		if (!next_string_flag)
		{
			if (IN_EMPTY)
			{
				UCSRB &= ~(1 << UDRIE);
			}
			else
			{
				out_frequency = access_data();
				position = decim_digits_num(out_frequency);
				next_string_flag = 1;
			}
		}
		else if (next_string_flag == 1)
		{
			UDR = '\n';
			next_string_flag = 2;
		}
		else if (next_string_flag == 2)
		{
			UDR = '\r';
			next_string_flag = 0;
		}
	}
	else if (position > 0)
	{
		send_element(out_frequency, &position);
	}
}

ISR(USART_RXC_vect)
{
	uint8_t MYUDR = UDR;
	if ((MYUDR >= '0') && (MYUDR <= '9'))
	{
		in_frequency = (in_frequency * 10) + MYUDR - '1' + 1;
	}
	else if (MYUDR == '\r')
	{
		add_in_data(in_frequency);
		in_frequency = 0;
		UCSRB |= (1 << UDRIE);
	}
	else
	{
		in_frequency = 0;
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
	//enable RX Complete, USART Data Register Empty Interrupts:
	UCSRB |= (1 << RXCIE) /* (1 << TXCIE) */ /*| (1 << UDRIE)|*/| (0 << UCSZ2);
	//enable transmitter, receiver:
	UCSRB |= (1 << RXEN) | (1 << TXEN);
	//double speed:
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
