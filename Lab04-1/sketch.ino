//#include <StandardCplusplus.h>

const int light=9;
int l;
void setup()
{
	Serial.begin(9600);
	pinMode(light,OUTPUT);
	l=0;

}

void loop()
{
	char r;
	if( Serial.available() )
		r = Serial.read();
	switch(r)
	{
		case '+': l+=10;if(l>256) l = 250;break;
		case '-': l-=10;if(l<0) l = 0 ;break;
	}
	analogWrite(light,l);
}
