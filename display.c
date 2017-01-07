#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

#define F_CPU 1000000UL  // 1 MHz
#define RS 0
#define RW 1
#define E 2

#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>

void setup_io()
{
	TCCR0 |= (1 << CS00) | (1 << CS02);//prescaler 1024
	TIMSK |= (1 << TOIE0);//timer0 overflow  interrupt enable;
	sei();//interrupts enable
}

bool receive_busy_flag()
{
	DDRA = 0;
	PORTA &= ~0;
	PORTC &= ~(1 << RS);
	PORTC |= (1 << RW);
	PORTC |= (1 << E);
	PORTC &= ~(1 << E);
	return (PORTC & 0b10000000);
}

void start_sending_command()
{
	DDRA = ~0;//вывод
	PORTA = 0;
	DDRC |= 111;//вывод
	PORTC = 0;
	PORTC &= ~((1 << RW) | (1 << RS));
	PORTC |= (1 << E);
	while (receive_busy_flag()) {};
}

void end_sending()
{
	PORTC &= ~(1 << E);
	PORTA = 0;
}

void start_sending_data()
{
	DDRA = ~0;//вывод
	PORTA = 0;
	DDRC |= 111;//вывод
	PORTC = 0;
	PORTC &= ~(1 << RW);
	PORTC |= (1 << RS);
	PORTC |= (1 << E);
	while (receive_busy_flag()) {}
}



void display_visibility_on()
{
	start_sending_command();
	PORTA = 0b00001111;
	end_sending();
}

void display_init()
{
	_delay_us(15);
	PORTA |= (1 << 5) | (1 << 4);
	_delay_ms(5);
	PORTA |= (1 << 5) | (1 << 4);
	_delay_us(100);
	PORTA |= (1 << 5) | (1 << 4);

	start_sending_command();
	PORTA |= (1 << 5) | (1 << 4) | (1 << 3) | (1 << 2);//3>2 lines,2>5x10dots
	end_sending();

	start_sending_command();
	PORTA |= (1 << 3);//display off
	end_sending();

	start_sending_command();
	PORTA |= (1 << 3);//display clear
	end_sending();

	start_sending_command();
	PORTA |= (1 << 2) | (1 << 1) | (0 << 0);//increment adress counter, zero shift
	end_sending();
}

int main()
{
	/*static uint8_t cycles_counter = 0;
	setup_io();
	while(true)
	{
		sleep_cpu();
		if (cycles_counter == 3)
		{

		}
		else
		{
			cycles_counter++;
		}
	}*/
	setup_io();
	display_init();
	display_visibility_on();

	start_sending_command();
	PORTA = 0b10000000;//DDRAM adress 00
	end_sending();

	start_sending_command();
	PORTA = 0b00010100;//cursor shift;
	end_sending();

	start_sending_data();//write data
	PORTA = 0b01000101;//'E'
	end_sending();


}
