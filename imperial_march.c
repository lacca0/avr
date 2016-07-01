#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define F_CPU 1000000UL  // 1 MHz

#define PRES0 64UL
#define PRES1 1024UL

#define ONE_MUSICAL_SXTNTH_LENGTH 143 // in ms

#define TO_OCR0(freq) (F_CPU / PRES0) / freq / 2 // 2 is magic number for frequency change
#define MS_TO_OCR1(ms) (ms * F_CPU) / (1000 * PRES1)
#define TO_OCR1(n) MS_TO_OCR1(n * ONE_MUSICAL_SXTNTH_LENGTH) // n is the number of musical sxtnths in note

// PB3 is connected with buzzer, PD3 with button

void setup_io()
{
	//PD3 - input, PullUp
	DDRD &= ~(1 << PD3);
	PORTD |= (1 << PD3);
	//PB3 - output
	DDRB |= (1 << PB3);
	PORTB |= (1 << PB3);
}

void turn_off_sound()
{
	TCCR0 &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
}

void setup_sound()
{
	turn_off_sound();
	//turn on the sound with prescaler 64:
	TCCR0 |= (0 << CS02) | (1 << CS01) | (1 << CS00);
}

void pause()
{
	turn_off_sound();
	//change current duration of pause:
	OCR1A = ONE_MUSICAL_SXTNTH_LENGTH / 2;
}

void change_note()
{
	static const uint8_t fq_len = 72;
	static uint8_t frequency_const[] = {
		TO_OCR0(98),  TO_OCR0(98),  TO_OCR0(98),  TO_OCR0(78),  TO_OCR0(117),
		TO_OCR0(98),  TO_OCR0(78),  TO_OCR0(117), TO_OCR0(98),  0,
		TO_OCR0(147), TO_OCR0(147), TO_OCR0(147), TO_OCR0(156), TO_OCR0(117),
		TO_OCR0(93),  TO_OCR0(78),  TO_OCR0(117), TO_OCR0(98),
		TO_OCR0(196), TO_OCR0(98),  TO_OCR0(98),  TO_OCR0(196), TO_OCR0(185), TO_OCR0(175),
		TO_OCR0(165), TO_OCR0(156), TO_OCR0(165), 0,            TO_OCR0(104), TO_OCR0(139), TO_OCR0(131), TO_OCR0(123),
		TO_OCR0(117), TO_OCR0(110), TO_OCR0(117), 0,            TO_OCR0(78),  TO_OCR0(93),  TO_OCR0(78),  TO_OCR0(93),
		TO_OCR0(117), TO_OCR0(98),  TO_OCR0(117), TO_OCR0(147),
		TO_OCR0(196), TO_OCR0(98),  TO_OCR0(98),  TO_OCR0(196), TO_OCR0(139), TO_OCR0(175),
		TO_OCR0(165), TO_OCR0(78),  TO_OCR0(165), 0,            TO_OCR0(104), TO_OCR0(139), TO_OCR0(131), TO_OCR0(123),
		TO_OCR0(117), TO_OCR0(110), TO_OCR0(117), 0,            TO_OCR0(78),  TO_OCR0(93),  TO_OCR0(78),  TO_OCR0(117),
		TO_OCR0(98),  TO_OCR0(78),  TO_OCR0(117), TO_OCR0(98),  0,
	};

	static uint16_t timing[] = {
		TO_OCR1(4), TO_OCR1(4), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(4),  TO_OCR1(4),
		TO_OCR1(4), TO_OCR1(4), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(16),
		TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(1), TO_OCR1(1), TO_OCR1(2),  TO_OCR1(2),
		TO_OCR1(2), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(1), TO_OCR1(1), TO_OCR1(2), TO_OCR1(2),
		TO_OCR1(2), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(8),
		TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(1), TO_OCR1(1), TO_OCR1(2),  TO_OCR1(2),
		TO_OCR1(2), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(1), TO_OCR1(1), TO_OCR1(2), TO_OCR1(2),
		TO_OCR1(2), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(4), TO_OCR1(3), TO_OCR1(1), TO_OCR1(8), TO_OCR1(32),
	};

	static uint8_t counter = 0;

	if (frequency_const[counter] == 0)
	{
		turn_off_sound();
	}
	else
	{
		//change frequency:
		OCR0 = frequency_const[counter];
		setup_sound();
	}

	//change current duration of note:
	OCR1A = timing[counter];

	counter++;
	if (counter == fq_len)
	{
		counter = 0;
	}
}

ISR(INT1_vect)
{
	//prescaler 1024:
	TCCR1B |= (1 << CS12) | (0 << CS11) | (1 << CS10);
	change_note();
}

ISR(TIMER1_COMPA_vect)
{
	static bool counter = 0;

	if (counter == 0)
	{
		change_note();
		counter = 1;
	}
	else
	{
		pause();
		counter = 0;
	}
}

int main(void)
{
	setup_io();

	//timer 0 for sound: clear on compare mode, toggle int1 on compare, timer stopped:
	TCCR0 = (0 << CS02) | (0 << CS01) | (0 << CS00) | (0 << WGM00) | (1 << WGM01) | (1 << COM00) | (0 << COM01);
	//timer 1 for timing: clear on compare(WGM), output pins disconnected(COM), timer stopped(CS):
	TCCR1A = (0 << COM1A1) | (0 << COM1A0) | (0 << COM1B1) | (0 << COM1B0) | (0 << WGM11) | (0 << WGM10);
	TCCR1B = (0 << WGM13) | (1 << WGM12) | (0 << CS12) | (0 << CS11) | (0 << CS10);
	//enable interrupt on timer 1 compare.
	TIMSK |=  (1 << OCIE1A);

	//enable external interrupts, falling edge generates a request:
	MCUCR |= (1 << ISC11) | (0 << ISC10);
	GICR |= (1 << INT1);

	sei();

	sleep_enable();

	while (true)
	{
		sleep_cpu();
	}
}
