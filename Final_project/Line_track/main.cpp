#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
//#include <StandardCplusplus.h>
//#include <cmath>
//#include <vector>


int mot[2];


void send_to_motor()
{
	int a,b;
	// for left
	if( mot[0] > 0 )
		a= mot[0],b=0;
	else 
		b=-mot[0],a=0;
	OCR0A = a ;
	OCR0B = b ;

	// for right
	if( mot[1] > 0 )
		a= mot[1],b=0;
	else 
		b=-mot[1],a=0;
	OCR2A = a ;
	OCR2B = b ;
}

#define lim_max(a) if(a> 255) a =  255;
#define lim_min(a) if(a<-255) a = -255;
#define abs(a) ((a)<0?-(a):(a))

void set_motor(int l,int r)
{
	mot[0] = l;
	mot[1] = r;
	lim_min(mot[0]);
	lim_max(mot[0]);
	lim_min(mot[1]);
	lim_max(mot[1]);
}

void stop()
{
	mot[0]=mot[1]=0;
	send_to_motor();
}

void init_motor()
{
	CLKPR = (1<<CLKPCE);
	CLKPR |= 0b00000011; // to 1 Mhz
	set_motor(0,0);
	
	DDRD = 0b01101000;
	DDRB = 0b00001000;
	
	TCCR0A=0b10100011;// fast PWM, non-inverted
	TCCR0B=0b00000010;// timer prescaler
	
	TCCR2A=0b10100011;// fast PWM, non-inverted
	TCCR2B=0b00000010;// timer prescaler
}

unsigned int getvol(int pin,int time=1)// time need to be smaller than 64 
{
	ADCSRA=0b10000001; // enable + prescaler
	ADMUX =0b01000000 | pin; // 8 Mhz
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

void time1_CTC()
{ //198	264	293	330	343	393	438	492
	int k = 1000;
	TCCR1A=0;
	TCCR1B=(1<<WGM12);
	OCR1AH=k>>8;
	OCR1AL=k&0xFF;
	TCCR1B^=0b00000001;//64
	TIFR1=(1<<OCF1A);
	while((TIFR1&(1<<OCF1A))==0);
	TCCR1B^=0b00000001;
}

void my_delay(long t)//ms
{
	while(t--)
		time1_CTC();
}


void initserial()
{
    unsigned int BaudR = 4800;
    unsigned int UbrrV = (F_CPU / (BaudR*16UL))-1;
    UBRR0H=(unsigned char)(UbrrV>>8);	// set Baud rate
    UBRR0L=(unsigned)UbrrV;
    UCSR0C|=(1<<UCSZ01)|(1<<UCSZ00);	// normal mode, int clk
    UCSR0B|=(1<<TXEN0);			// enable transmit
}

void printserial(int *num)
{
	char str[100],*print_str=str; 
	sprintf(str,"%d %d %d\r\n",num[0],num[1],num[2]);
	while(*print_str)
	{
        while(!(UCSR0A & (1<<UDRE0)));
        UDR0 = *(print_str++);
	}
}

void toLevel(int *num)
{
	for(int i=0;i<3;++i)
		num[i] = getvol(i,30);
	num[0]-=100;
	num[2]-=250;
//	printserial(num);
	for(int i=0;i<3;++i)
	{
		/*
		if(num[i]<400)
			num[i]=400;
		else if(num[i]>800)
			num[i]=800;
		num[i]=(num[i]-400)/50;
		*/
		if(num[i]<600)
			num[i]=0;
		else if(num[i]>750)
			num[i]=2;
		else 
			num[i]=1;
	}
}


int main()
{
	init_motor();
	initserial();
	my_delay(2000);

	/*
	while(1)
	{
		int num[3];
		toLevel(num);
		printserial(num);
	}
	*/

	int num[3],sp=0,kp=80;
	int t=0;
	while(1)
	{
		toLevel(num);
		if(num[0]+num[1]+num[2]==0)
		{
			if(++t==10)
				break;
		}
		else
			t=0;
		int r=0;
		for(int i=0;i<3;++i)
			r += (i-1)*num[i];
		int e = sp-r;
		int v = kp*e;
		set_motor(150-v,150+v);
		send_to_motor();
		my_delay(10);
	}
	stop();



	return 0;
}
