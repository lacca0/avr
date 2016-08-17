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
	if (IN_buff >= '0' && IN_buff <= '9') {
		IN_buff = 'a' - '0' + IN_buff;
	}
	UDR = IN_buff + 1;
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
	UBRRH = 0;
	UBRRL = 12; // for 4800 bps baud rate
	//set asynchronous mode, disable parity mode, 1 stop bit, 8-bit character:
	UCSRC |= (1 << URSEL) | (0 << UMSEL) | (0 << UPM1) | (0 << UPM0) | (0 << USBS) | (1 << UCSZ1) | (1 << UCSZ0) | (0 << UCPOL);
	//enable RX Complete, no yet USART Data Register Empty Interrupts:
	UCSRB |= (1 << RXCIE) |  (0 << UDRIE) | (0 << UCSZ2);
	//enable transmitter, receiver
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
