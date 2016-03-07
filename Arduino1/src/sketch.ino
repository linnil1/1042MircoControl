#include "pitches.h"
int melody[] = {  NOTE_C4, NOTE_D3, NOTE_E3, NOTE_F3, NOTE_G3, NOTE_A3, NOTE_B3};

const int out=3;
void setup() {
  for(int i=13;i>=7;--i)
    pinMode(i,INPUT);
  noTone(out);
}

int here;
bool isinput(){
   for(int i=13;i>=7;--i)
      if( digitalRead(i))
        {here=13-i;return 1;}
  return 0;
}


void loop() {
   if ( isinput() ){
        tone(out, melody[here],250);
        delay(1000);
        noTone(out);
      }
}
