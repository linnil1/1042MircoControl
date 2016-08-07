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
bool before = 0;
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

void forward() {
	analogWrite(motorlA, 0     );
	analogWrite(motorlB, speedl);
	analogWrite(motorrA, 0     );
	analogWrite(motorrB, speedr);
	count_rpm(10);
}
void backoff() {
	analogWrite(motorlA, speedl);
	analogWrite(motorlB, 0     );
	analogWrite(motorrA, speedr);
	analogWrite(motorrB, 0     );
	count_rpm(10);
}
void right() {
	analogWrite(motorlA, 0  );
	analogWrite(motorlB, 70 );
	analogWrite(motorrA, 130);
	analogWrite(motorrB, 0  );
}
void left() {
	analogWrite(motorlA, 130);
	analogWrite(motorlB, 0  );
	analogWrite(motorrA, 0  );
	analogWrite(motorrB, 70 );
}
void stopmove() {
	analogWrite(motorlA, 0);
	analogWrite(motorlB, 0);
	analogWrite(motorrA, 0);
	analogWrite(motorrB, 0);
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

long beforedis1, beforedis2;
int observe()
{
	long  dis1 = trigerSend(TRIG_PIN1, ECHO_PIN1),
	      dis2 = trigerSend(TRIG_PIN2, ECHO_PIN2);

	if (dis1 < 8 && dis2 < 8) {
		return 0;
	}
	else if ( (dis1 < 8 || dis2 < 8) && !before)
	{
		// Serial.println("stop");
		stopmove();
		delay(2000);

		for (int i = 0; i < 50; ++i)
		{
			long  disA = trigerSend(TRIG_PIN1, ECHO_PIN1),
			      disB = trigerSend(TRIG_PIN2, ECHO_PIN2);

			if ( (dis1 < 8 && abs(disA - dis1) > 4) ||
			     (dis2 < 8 && abs(disB - dis2) > 4) )
			{
				//  Serial.println("slow");
				before = 1;
				beforedis1 = dis1;
				beforedis2 = dis2;
				return 1;
			}

		}
		// Serial.println("Ignore");
		before = 1;
		beforedis1 = dis1;
		beforedis2 = dis2;
		return 2;
	}
	else if ( (dis1 < 8 ? beforedis1 < 8 : 1) && 
	          (dis2 < 8 ? beforedis2 < 8 : 1) && before)
	{
		beforedis1 = dis1; beforedis2 = dis2;
		// Serial.println("Continue");
		return 3;
	}


	else
	{
		before = 0;
		beforedis1 = dis1; 
		beforedis2 = dis2;
		// Serial.println("Normal");
		return 0;
	}
}


/* line status
 * 0 for no line detect
 * 1 for left  detect line
 * 2 for right detect line
 * 3 for left just out of line
 * 4 for right just out of line
 */

int linetrack() 
{
//	AIR = analogRead(A0);
//	BIR = analogRead(A1);
	AIR = BIR = 0;
	CIR = analogRead(A2);
	DIR = analogRead(A3);
	if (AIR < 500 && BIR < 500)
	{
		if (CIR > 500 && DIR < 500) 
			return 1;

		else if (DIR > 500 && CIR < 500) 
			return 2;
		else
			return 0;
	}
	return -1;
};

int sonic_linetrack() 
{
	long dis3 = trigerSend(TRIG_PIN3, ECHO_PIN3);
	long dis4 = trigerSend(TRIG_PIN4, ECHO_PIN4);
	Serial.println(dis3);
	Serial.println(dis4);
	Serial.println("--");
	if (dis3 > 7)
		return 1;
	else if (dis4 > 7 )
		return 2;
	else
		return 0;
};

int linestatus = 0;
void loop()
{
	/*
	while(true)
	{
		long  dis1 = trigerSend(TRIG_PIN1,ECHO_PIN1),
			  dis2 = trigerSend(TRIG_PIN2,ECHO_PIN2);
		long  dis3 = trigerSend(TRIG_PIN3,ECHO_PIN3),
			  dis4 = trigerSend(TRIG_PIN4,ECHO_PIN4);
		Serial.println(dis1);
		Serial.println(dis2);
		Serial.println(dis3);
		Serial.println(dis4);
		Serial.println("--");
		delay(100);
	}
	*/
	int ls = sonic_linetrack();
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
	delay(50);
	/*
	int barrier = observe();
	if (barrier == 0)
		linetrack(), delay(10);
	else if (barrier == 1)
		linetrack();
	else if (barrier == 2)
		linetrack();
	else if (barrier == 3)
		for (int i = 0; i <= 100; i++) {
			linetrack();
		}
	*/

/*


	Serial.print(AIR);
	Serial.print("  ");
	Serial.print(BIR);
	Serial.print("  ");
	Serial.print(CIR);
	Serial.print("  ");
	Serial.println(DIR);

  if (Serial.available() > 0) {
    char i = Serial.read();
    if (i == '1') {
      forward(200);
      delay(2000);
    }
    else if (i == '2') {
      right(200);
      delay(2000);
    }
    else if (i == '3') {
      left(200);
      delay(2000);
    }
    else if (i == '4') {
      stopmove();
      delay(2000);
    }
    }*/
}
