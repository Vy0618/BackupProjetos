 #include <Arduino.h>
#include <ESP32Servo.h>

const int ESC_PIN1 = 12;
const int ESC_PIN2 = 14;

Servo esc1;
Servo esc2;


int pctParaAngulo(int pct) {
  pct = constrain(pct, 0, 100);
  return map(pct, 0, 100, 0, 180);
}

void parar(){
    esc1.write(0);
    esc2.write(0);
   
}

void avancar(){
    int velocidade = pctParaAngulo(40);
    esc1.write(velocidade);
    esc2.write(velocidade);
}

void setup() {
  Serial.begin(115200);

  esc1.attach(ESC_PIN1, 1000, 2000);
  esc2.attach(ESC_PIN2, 1000, 2000);

  parar();
  delay(3000);
 
}

void loop() {
    
    avancar();
    delay(5000);
    parar();
    delay(5000);
    
}
