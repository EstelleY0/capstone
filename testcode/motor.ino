#include <Car_Library.h>

const int mA1 = 3;
const int mA2 = 4;

void setup() {
  Serial.begin(115200);
  pinMode(mA1, OUTPUT);
  pinMode(mA2, OUTPUT);
}

void loop() {
  // Forward
  motor_forward(mA1, mA2, 75);
  delay(10000);
  
  // Backward
  motor_backward(mA1, mA2, 75);
  delay(10000);
  
  // Hold
  motor_hold(mA1, mA2);
  delay(1000);
}
