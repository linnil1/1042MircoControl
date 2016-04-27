#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

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
void print_vol_to_dis(int ori,int num,int dot=0)
{
	char str[100];
	if(!dot)
		sprintf(str,"%d-> %d\n\r",ori,num);
	else
		sprintf(str,"%d-> %d.%d\n\r",ori,num/10,num%10);
	printserial(str);
	
	allavg += ori;
	if(all++==31)
	{
		sprintf(str,"average: %d \n\r",allavg/31);
		allavg=all=0;
		printserial(str);
	}

}


void shownum(int ori,int num,int dot=0)
{
	if(dot)
		PORTD=~(digit[num/10] | (1<<7));// .
	else
		PORTD=~digit[num/10] ;// .
	PORTC = (digit[num/10]&0x2)^0x2 ;// exculde for TX
	PORTB=~digit[num%10];
	print_vol_to_dis(ori,num,dot);
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

unsigned int getvol_2(int time)//right adjusted needed
{
	unsigned int d0=0,d1=0;
	for(int i=0;i<time;++i)
	{
		ADCSRA|=(1<<ADIF); // clear ADIF
		ADCSRA|=(1<<ADSC); // start ADC
		while((ADCSRA&(1<<ADIF))==0); // wait for ADC done
		d0+=ADCH&0x3; //high first i don't know why
		d1+=ADCL&0xFF;
	}
	return  (d0/time)*256+d1/time;
}

#define min(a,b) a<b? a:b;
int main(void)
{
	initserial();
// init dis	
	for(int i=0;i<disn;++i)
		dis[i]=1./dis[i],
		vol[i]=vol[i];

	DDRB=0xFF;
	PORTB=~digit[0];
	DDRD=0xFF;
	PORTD=~digit[0];
	DDRC=0x02;// 1
	
	ADCSRA=0b10000001; // enable + prescaler
	ADMUX =0b01000000;
	

	while(1){
		unsigned int d  =0 ; 
		for(int i=0;i<32;++i)
			d += getvol_1(32);
		d/=32;
		
		double k = (double)caldis(d);
		if(!k) // error
			k = 1./99;
		k = 1/k;
		uint8_t num = (uint8_t)k;
		if(num>=10)
		{
			num = min(99,num);
			shownum(int(d),num);
			
		}
		else
		{
			num = (uint8_t)(k*10);
			shownum(int(d),num,1);
		}
		_delay_ms(100);// 250 ms
	}
	return 1;
}

