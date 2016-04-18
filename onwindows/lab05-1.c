#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

enum {G=0x40,F=0x20,E=0x10,D=0x08,C=0x04,B=0x02,A=0x01};

int digit[16]= { A+B+C+D+E+F , B+C , A+B+G+E+D , A+B+G+C+D , F+G+B+C,
	A+F+G+C+D , A+F+E+D+C+G , A+B+C , A+B+C+D+E+F+G , A+B+C+D+F+G,
A+B+C+E+F+G,F+G+E+C+D,A+F+E+D,B+C+G+E+D,A+F+G+E+D,A+F+G+E};

int board[4][4]={{0,1,4,7},{10,2,5,8},{11,3,6,9},{15,14,13,12}};
int find()
{
	//row
	DDRB=0xF0;
	PORTB=0x0F;
	for(int i=0;i<4;++i)
	{
		PORTB=(1<<(4+i))+0xF;
		int k = PINB;
		for(int j=0;j<4;++i)
		if(PINB&(1<<j))
		return board[i][j];
	}
	return -1;

}

int main(void)
{
	DDRC=0b11111111;

	int t=1;
	PORTC=~digit[0];

	DDRB=0;
	DDRD=0b11110100; // C6 is broken so put it D2
	PORTB=0xFF;
	PORTD=0b11110111;

	//PORTB=0;

	while(1){
		int k  = PINB;
		if( k)
		{
			for(int i=0;i<8;++i)
			if((~k)&(1<<i))
			{
				PORTC=~digit[i];
				if((digit[i])&0b01000000)
				PORTC ^=   0b01000000,
				PORTD =    0b11110011;
				else
				PORTD =    0b11110111;
			}
			_delay_ms(100);

		}
		k  = PIND;
		if( k)
		{
			for(int i=0;i<2;++i)
			if((~k)&(1<<i))
			{
				PORTC=~digit[i+8];
				if((digit[i+8])&0b01000000)
				PORTC ^=   0b01000000,
				PORTD  =   0b11110011;
				else
				PORTD  =   0b11110111;
			}
			_delay_ms(100);

		}
		

	}
	return 1;
}

