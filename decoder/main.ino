#include<stdio.h>

volatile byte half_revolutionsl,half_revolutionsr;
unsigned long timeold;

const int l0=5;
const int l1=6;
const int r0=10;
const int r1=11;

void countl()
{
	half_revolutionsl++;
	//Serial.println("detect");
}
void countr()
{
	half_revolutionsr++;
	//Serial.println("detect");
}

void setup()
{
	Serial.begin(9600);
	attachInterrupt(0, countl, RISING);
	attachInterrupt(1, countr, RISING);
	half_revolutionsl = half_revolutionsr = 0;
	pinMode(l0,OUTPUT);
	pinMode(l1,OUTPUT);
	pinMode(r0,OUTPUT);
	pinMode(r1,OUTPUT);
	timeold = millis();
}

void loop()//Measure RPM
{
	analogWrite(l0,0);
	analogWrite(l1,100);
	analogWrite(r0,0);
	analogWrite(r1,120);

	delay(10);
	int timedur = (millis() - timeold);
	double rpml = 1000.000/timedur*half_revolutionsl,
	       rpmr = 1000.000/timedur*half_revolutionsr;
	half_revolutionsl = half_revolutionsr = 0;
	if( rpml>1 || rpmr>1 )
	{
		Serial.println(rpml);
		Serial.println(rpmr);
		Serial.println("----");

	}
	timeold = millis();
}

