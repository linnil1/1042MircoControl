//#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

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

void mult_motor(double mult)
{
	mot[0] *= mult;
	mot[1] *= mult;
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


int main()
{
	init_motor();
	_delay_ms(1000);

	int k=0;
	while(1)
	{
		if((k++)&1)
		{
			set_motor(70,70);
			send_to_motor();
			_delay_ms(4000);
		}
		else
		{
			set_motor(65,65);
			send_to_motor();
			_delay_ms(2000);
		}
		set_motor(70,70);
		_delay_ms(500);
		for(int i=0;i<10;++i)
		{
			change_motor(-10,10);
			send_to_motor();
			_delay_ms(57);
		}
	}
}
