//#include "StandardCplusplus.h"

const int in=12;
const int out=13;
void setup() {
	pinMode(in,INPUT);
	pinMode(out,OUTPUT);
}


int b=0,Dtime=500;
void loop() {
	if (digitalRead(in))
		Dtime=500;
	else
		Dtime = 2000;
	digitalWrite(out,b);
	delay(Dtime);
	b=!b;
}
