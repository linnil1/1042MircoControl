#include<stdio.h>

volatile byte half_revolutionsl,half_revolutionsr;
unsigned long timeold;

const int l0=5;
const int l1=6;
const int r0=10;
const int r1=11;

void countl() { half_revolutionsl++; }
void countr() { half_revolutionsr++; }

int speedl=100,speedr=100;
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
	speedl = speedr = 100;
	timeold = millis();

	speedl = speedr = 100;
}

void update_rpm()
{
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
	if(rpml > rpmr)
		--speedl,
		++speedr;
	else if(rpml < rpmr)
		++speedl,
		--speedr;
		
	timeold = millis();
}

void loop()//Measure RPM
{
	delay(10);
	while(true)
	{
		analogWrite(l0,0);
		analogWrite(l1,speedl);
		analogWrite(r0,0);
		analogWrite(r1,speedr);
		delay(10);
		update_rpm();
	}
}

