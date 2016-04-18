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
	//PORTD=0xF0;
	for(int i=0;i<4;++i)
	{
		PORTD=(1<<(i));//+0xF;
		int k = PIND>>4;
		for(int j=0;j<4;++j)
			if(k&(1<<j))
				//return board[3-i][j];
				return board[3-j][(3+i)%4];// i dont know why need to map
	}
	return -1;

}

/*int test()
{
	int k = PIND>>4;
	for(int j=0;j<4;++i)
		if(k&(1<<j))
			return j;
	return -1;
}*/
int main(void)
{
	DDRB=0b11111111; // output
	PORTB =~digit[0];

	DDRD=0x0F;
	/*while(1)
	{
		int k = test();
		if(k>0)
		{
			PORTB =~digit[k];
			_delay_ms(100);

		};
	}*/

	while(1){
		int k = find();
		if(k>=0)
		{
			PORTB =~digit[k];
			_delay_ms(100);

		};
		/*int k  = PINB;
		for(int i=0;i<16;++i)
		{
			PORTB = ~digit[i];
			_delay_ms(1000);
		}*/

	}
	return 1;
}

