#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

enum {G=0x40,F=0x20,E=0x10,D=0x08,C=0x04,B=0x02,A=0x01};

int dig[17]= { 0,A+B+C+D+E+F , B+C , A+B+G+E+D , A+B+G+C+D , F+G+B+C,
	A+F+G+C+D , A+F+E+D+C+G , A+B+C , A+B+C+D+E+F+G , A+B+C+D+F+G,
A+B+C+E+F+G,F+G+E+C+D,A+F+E+D,B+C+G+E+D,A+F+G+E+D,A+F+G+E};

int board[4][4]={
{0x0,0x1,0x4,0x7},
{0xA,0x2,0x5,0x8},
{0xB,0x3,0x6,0x9},
{0xF,0xE,0xD,0xC}};

#define usint uint8_t

int find()
{
	for(int i=0;i<4;++i)
	{
		PORTD=0xFF ^ (1<<i);
		usint k = ~(PIND>>4);
		for(int j=0;j<4;++j)
			if( k & (1<<j))
				return board[i][j];
	}
	return -1;

}

/*
c4 F  E  D  C
c3 B  3  6  9
c2 A  2  5  8
c1 0  1  4  7
   r1 r2 r3 r4

r1 r2 r3 r4 c3 c2 c1 c4
*/


int main(void)
{
	int *digit = dig+1;
	DDRB=0xFF; // output
	PORTB =~digit[-1];

	DDRD=0x0F; //input output

	while(1)
	{
		int k = find();
		PORTB =~digit[k];

	}
	return 1;
}

