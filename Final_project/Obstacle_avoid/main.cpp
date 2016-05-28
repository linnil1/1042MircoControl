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
void change_motor(int l,int r)
{
	mot[0] += l;
	mot[1] += r;
	lim_min(mot[0]);
	lim_max(mot[0]);
	lim_min(mot[1]);
	lim_max(mot[1]);
}

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


void bigturn(char c,int deg=2)
{
	set_motor(0,0);
	send_to_motor();
	my_delay(100);
	switch(c)
	{
		case 'a':
			set_motor(-140,140);
			break;
		case 'd':
			set_motor(140,-140);
			break;
	}
	send_to_motor();
	my_delay(130*deg);

	stop();
}

/* NOTE
 * We can't use max because the car will move
 * We can't use pre - num < 1 beacause the data is not very correct
 */

void blink()
{
		for(int j=0;j<10;++j)
		{
			PORTB = 2;
			my_delay(100);
			PORTB = 0;
			my_delay(100);
		}
}


void findnearest()
{
	//left
	int dir=1;
	
	int max = readDis();
	while(1)
	{
		PORTB = 0;
		set_motor(65*dir,-65*dir);
		send_to_motor();
		my_delay(300);
		stop();
		int num = readDis();
		if(num > max )
			max = num;//400 is normal
		else if (max>600 && num < max-5 )// 10 is range
		{
			PORTB = 2;
//			if(readDis()>=max-5)//double check
//				continue;
			dir *= -1;
			max = num;
			break;
		}
	}
	set_motor(60*dir,-60*dir);
	send_to_motor();
	my_delay(300);

	stop();
/*
	set_motor(-80,80);
	send_to_motor();
	my_delay(50);
	set_motor(0,0);
	send_to_motor();
	my_delay(1000);
*/
}

void findwall()
{
	set_motor(0,0);
	send_to_motor();
	int now = readDis();
	
	if(now <700)
	{
		// for far away
		set_motor(250,250);
		while(now < 400)
		{
			send_to_motor();
			now = getvol(3,50); 
		}

		// close it
		set_motor(150,150);
		while(now < 700)
		{
			send_to_motor();
			now = getvol(3,50); 
		}
	}
	else if(now>800)
	{
		// too close
		set_motor(-70,-70);
		while(now > 800)// back , need to be high idon'tkonw
		{
			send_to_motor();
			now = getvol(3,50); 
		}
	}

	stop();
}

bool go_some()// true -> wall
{
	if ( getvol(3,50) > 700) return true;
	set_motor(250,250);
	send_to_motor();
	for(int i=0;i<30;++i)//1000
	{
		my_delay(10);
		if ( getvol(3,50) > 700) return true;
	}
	//slow down
	for(int i=250;i>=0;i-=50)//300
	{
		set_motor(i,i);
		send_to_motor();
		for(int j=0;j<5;++j)
		{
			if ( getvol(3,50) > 700) return true;
			my_delay(10);
		}
	}
	return false;
}
bool go()
{
	bool tmp = go_some();
	stop();
	return tmp;
}


int main()
{
	init_motor();
	initserial();
	DDRB |= 2;
	my_delay(2000);

	//distance
	/*
	
	   while(1)
	{
		int now = readDis();
		printserial(now);
		my_delay(100);
	}

	while(1)
	{
//		findnearest();
		findwall();
		my_delay(2000);
	}
	*/


//	along the border
/*
	while(1)
	{
		go();
		set_motor(0,0);send_to_motor();
//		my_delay(1000);
		bigturn('d');
//		my_delay(1000);
		findwall();
//		my_delay(1000);
		bigturn('a');
//		my_delay(1000);
		findnearest();
//		my_delay(1000);
		bigturn('a');
//		my_delay(1000);
	}
*/
	
	int isWall=0,noright=0;
	while(1)
	{
		isWall = go();
		my_delay(1000);
//		printserial(isWall);
		bigturn('d');
		my_delay(1000);
		if( go() )
		{
			findwall();
			noright=0;
		}
		else 
		{
			bigturn('d');
			if(++noright ==1)
			{
				while(!go());
				noright=0;
				my_delay(1000);
				bigturn('a');
			}
			continue;
		}
		my_delay(1000);
		bigturn('a');
		if(isWall)// turn 180
			bigturn('a');
		my_delay(1000);
		findnearest();
		my_delay(1000);
		bigturn('a');
		my_delay(1000);
	}

	return 0;
}
