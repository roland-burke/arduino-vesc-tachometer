#include <SPI.h>

#define BUTTON_PIN 4

int controlState = 0;
int nStates = 3;

int buttonState = 0;
int lastButtonState = 0;

void changeState() {
  if(controlState == nStates - 1) {
    controlState = 0;
  } else {
    controlState++;
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT);

}

void loop()
{

int buttonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == 0 && buttonState == 1) {
    changeState();
  }

  if(controlState == 0) {
    Serial.println("State 0");  
  } else if (controlState == 1) {
    Serial.println("State 1");  
  } else {
    Serial.println("State 2");  
  }

  delay(100);
}