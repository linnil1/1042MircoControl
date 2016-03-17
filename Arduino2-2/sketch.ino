//#include<time.h>
enum {G=0x1,F=0x2,E=0x4,D=0x8,C=0x10,B=0x20,A=0x40};

int digit[10]= { A+B+C+D+E+F , B+C , A+B+G+E+D , A+B+G+C+D , F+G+B+C,
	A+F+G+C+D , A+F+E+D+C+G , A+B+C , A+B+C+D+E+F+G , A+B+C+D+F+G};

const int base[2]={2,13};
const int nowtn=2;
#define LL long long 

void output(LL a,int b)
{
	a = digit[a];
	for(int i=0;i<7;++i,a>>=1)
		digitalWrite(base[b]+i,!(a&1));
}

void timetick(LL dtime)
{
	LL now=0;
	if(dtime==60000)
		now = 60;
	else if(dtime>0)
		now =  (dtime/1000)%60 ;
	
	for(int i=0;i<nowtn;++i,now/=10)
		output(now%10,i);
}

int stopmode=2,mode=0;
LL stopt=0,stopadd=0;

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
	if( digitalRead(9) )	//mode change 
	{
		if(mode==0)			//change to clock mode
			mode=stopmode;
		else 				//change to stopwatch mode
		{
			stopmode = mode ;
			mode =0 ;
		}
		while( digitalRead(9) )
			delay(1);
	}
	else if( digitalRead(10) )//cont  stop
	{
		if(mode == 0 )
			return ;
		
		else if( mode == 1)
		{
			mode = 2;
			stopadd+= (LL)millis()-stopt;
			while( digitalRead(10) )
				delay(1);
		}
		else if( mode == 2)
		{
			while( digitalRead(10) )
				delay(1);
			stopt = (LL)millis();
			mode = 1;
		}

	}
	else if( digitalRead(11) )//reset
	{
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
			timetick((LL)millis());break;
		case 1:
			timetick(60000-(stopadd+(LL)millis()-stopt));
			if(stopadd+(LL)millis()-stopt>=60000)
			{
				stopadd = 60000;
				mode=2;
			}
			break;
		case 2:
			timetick(60000-stopadd);break;
	}
}
