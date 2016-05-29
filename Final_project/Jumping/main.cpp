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
{
	int k = 1000; // 1MHz
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

void printserial(int num)
{
	char str[100],*print_str=str; 
	sprintf(str,"%d\r\n",num);
	while(*print_str)
	{
        while(!(UCSR0A & (1<<UDRE0)));
        UDR0 = *(print_str++);
	}
}

void test(int num,int m)
{
	char str[100],*print_str=str; 
	sprintf(str,"--%d %d\r\n",num,m);
	while(*print_str)
	{
        while(!(UCSR0A & (1<<UDRE0)));
        UDR0 = *(print_str++);
	}
}

unsigned int readDis()
{
	unsigned int sum=0;
	for(int i=0;i<50;++i)
		sum += getvol(3,50);
	return sum/50;
}


void gostraight()
{
	set_motor(150,150);
	send_to_motor();
	while(getvol(3,50)<600);
	stop();
}

void go()
{
	set_motor(250,100);
	send_to_motor();
	int t=0;
	while(getvol(3,50) < 650 && ++t < 1000);
	if(t>=1000)
		gostraight();
	stop();
	if(readDis()>850)
	{
		set_motor(-150,-150);
		send_to_motor();
		my_delay(80);

	}
	stop();
}

void blink(int ms)
{
		for(int j=0;j<10;++j)
		{
			PORTB = 2;
			my_delay(ms);
			PORTB = 0;
			my_delay(ms);
		}
}

void turn(int level,int dir=1)
{
	set_motor(-150*dir,150*dir);
	send_to_motor();
	my_delay(level);
	stop();
	my_delay(10);
}

int where()
{
	int now = readDis();
	turn(100,-1);
	int next = readDis();
	if( now < 600 && next < 600)// go straight
		gostraight();
	else if( abs(now-next) < 15)
		turn(425);
	else if( now < next)
		turn(350);
	else // now > next
		turn(450);
	stop();
}


int main()
{
	init_motor();
	initserial();
	DDRB|=2;
	my_delay(2000);
	while(1)
	{
		go();
//		my_delay(2000);
		where();
//		my_delay(2000);
	}

	return 0;
}
