//#define F_CPU 1000000UL

enum {G=0x40,F=0x20,E=0x10,D=0x08,C=0x04,B=0x02,A=0x01};

int digit[8]= { A+F+E+D,B+C+G+E+D,A+F+G+E+D,A+F+G+E,A+F+C+D+E,A+B+C+E+F+G,F+G+E+C+D,0xFF};

#include <avr/io.h>
#include <math.h>

void sethz(double hz)// hz < 500
{
	OCR0A = (int)floor((double)1000000/64/hz)/2;
}

void init()
{
	CLKPR=(1<<CLKPCE);
	CLKPR=0b00000011;// set clkto 1Mhz
	TCCR0A=0x02;//CTC

	DDRC=0x01;// PC0 as sound
	PORTC=0;// initial sound 0

	DDRB=0xFF; // output
	PORTB =~digit[7];

	DDRD=0x00; // input
	PORTD=0xFF;

}

double fre[7]={261.63,293.66,329.63,349.23,392,440,493.88};

int getdig()
{
	int k = ~PIND;
	if(k)
	{
		for(int i=0;i<7;++i)
			if(k & (1<<i))
				return i;
	}
	return -1;
}

int main(void)
{
	init();
	while(1){
		int k = getdig();
		if(k==-1)
			continue;
		PORTB =~digit[k];
		sethz(fre[k]);

		while((~PIND)&0x7f) // i don't know why
		{
			TCCR0B=0b00000011;// p=64, start Timer
			PORTC^=1;
			while((TIFR0&(1<<OCF0A))==0);
			TCCR0B=0x00;// stop Timer0
			TIFR0|=(1<<OCF0A);// clear TOV0
		}
		PORTB =~digit[7];

	}
}

