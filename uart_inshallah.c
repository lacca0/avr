#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <string.h>

#define F_CPU 1000000UL  // 1 MHz

#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>
#define PRES2 1024UL
#define MY_OCR2 MS_TO_CLOCKS(3, PRES2) - 1 // the real (actual) period between comparator trigger events is OCRn + 1, so subtract one

#define CELLS_NUMBER 3
#include "lib/utilities.c"
#include "lib/sound.c"

uint8_t display_current_number[CELLS_NUMBER] = {0, 0, 0};

#include "lib/7seg_display.c"

#include "lib/uart_alt.c"

//PB3 is connected to buzzer.
//PORTB 0,1,2 pins are connected to display, all PORTA for segments. (if changed change "7seg_display.c")

ISR(TIMER1_COMPA_vect)
{
	static bool counter = 0;
	if (counter == 0)
	{
		sound_change_note();
		counter = 1;
	}
	else
	{
		sound_pause();
		counter = 0;
	}
}

uint8_t state_flag = 0;

void timer2_init()
{
	//prescaler 1024
	TCCR2 |= (1 << CS22) | (1 << CS21) | (1 << CS20);
	TIMSK |= (1 << OCIE2);
}

ISR(USART_RXC_vect)
{
	uint8_t MYUDR = UDR;
	static uint8_t in_value[CELLS_NUMBER] = {};
	static uint8_t counter = 0;
	if ((MYUDR >= '0') && (MYUDR <= '9'))
	{
		if (counter < CELLS_NUMBER)
		{
			in_value[counter] = MYUDR - '0';
		}
		counter++;
	}
	else if (MYUDR == '\r')
	{
		if ((counter > 0) && (counter <= CELLS_NUMBER))
		{
			//настройка дисплея:
			memcpy(display_current_number + CELLS_NUMBER - counter, in_value, counter);
			memset(display_current_number, 0, CELLS_NUMBER - counter);
			display_zero_flag = 0;
			timer2_init();
			//настройка вывода по uart:
			memset(uart_outcoming_number, 0, UART_ARRAY_LEN);
			memcpy(uart_outcoming_number + UART_ARRAY_LEN - counter, in_value, counter);
			uart_outcoming_digits_cnt = counter;
			UCSRB |= (1 << UDRIE);
			//отключение музыки:
			sound_note_counter = 0;
			sound_turn_off();
		}
		memset(in_value, 0, CELLS_NUMBER);
		counter = 0;
	}
	else
	{
		memset(in_value, 0, CELLS_NUMBER);
		counter = CELLS_NUMBER + 1; // далее вводимые символы игнорируются до ввода "enter".
	}
}

ISR(TIMER2_COMP_vect)
{
	static uint16_t cycles_counter = 0;
	display_show_next_digit();
	cycles_counter++;
	if(cycles_counter == MS_TO_CLOCKS(1000, PRES2 * MY_OCR2))
	{
		cycles_counter = 0;
		display_value_decrement();
		if (display_zero_flag)
		{
			//stop timer2:
			TCCR2 &= ~(1 << CS22) | (1 << CS21) | (1 << CS20);
			TIMSK &= ~(1 << OCIE2);
			//buzzer!
			sound_turn_on();
			sound_change_note();
		}
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

	//***************************display:
	//timer 2 for indicator update: clear on compare mode(WGM), output pins disconnected(COM), timer stopped(CS):
	TCCR2 |= (1 << WGM21) | (0 << WGM20) | (0 << COM21) | (0 << COM20) | (0 << CS22) | (0 << CS21) | (0 << CS20);
	OCR2 = MY_OCR2;
	DDRA &= ~0;
	DDRB |= 0b111;
	//enable interrupt on timer 2 compare.
	TIMSK |=  (1 << OCIE2);

	//****************************buzzer:
	//PB3 - output
	DDRB |= (1 << PB3);
	PORTB |= (1 << PB3);

	//****************************sound:
	//timer 0 for sound: clear on compare mode, toggle int1 on compare, timer stopped:
	TCCR0 = (0 << CS02) | (0 << CS01) | (0 << CS00) | (0 << WGM00) | (1 << WGM01) | (1 << COM00) | (0 << COM01);
	//timer 1 for timing: clear on compare(WGM), output pins disconnected(COM), timer stopped(CS):
	TCCR1A = (0 << COM1A1) | (0 << COM1A0) | (0 << COM1B1) | (0 << COM1B0) | (0 << WGM11) | (0 << WGM10);
	TCCR1B = (0 << WGM13) | (1 << WGM12) | (0 << CS12) | (0 << CS11) | (0 << CS10);
	//enable interrupt on timer 1 compare.
	TIMSK |=  (1 << OCIE1A);

	//***************************UART:
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
