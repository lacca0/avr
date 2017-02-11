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

void setup_timers()
{
	TCCR0 |= (1 << CS00) | (1 << CS02);//prescaler 1024
	TIMSK |= (1 << TOIE0);//timer0 overflow  interrupt enable;
	sei();//interrupts enable
}


void setup_io()
{
	DDRC |= (1 << RS) | (1 << RW) | (1 << E);//вывод
}

bool receive_busy_flag()
{
	DDRA = 0;//ввод
	PORTA = ~0;
	PORTC &= ~(1 << RS);
	PORTC |= (1 << RW);
	PORTC |= (1 << E);
	_delay_us(1);
	bool busy_flag = !!(PINA & (1 << 7));
	PORTC &= ~(1 << E);
	return busy_flag;
}

void start_sending_command()
{
	while (receive_busy_flag()) {};
	DDRA = ~0;//вывод
	PORTA = 0;
	PORTC = (1 << E);
}

void start_sending_command_init()
{
	DDRA = ~0;//вывод
	PORTA = 0;
	PORTC = (1 << E);
}

void end_sending()
{
	PORTC &= ~(1 << E);
	PORTA = 0;
}

void start_sending_data()
{
	while (receive_busy_flag()) {}
	DDRA = ~0;//вывод
	PORTA = 0;
	PORTC = (1 << RS) | (1 << E);
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

	start_sending_command_init();
	PORTA = 0b00110000;
	end_sending();
	_delay_ms(5);

	start_sending_command_init();
	PORTA = 0b00110000;
	end_sending();
	_delay_us(100);

	start_sending_command_init();
	PORTA = 0b00110000;
	end_sending();
	_delay_ms(1);

	start_sending_command_init();
	PORTA = 0b0011100;//8-bit interface, 2 lines,5x8dots
	end_sending();
	_delay_ms(1);

	start_sending_command_init();
	PORTA = 0b00001000;//display off
	end_sending();
	_delay_ms(1);

	start_sending_command_init();
	PORTA = 0b00000001;//display clear
	end_sending();
	_delay_ms(1);

	start_sending_command_init();
	PORTA = 0b00000110;//increment adress counter, zero shift
	end_sending();
	_delay_ms(1);

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
	//setup_timers;
	display_init();
	display_visibility_on();

	start_sending_command();
	PORTA = 0b10000000;//set DDRAM adress 00
	end_sending();

	/*start_sending_command();
	PORTA = 0b00010100;//cursor shift;
	end_sending();*/

	start_sending_data();//write data
	PORTA = 0b01001000;//'E'
	end_sending();

	start_sending_data();//write data
	PORTA = 0b01001001;//'E'
	end_sending();

	while(true)
	{
		sleep_cpu();
	}

}
