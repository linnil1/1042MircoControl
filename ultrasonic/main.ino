enum {
	ECHO_PIN1 = 7 ,
	TRIG_PIN1 = 8 ,
	ECHO_PIN2 = 12,
	TRIG_PIN2 = 13
}; 

bool before=0;
void setup()
{
	Serial.begin(38400);
	pinMode(ECHO_PIN1,  INPUT);
	pinMode(TRIG_PIN1, OUTPUT);
	pinMode(ECHO_PIN2,  INPUT);
	pinMode(TRIG_PIN2, OUTPUT);
	before = 0;
}

long trigerSend(int trig,int echo)
{
	digitalWrite(trig, LOW);
	delayMicroseconds(2);
	digitalWrite(trig, HIGH); // Pulse for 10ms to trigger ultrasonic detection
	delayMicroseconds(10);
	digitalWrite(trig, LOW);
	long duration = pulseIn(echo, HIGH); // Read receiver pulse time
	long distance = (duration / 2) / 29.1;
	return distance;
}


/*
 * if find the object :
 *   stop
 *   if move:
 *     slow
 *   else:
 *     ignore
 */

/* status code 
0:Continue
1:slow
2:ignore
*/

int observe()
{
	long  dis1 = trigerSend(TRIG_PIN1,ECHO_PIN1),
	      dis2 = trigerSend(TRIG_PIN2,ECHO_PIN2);
	
	
	if( (dis1 < 20 || dis2 < 20) && !before)
	{
		Serial.println("stop");
		//motor stop
		delay(1000);

		bool isslow=0;
		for(int i=0;i<50;++i)
		{
			long  disA = trigerSend(TRIG_PIN1,ECHO_PIN1),
				  disB = trigerSend(TRIG_PIN2,ECHO_PIN2);

			if ( abs(disA-dis1) > 3 || abs(disB-dis2) > 3)
			{
				Serial.println("slow");
				//motor slow
				return 1;
				before = 1;
				isslow = 1;
				break;
			}
			delay(10);
		}
		if (!isslow)
		{
			Serial.println("Ignore");
			//motor go
			return 2;
			before = 1;
		}
//		delay(1000);//for debug use
	}
	else if ( (dis1 < 20 || dis2 < 20) && before)
	{
		Serial.println("Continue");
		return 0;
	}
	else
	{
		before=0;
		Serial.println("Normal");
		return 0;
		//motor go
	}
	
	Serial.println(dis1);
	Serial.println(dis2);
	Serial.println("--");
	delay(100);

}

void loop()
{
	code = observe();
}
