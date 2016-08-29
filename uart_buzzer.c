#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define F_CPU 1000000UL  // 1 MHz
#define BUFF_LENGTH 10

#include "lib/utilities.c"
#include "lib/buffer.c"
#include "lib/sound.c"
#include "lib/uart_alt.c"

//PB3 is connected to buzzer.

ISR(TIMER1_COMPA_vect)
{
	if(IN_EMPTY)
	{
		sound_turn_off();
	}
	else
	{
		uint16_t temp = buffer_access_data();
		sound_set(temp, 2000);
		turn_number_to_array(uart_outcoming_number, temp, &uart_outcoming_digits_cnt, UART_ARRAY_LEN);
		UCSRB |= (1 << UDRIE);

	}
}

ISR(USART_RXC_vect)
{
	uint8_t MYUDR = UDR;
	static uint16_t in_frequency = 0;

	if ((MYUDR >= '0') && (MYUDR <= '9'))
	{
		in_frequency = (in_frequency * 10) + MYUDR - '1' + 1;
	}
	else if (MYUDR == '\r')
	{
		buffer_add_in_data(in_frequency);
		in_frequency = 0;
		if (!sound_enabled)
		{
			uint16_t temp = buffer_access_data();
			sound_set(temp, 2000);
			turn_number_to_array(uart_outcoming_number, temp, &uart_outcoming_digits_cnt, UART_ARRAY_LEN);
			UCSRB |= (1 << UDRIE);
			sound_turn_on();
		}
	}
	else
	{
		in_frequency = 0;
	}
}

ISR(USART_UDRE_vect)
{
	uart_send_number_iter();
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

	//PB3 - output
	DDRB |= (1 << PB3);
	PORTB |= (1 << PB3);
	//timer 0 for sound: clear on compare mode, toggle int1 on compare, timer stopped:
	TCCR0 = (0 << CS02) | (0 << CS01) | (0 << CS00) | (0 << WGM00) | (1 << WGM01) | (1 << COM00) | (0 << COM01);
	//timer 1 for timing: clear on compare(WGM), output pins disconnected(COM), timer stopped(CS):
	TCCR1A = (0 << COM1A1) | (0 << COM1A0) | (0 << COM1B1) | (0 << COM1B0) | (0 << WGM11) | (0 << WGM10);
	TCCR1B = (0 << WGM13) | (1 << WGM12) | (0 << CS12) | (0 << CS11) | (0 << CS10);
	//enable interrupt on timer 1 compare.
	TIMSK |=  (1 << OCIE1A);

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
