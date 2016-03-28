//#include <StandardCplusplus.h>

const int wheel[2][2] = { {5,6},{10,11} };
int vel[2];

// w a d c s

const int obtact[4]={A5,A4,A3,A2};
void setup()
{
	Serial.begin(9600);
	for(int i=0;i<2;++i)
		for(int j=0;j<2;++j)
			pinMode(wheel[i][j],OUTPUT);
	vel[0]=vel[1]=0;

	for(int i=0;i<4;++i)
		pinMode(obtact[i],INPUT);
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
	int mmax=60;
	switch(c)
	{
		case 'w':
			vel[0]=vel[1]=max(vel[0],vel[1]);
			vel[0] = min(mmax,vel[0]+20);
			vel[1] = min(mmax,vel[1]+20);
			Serial.println('w');
			break;
		case 'x':
			vel[0]=vel[1]=min(vel[0],vel[1]);
			vel[0] = max(-mmax,vel[0]-20);
			vel[1] = max(-mmax,vel[1]-20);
			Serial.println('x');
			break;
		case 'a':
			vel[0] = max(-mmax,vel[0]-20);
			vel[1] = min(mmax,vel[1]+20);
			Serial.println('a');
			break;
		case 'd':
			vel[0] = min(mmax,vel[0]+20);
			vel[1] = max(-mmax,vel[1]-20);
			Serial.println('d');
			break;
		case 's':
			vel[0]=vel[1]=0;
			Serial.println('s');
			break;
	}
	out();
			
}

int avg[4],dirhold=600,fronthold=500;

int getlight()
{
	for(int i=0;i<4;++i)
		avg[i]=0;
	for(int j=0;j<10;++j)
	{
		for(int i=0;i<4;++i)
			avg[i] += analogRead( obtact[i] );
		delay(1);
	}
	for(int i=0;i<4;++i)
		avg[i]/=10;
}

//white < hold < black
void loop()
{
	getlight();
	if(avg[0]>fronthold )
		godir('s');
//   0
// 1 3 2	
	else if(avg[1]>dirhold && avg[2]<dirhold)
		godir('a');
	else if(avg[1]<dirhold && avg[2]>dirhold)
		godir('d');
	else if(avg[2]<dirhold )
		godir('w');
	else if(avg[2]<dirhold && avg[1]>dirhold && avg[1]>dirhold)
		godir('s');
}
