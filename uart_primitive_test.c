#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define F_CPU 1000000UL  // 1 MHz
#define BUFF_LENGTH 100


char IN_buff;

void add_in_data()
{
	IN_buff = UDR;
}


void send_out_data()
{
	UDR = IN_buff;
}


ISR(USART_RXC_vect)
{
	add_in_data();
	send_out_data();
}

void setup_io()
{
	//disable interrupts:
	cli();
	//set the baud rate:
	UBRRL |= 6; // for 9600 bps baud rate
	//set asynchronous mode, disable parity mode, 1 stop bit, 8-bit character:
	UCSRC |= (1 << URSEL) | (0 << UMSEL) | (0 << UPM1) | (0 << UPM0) | (0 << USBS) | (0 << UCSZ2) | (1 << UCSZ1) | (1 << UCSZ0) | (0 << UCPOL);
	//enable RX Complete, no yet USART Data Register Empty Interrupts:
	UCSRB |= (1 << RXCIE) |  (0 << UDRIE);
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
