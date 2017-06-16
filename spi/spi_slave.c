#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

#define F_CPU 8000000UL

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define DDR_SPI DDRB
#define DD_MOSI 3
#define DD_MISO 4
#define DD_SCK 5

// We'll (ab)use TX led on the Nano because the L led clashes with SCK
#define DDR_LED DDRD
#define PORT_LED PORTD
#define DD_LED 1

void SPI_SlaveInit(void)
{
 /* Set MISO output, all others input */
 DDR_SPI |= (1 << DD_MISO);
 /* Enable SPI */
 SPCR = (1 << SPE)|(0 << CPOL)|(0 << CPHA);
}

char SPI_SlaveReceive(void)
{
 /* Wait for reception complete */
 while(!(SPSR & (1 << SPIF)))
 ;
 /* Return Data Register */
 return SPDR;
}

void setup_io()
{
	DDR_LED |= 1 << DD_LED;
}

int main()
{
	setup_io();
	SPI_SlaveInit();

	PORT_LED &= ~(1 << DD_LED);
	uint8_t spi_buffer = SPI_SlaveReceive();
	_delay_ms(1000);
	PORT_LED |= (1 << DD_LED);

	for (char i = 0; i < 8; i++)
	{
		if (spi_buffer & (1 << i))
		{
			PORT_LED &= ~(1 << DD_LED);
			_delay_ms(900);
			PORT_LED |= (1 << DD_LED);
			_delay_ms(100);
		}
		else
		{
			PORT_LED &= ~(1 << DD_LED);
			_delay_ms(100);
			PORT_LED |= (1 << DD_LED);
			_delay_ms(900);
		}
	}
}
