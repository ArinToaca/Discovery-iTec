#include <CalinMotion.h>

CalinMotion cm ;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  cm = CalinMotion();
  cm.Init();
  
  Serial.println("Ready!");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(cm.DoStuff());
  //Serial.println("Ready!");
  delay(500);
}
