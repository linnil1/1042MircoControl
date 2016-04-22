//#define F_CPU 1000000UL

enum {G=0x40,F=0x20,E=0x10,D=0x08,C=0x04,B=0x02,A=0x01};
int digit[8]= { A+F+E+D,B+C+G+E+D,A+F+G+E+D,A+F+G+E,A+F+C+D+E,A+B+C+E+F+G,F+G+E+C+D,0xFF};

#include <avr/io.h>
#include <math.h>

#define usint uint8_t
#define uint unsigned int
uint sethz(double hz,long mult)// hz < 500
{
	return (uint)floor((double)1000000/(mult*hz*2));
}

void init()
{
	CLKPR=(1<<CLKPCE);
	CLKPR=0b00000011;// set clkto 1Mhz

	DDRC=0x01;// PC0 as sound
	PORTC=0;// initial sound 0

	DDRB=0XFF; //output
	PORTD = ~0;

}

double fre[7]={261.63,293.66,329.63,349.23,392,440,493.88};

void time1_CTC(uint k,long del,usint out)//del is ms
{ //198	264	293	330	343	393	438	492
	TCCR1A=0;
	TCCR1B=(1<<WGM12);
	OCR1AH=k>>8;
	OCR1AL=k&0xFF;

	long tick= (long)k*2000/del; 
	
	while( tick--  )
	{
		TCCR1B^=0b00000011;//64
		while((TIFR1&(1<<OCF1A))==0);
		TCCR1B^=0b00000011;
		TIFR1=(1<<OCF1A);
		PORTC ^= out;
	}
}

void playsound(int feq,int t)
{
	PORTB = ~digit[feq];
	time1_CTC( sethz(fre[feq],64),100,1);
	PORTC=0;
	PORTB = ~0;
	time1_CTC( sethz(fre[feq],64),t*500,0);
}

int twinkle[6*7]={
0,0,4,4,5,5,4,
4,4,3,3,2,2,1,
5,5,4,4,3,3,2,
5,5,4,4,3,3,2,
1,1,5,5,6,6,5,
4,4,3,3,2,2,1
};

int main(void)
{
	init();
	while(1){
		for(int i=0;i<6;++i)
		{
			for(int j=0;j<6;++j)
				playsound(twinkle[i*7+j],1);
			playsound(twinkle[i*7+6],2);
		}
	}
	return 0;
}

