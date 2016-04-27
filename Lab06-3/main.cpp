//#define F_CPU 1000000UL

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

	DDRD=0x00; // input
	PORTD=0xFF;//pull up

}

double fre[8]={200,261.63,293.66,329.63,349.23,392,440,493.88};

int getdig()
{
	int k = ~PIND;
	if(k)
	{
		for(int i=0;i<8;++i)
			if(k & (1<<i))
				return i;
	}
	return -1;
}

void time0_normal(uint k)
{ // 203 273 304 343 360 415 465 526
	k  = sethz( k , 64);
	TCCR0A=0x00;
	while((~PIND)&0xFF){
		TCNT0=256L-k;
		TCCR0B=0b00000011;//64
		while((TIFR0&(1<<TOV0))==0);
		TCCR0B=0x00;
		TIFR0=(1<<TOV0);
		PORTC ^= 1;
	}
}

void time0_CTC(uint k)
{ // 198 264 294 330 344 395 440 495
	k  = sethz( k , 64);
	TCCR0A=0x02;
	OCR0A=k;
	while((~PIND)&0xFF){
		TCCR0B=0b00000011;//64
		while((TIFR0&(1<<OCF0A))==0);
		TCCR0B=0;
		TIFR0=(1<<OCF0A);
		PORTC ^= 1;
	}
}

void time2_normal(uint k)
{ //203	268	298	336	359	404	450	507
	k  = sethz( k , 32);
	TCCR2A=0x00;
	while((~PIND)&0xFF){
		TCNT2=256L-k;
		TCCR2B=0b00000011;//32
		while((TIFR2&(1<<TOV2))==0);
		TCCR2B=0x00;
		TIFR2=(1<<TOV2);
		PORTC ^= 1;
	}
}

void time2_CTC(uint k)
{ // 200	263	293	328	352	395	438	495
	k  = sethz( k , 32);
	TCCR2A=0x02;
	OCR2A=k;
	while((~PIND)&0xFF){
		TCCR2B=0b00000011;//32
		while((TIFR2&(1<<OCF2A))==0);
		TCCR2B=0;
		TIFR2=(1<<OCF2A);
		PORTC ^= 1;
	}
}

void time1_CTC(uint k)
{ //198	264	293	330	343	393	438	492
	k  = sethz( k , 64);
	TCCR1A=0;
	TCCR1B=(1<<WGM12);
	OCR1AH=k>>8;
	OCR1AL=k&0xFF;
	while((~PIND)&0xFF){
		TCCR1B^=0b00000011;//64
		while((TIFR1&(1<<OCF1A))==0);
		TCCR1B^=0b00000011;
		TIFR1=(1<<OCF1A);
		PORTC ^= 1;
	}
}

void time1_normal(uint k)
{ //202 271	303	343	358	414	463	523
	k  = sethz( k , 64);
	uint uk = 65536u - k ;
	TCCR1A=0;
	TCCR1B=0;
	while((~PIND)&0xFF){
		TCNT1H=uk>>8;
		TCNT1L=uk&0xFF;
		TCCR1B^=0b00000011;//64
		while((TIFR1&(1<<TOV1))==0);
		TCCR1B^=0b00000011;
		TIFR1=(1<<TOV1);
		PORTC ^= 1;
	}
}

int main(void)
{
	init();
	while(1){
		int k = getdig();
		if(k==-1)
			continue;

//		time0_normal(fre[k]);
//		time0_CTC(fre[k]);
//		time2_normal(fre[k]);
//		time2_CTC(fre[k]);
//		time1_CTC(fre[k]);
//		time1_normal(fre[k]);
		
	}
}

