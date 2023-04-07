#include <Car_Library.h>

#
const int analog_Pin = A5;
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  intval;
  
  val = potentiometer_Read(analogPin);
  
  Serial.print(val);
  
  analogWrite(LED_BULITIN, val);
}
