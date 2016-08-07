enum {
	motorlA = 5 ,
	motorlB = 6 ,
	motorrA = 9 ,
	motorrB = 10,
	ECHO_PIN1 = 8 ,
	TRIG_PIN1 = 7 ,
	ECHO_PIN2 = 12,
	TRIG_PIN2 = 13,
	ECHO_PIN3 = 11,
	TRIG_PIN3 = 4 , 
	ECHO_PIN4 = A5,
	TRIG_PIN4 = A4 
};

int CIR, DIR, AIR, BIR;
long dis1,dis2,dis3,dis4;
volatile byte half_revolutionsl, half_revolutionsr;
unsigned long timeold;
int speedl = 60, speedr = 60;

void countl() { half_revolutionsl++; }
void countr() { half_revolutionsr++; }

void update_rpm()
{
	int timedur = (millis() - timeold);
	double rpml = 1000.000 / timedur * half_revolutionsl,
           rpmr = 1000.000 / timedur * half_revolutionsr;
  half_revolutionsl = half_revolutionsr = 0;

	if (rpml > rpmr)
		--speedl,
		++speedr;
	else if (rpml < rpmr)
		++speedl,
		--speedr;

	timeold = millis();
}
void count_rpm(int n)
{
	half_revolutionsl = half_revolutionsr = 0 ;
	timeold  = millis();
	for(int i=0;i<n;++i)
	{
		delay(1);
		update_rpm();
	}
}

void setup()
{
	Serial.begin(38400);
	
	pinMode(motorlA, OUTPUT);
	pinMode(motorlB, OUTPUT);
	pinMode(motorrA, OUTPUT);
	pinMode(motorrB, OUTPUT);

	pinMode(ECHO_PIN1,  INPUT);
	pinMode(TRIG_PIN1, OUTPUT);
	pinMode(ECHO_PIN2,  INPUT);
	pinMode(TRIG_PIN2, OUTPUT);
	pinMode(ECHO_PIN3,  INPUT);
	pinMode(TRIG_PIN3, OUTPUT);
	pinMode(ECHO_PIN4,  INPUT);
	pinMode(TRIG_PIN4, OUTPUT);

	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	pinMode(A2, INPUT);
	pinMode(A3, INPUT);

	attachInterrupt(0, countl, RISING);
	attachInterrupt(1, countr, RISING);
}

int record_speedl , record_speedr;
void realGo(int speedl,int speedr)
{
	record_speedl = speedl;
	record_speedr = speedr;
	analogWrite(motorlA, speedl>0?0:-speedl);
	analogWrite(motorlB, speedl<0?0: speedl);
	analogWrite(motorrA, speedr>0?0:-speedr);
	analogWrite(motorrB, speedr<0?0: speedr);
}
void forward() {
	realGo(speedl,speedr);
	count_rpm(10);
}
void backoff() {
	realGo(-speedl,-speedr);
	count_rpm(10);
}
void right() {
	realGo(70,-130);
}
void left() {
	realGo(-130,70);
}
void stopmove() {
	realGo(0,0);
}

long trigerSend(int trig, int echo)
{
	digitalWrite(trig,  LOW);
	delayMicroseconds(2);
	digitalWrite(trig, HIGH); // Pulse for 10ms to trigger ultrasonic detection
	delayMicroseconds(10);
	digitalWrite(trig,  LOW);
	long duration = pulseIn(echo, HIGH); // Read receiver pulse time
	long distance = (duration / 2) / 29.1;
	return distance;
}

bool before = 0;
const int observe_thread = 15;
int observe()// 1 for detect the thing moving
{
	dis1 = trigerSend(TRIG_PIN1, ECHO_PIN1);
	dis2 = trigerSend(TRIG_PIN2, ECHO_PIN2);

	if ( (dis1 < observe_thread || dis2 < observe_thread ) && !before)
	{
		// Serial.println("stop");
		stopmove();
		delay(2000);

		for (int i = 0; i < 50; ++i)
		{
			long  disA = trigerSend(TRIG_PIN1, ECHO_PIN1),
			      disB = trigerSend(TRIG_PIN2, ECHO_PIN2);

			if ( (dis1 < observe_thread && abs(disA - dis1) > 4) ||
			     (dis2 < observe_thread && abs(disB - dis2) > 4) )
			{
				//  Serial.println("slow");
				before = 0;
				return 1;
			}
			delay(10);

		}
		// Serial.println("Ignore");
		before = 1;
		return 0;
	}
	else if ( (dis1<observe_thread || dis2<observe_thread ) && before )
	{
		// Serial.println("Ignore");
		return 0;
	}
	else
	{
		// Serial.println("Normal");
		before = 0;
		return 0;
	}
}


/* line status
 * -1 for change to sonic_linetrack
 * 0 for no line detect
 * 1 for left  detect line
 * 2 for right detect line
 * 3 for left just out of line
 * 4 for right just out of line
 */

int linetrack() 
{
	AIR = analogRead(A0);
	BIR = analogRead(A1);
	CIR = analogRead(A2);
	DIR = analogRead(A3);
	if (AIR>500 && BIR>500)
		return -1;// on
	if (CIR>500 && DIR<500) 
		return 1;
	else if (DIR > 500 && CIR < 500) 
		return 2;
	else 
		return 0;
};

int sonic_linetrack() 
{
	dis3 = trigerSend(TRIG_PIN3, ECHO_PIN3);
	dis4 = trigerSend(TRIG_PIN4, ECHO_PIN4);
	if (dis3 > 7 && dis4<7)
		return 1;
	else if (dis4 > 7 && dis3<7)
		return 2;
	else
		return 0;
};

int linestatus = 0;
void loop()
{
	/*
	// ultra sonic sensor
	Serial.print(dis1);Serial.print("  ");// front left
	Serial.print(dis2);Serial.print("  ");// front right
	Serial.print(dis3);Serial.print("  ");// left
	Serial.print(dis4);Serial.print("  ");// right
	Serial.println("-------");

	// IR sensor
	Serial.print(AIR );Serial.print("  ");// outside left
	Serial.print(BIR );Serial.print("  ");// outside right
	Serial.print(CIR );Serial.print("  ");// inside left
	Serial.print(DIR );Serial.print("  ");// inside right
	Serial.println("-------------------");
	*/
	
	// detect the object
	int rspeedl = record_speedl,
		rspeedr = record_speedr,obs=0;
	while( observe() ){++obs;}
	if(obs)
		realGo(rspeedl,rspeedr);
	
	// line track
	int ls = linetrack();
	if(ls==-1)
		ls = sonic_linetrack();

	// get step of track
	if(ls==0)
	{
		switch(linestatus)
		{
			case 0:
				forward();break;
			case 1:
				right();
				linestatus = 3;break;
			case 2:
				left();
				linestatus = 4;break;
			case 3:case 4:
				forward();
				linestatus = 0;break;
		}
	}
	else
	{
		switch(linestatus)
		{
			case 0:
				backoff();break;
			case 1:break;
			case 2:break;//road too small
			case 3:
				  backoff();break;
			case 4:break;//road too small
		}
		linestatus = ls;
	}
	delay(150);
}
