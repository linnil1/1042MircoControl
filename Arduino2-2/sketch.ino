//#include<time.h>
enum {G=0x1,F=0x2,E=0x4,D=0x8,C=0x10,B=0x20,A=0x40};

int digit[10]= { A+B+C+D+E+F , B+C , A+B+G+E+D , A+B+G+C+D , F+G+B+C,
	A+F+G+C+D , A+F+E+D+C+G , A+B+C , A+B+C+D+E+F+G , A+B+C+D+F+G};

const int base[2]={2,13};
#define UL unsigned long 

void output(UL a,int b)
{
//	Serial.print(a);
//	Serial.print(":");
	a = digit[a];
	for(int i=0;i<7;++i,a>>=1)
	{
		digitalWrite(base[b]+i,!(a&1));
//		Serial.print(a&1);
	}
//	Serial.println("");
}

void timetick(UL dtime)
{
	UL now=0;
	if(dtime==60000)
		now = 60;
	else if(dtime>0)
		now =  (dtime/1000)%60 ;
	output(now/10,1);
	output(now%10,0);
}

const int nowtn=2;
int stopmode=2,mode=0;
UL stopt=0,stopadd=0;
void setup() {
	
	Serial.begin(9600);
	//7 segment
	for(int j=0;j<nowtn;++j)
		for(int i=0;i<7;++i)
			pinMode(base[j]+i,OUTPUT);
	// mode 
	for(int i=9;i<=11;++i)
		pinMode(i,INPUT);

	stopt=stopadd=0;
	mode=0;
	stopmode=2;
}



// 0 clock , 1 stopwatch cont, 2 stay
void getmode()
{
	if( digitalRead(9) )//mode change 
	{
//		Serial.print(9);
		if(mode==0)
			mode=stopmode;
		else
		{
			stopmode = mode ;
			mode =0 ;
		}
		while( digitalRead(9) )
			delay(1);
	}
	else if( digitalRead(10) )//cont  stop
	{
//		Serial.print(10);
		if(mode == 0 )
			return ;
		
		else if( mode == 1)
		{
			mode = 2;
			stopadd+= millis()-stopt;
		}
		else if( mode == 2)
		{
			stopt = millis();
			mode = 1;
		}

		while( digitalRead(10) )
			delay(1);
	}
	else if( digitalRead(11) )//reset
	{
//		Serial.print(11);
		if(mode==0)
			return ;
		stopt=stopadd=0;
		mode = 2;

		while( digitalRead(11) )
			delay(1);
	}
}

void loop() 
{
//	delay(10);
	getmode();
	switch(mode)// 0 clock , 1 stopwatch cont, 2 stay
	{
		case 0:
			timetick(millis());break;
		case 1:
			timetick(60000-(stopadd+millis()-stopt));break;
		case 2:
			timetick(60000-stopadd);break;
	}
}
