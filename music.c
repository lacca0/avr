#define TO_OCR0(freq) (F_CPU / PRES0) / freq / 2 // 2 is magic number for frequency change
#define MS_TO_OCR1(ms) (ms * F_CPU) / (1000 * PRES1)

bool smth_is_playing = 0;

void set_frequency(uint16_t freq)
{
	OCR0 = TO_OCR0(freq);
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
