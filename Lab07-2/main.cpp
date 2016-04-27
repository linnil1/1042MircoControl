//#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

enum {G=0x40,F=0x20,E=0x10,D=0x08,C=0x04,B=0x02,A=0x01};

uint8_t digit[16]= { A+B+C+D+E+F , B+C , A+B+G+E+D , A+B+G+C+D , F+G+B+C,
	A+F+G+C+D , A+F+E+D+C+G , A+B+C , A+B+C+D+E+F+G , A+B+C+D+F+G,
A+B+C+E+F+G,F+G+E+C+D,A+F+E+D,B+C+G+E+D,A+F+G+E+D,A+F+G+E};

void initserial()
{
	CLKPR=(1<<CLKPCE);
	CLKPR=0b00000000;//  to 8 Mhz
    unsigned int BaudR = 9600;
    unsigned int UbrrV = (F_CPU / (BaudR*16UL))-1;
    UBRR0H=(unsigned char)(UbrrV>>8);	// set Baud rate
    UBRR0L=(unsigned)UbrrV;
    UCSR0C|=(1<<UCSZ01)|(1<<UCSZ00);	// normal mode, int clk
    UCSR0B|=(1<<TXEN0);			// enable transmit
}

void printserial(char *str)
{
	while(*str)
	{
        while(!(UCSR0A & (1<<UDRE0)));
        UDR0 = *(str++);
	}
}

int all=0,allavg=0;
void print_vol_to_dis(int ori,int* num,int dot=0)
{
	// array to number 
	char str[100];
	sprintf(str,"%d -> ",ori);
	int len = strlen(str);
	for(int i=0;i<=dot;++i)
		str[len++] = num[i] + '0' ;
	str[len++] = '.' ;
	for(int i=dot+1;i<4;++i)
		str[len++] = num[i] + '0' ;
	str[len++] = '\r' ;
	str[len++] = '\n' ;
	str[len  ] = '\0' ;
	
//	printserial(str);
	
	// additional
	allavg += ori;
	if(all++==31)
	{
		sprintf(str,"average: %d \n\r",allavg/31);
		allavg=all=0;
		printserial(str);
	}

}


const uint8_t disn=12;
double dis[disn]={99,80 ,50 ,30 ,20 ,15 ,10 ,9  ,8  ,7  ,6  ,  5} ;
double vol[disn]={0 ,125,167,232,316,414,577,622,698,772,800,805};

double caldis(double d)  // return 1/dis
{
	for(uint8_t i=1;i<disn;++i)
		if( vol[i] >= d &&  d > vol[i-1])
			return dis[i-1]+(dis[i]-dis[i-1])*((d-vol[i-1])/(vol[i]-vol[i-1]));
	return 0;//error
}

unsigned int getvol_1(int time)// time need to be smaller than 64 
{
	unsigned int d=0;
	for(int i=0;i<time;++i)
	{
		ADCSRA|=(1<<ADIF); // clear ADIF
		ADCSRA|=(1<<ADSC); // start ADC
		while((ADCSRA&(1<<ADIF))==0); // wait for ADC done
		d+=ADC;
	}
	return d/time;
}

//------4 digits 7 segments

void switchdigit(int num)// 0 1 2 3
{
	if(num>=1)// for TX
		++num;
	PORTD = (1<<num);
}

void setonedigit(int num,int dig,int dot=0)
{
	switchdigit(num);
	PORTB = ~(digit[dig] + dot*(1<<7));
}

void showarray(int *dig,int dot,int time=1)
{
	if(dot==3) // the last one
		dot=-1;
	while(time--)
		for(int i=0;i<4;++i)
		{
			setonedigit(i,dig[i],i == dot);
			_delay_ms(1);
		}
}


#define min(a,b) a<b? a:b;
int main(void)
{
	ADCSRA=0b10000001; // enable + prescaler
	ADMUX =0b01000000; // 8 Mhz
	initserial();

// init dis	
	for(int i=0;i<disn;++i)
		dis[i]=1./dis[i],
		vol[i]=vol[i];

	DDRD = 0x1F;// control dig 1~4
	DDRB = 0xFF;// control digits
	DDRC = 0;

	int dis_arr[10],dis_dot;

	while(1){
		// get number
		unsigned int d  =0 ; 
		for(int i=0;i<32;++i)
		{
			d += getvol_1(32);
			showarray(dis_arr,dis_dot); // 1 ms
		}
		d/=32;
		
		// calcuate dis
		double k = (double)caldis(d);
		if(!k) // error
			k = 1./99;
		k = 1/k;

		// display dis
		int num = (int)k;
		dis_dot =3 ;
		while(num<=999)
		{
			num = (int)(k*=10);
			--dis_dot;
		}

		showarray(dis_arr,dis_dot); // 1 ms
		for(int i=1000,j=0;i>0;++j,i/=10)
			dis_arr[j] = num / i % 10;

		showarray(dis_arr,dis_dot,25); // 100 ms

	}
	return 1;
}

