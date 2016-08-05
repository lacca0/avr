#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define BUFF_LENGTH 10
#include "uart_buffer.c"

#define PRES0 64UL
#define PRES1 1024UL

#define F_CPU 1000000UL  // 1 MHz

#define TO_OCR0(freq) (F_CPU / PRES0) / freq / 2 // 2 is magic number for frequency change
#define MS_TO_OCR1(ms) (ms * F_CPU) / (1000 * PRES1)

//PB3 is connected to buzzer.

uint16_t in_frequency = 0;
uint16_t out_frequency = 0;

bool smth_is_playing = 0;

void get_next_frequency()
{
		out_frequency = access_data();
		OCR0 = TO_OCR0(out_frequency);
		OCR1A = MS_TO_OCR1(2000);
}

void turn_on_sound()
{
		smth_is_playing = 1;
		//run timer1 with prescaler 1024:
		TCCR1B |= (1 << CS12) | (0 << CS11) | (1 << CS10);
		//run timer0 with prescaler 64:
		TCCR0 |= (0 << CS02) | (1 << CS01) | (1 << CS00);
}

void turn_off_sound()
{
	TCCR0 &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
	TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
	smth_is_playing = 0;
}

ISR(TIMER1_COMPA_vect)
{
	turn_off_sound();
	if (!IN_EMPTY)
	{
		get_next_frequency();
		turn_on_sound();
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
		if (!smth_is_playing)
		{
			get_next_frequency();
			turn_on_sound();
		}
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
