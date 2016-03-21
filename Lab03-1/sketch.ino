//#include <StandardCplusplus.h>

const int light=13;
int l,time;
void setup()
{
	Serial.begin(9600);
	pinMode(light,OUTPUT);
	time=500;
	l=1;
}

void loop()
{
	char r;
	if( Serial.available() )
		r = Serial.read();
	switch(r)
	{
		case 'a': time = 500;break;
		case 'b': time = 2000;break;
		case 'c': time = 8000;break;
	}
	digitalWrite(light,l);
	l = !l ;
	delay(time);
}
