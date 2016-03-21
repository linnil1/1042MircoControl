//#include <StandardCplusplus.h>

const int wheel[2][2] = { {5,6},{10,11} };
int vel[2];

// w a d c s q	
int dir[6][2] = { {20,20} , {-20,20} , {20,-20}, {-20,-20} , {255,255} ,{0,0} };
int cdir[255];
void setup()
{
	Serial.begin(9600);
	for(int i=0;i<2;++i)
		for(int j=0;j<2;++j)
			pinMode(wheel[i][j],OUTPUT);
	vel[0]=vel[1]=0;
	char cd[7]={"WADCSQ"};
	for(int i=0;i<7;++i)
		cdir[cd[i]]=i;
}

inline void out()
{
	for(int i=0;i<2;++i)
		if(vel[i]<0)
		{
			analogWrite(wheel[i][0],-vel[i]);
			analogWrite(wheel[i][1],0);
		}
		else
		{
			analogWrite(wheel[i][0],0);
			analogWrite(wheel[i][1],vel[i]);
		}
}


void godir(char c)
{
	for(int i=0;i<2;++i)
	{
		vel[i] += dir[ cdir[c] ][i];
		if(vel[i]>255)
			vel[i]=255;
		else if(vel[i]<-255)
			vel[i]=-255;
	}
	out();
}

void turn(int angel)
{
	for(int i=100;i>=-90;i--)
	{
		vel[0] = i;
		out();
		delay(1);
	}
	vel[0] = -100;
	vel[1] = 100;
	out();
	delay(150);
}


void loop()
{
	vel[0]=vel[1]=100;
	out();
	delay(1800);
	turn(90);
	vel[0]=vel[1]=100;
	out();
	delay(3200);
	turn(90);
//	delay(1000);
//	turn(90);
//	vel[0]=vel[1]=255;
//	out();
//	delay(800);
//	turn(90);
	
}
