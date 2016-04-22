#include <SoftwareSerial.h> 
#include <Wire.h>

int R1=13,R2=11,enR=12;
int L1=5,L2=6,enL=7;

int powR,powL;

SoftwareSerial BT(10,9); //tx rx


void setup() {
  Serial.begin(9600);
  BT.begin(9600); 
    
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(enR, OUTPUT);
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(enL, OUTPUT);
  digitalWrite(enL,HIGH);
  digitalWrite(enR,HIGH);
  powL=0;
  powR=0;
}

void go(int a,int b,int power){
  if(power<0){
    analogWrite(a, 0);
    analogWrite(b,-power);
  }
  else{
    analogWrite(a, power);
    analogWrite(b,0);
  }
}

void forward(){
  powL = powR = max(powL , powR);
  powL=min(255,powL+10);
  powR=min(255,powR+10);
}
void left(){
  powL=max(-255,powL-30);
  powR=min(255,powR+30);
}
void right(){
  powR=max(-255,powR-30);
  powL=min(255,powL+30);
} 
void slowdown(){
  powL = 0;
  powR = 0;
}
void back(){
   powL = powR = min(powL , powR);
  powL=max(-255,powL-10);
  powR=max(-255,powR-10);
}

void loop() {
  if(BT.available()){
   // Serial.println(BT.read());
    switch(BT.read()){
      case 'a':left();break;
      case 's':slowdown();break;
      case 'd':right();break;
      case 'w':forward();break;
      case 'x':back();break;
    }
  }
  
  go(R1,R2,powR);
  go(L1,L2,powL);
  Serial.print(powR);Serial.print(" ");Serial.println(powL);
  delay(100);

}

