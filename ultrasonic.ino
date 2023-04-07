#include <Car_Library.h>

//trig and echo pin
int trig = 3;
int echo = 2;

void setup() {
  Serial.begin(115200);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

void loop() {
  long d;
  d = usDistance(trig, echo, 100);
  
  Serial.print(d);
  
  delay(100);
}

float usDistance (int trigPin, int echoPin, duration) {
  d = ((float)(340*duration)/1000)/2;
  return d;
}
