#include <Wire.h>

void setup() {
  // put your setup code here, to run once:
  pinMode (13, OUTPUT);
  Wire.begin(8);
  Wire.onReceive(recieve_cmd);
}

void recieve_cmd (int num) {
  while (Wire.available()) {
    char c = Wire.read();
    if (c == '0') {
      digitalWrite (13, LOW);
    } else if (c == '1') {
      digitalWrite (13, HIGH);      
    }else {
      digitalWrite (13, HIGH);
      delay (1000);
      digitalWrite (13, LOW);
      delay (1000);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  delay (100);
}
