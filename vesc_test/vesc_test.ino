#include "config.h"
#include "datatypes.h"
#include "vesc_uart.h"

#include <Wire.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 32, &Wire, 4);

mc_values VescMeasuredValues;

void setup() {
  
  //Serial.begin(115200);
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();


  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setTextSize(2);
}

void loop() {
    Serial.println(F("Hello World"));

    if ( vesc_get_values(VescMeasuredValues) ) {
      Serial.println(F("got values"));
      float voltage = VescMeasuredValues.v_in;
      display.setCursor(0,0);             // Start at top-left corner
      display.println(String(voltage));
      //display.setCursor(30,0);
      //display.println(String(UART.data.rpm));
      //display.setCursor(60,0);
      //display.println(String(UART.data.ampHours));
      //display.setCursor(90,0);
      //display.println(String(UART.data.tachometerAbs));
      
      //display.setCursor(0,12);             // Start at top-left corner
      //display.println(String("rpm:"));
      //display.setCursor(42,12);
      //display.println(String(UART.data.rpm));
        

      //Serial.println(UART.data.rpm);
      //Serial.println(UART.data.inpVoltage);
      //Serial.println(UART.data.ampHours);
      //Serial.println(UART.data.tachometerAbs);

  } else {
    Serial.println("Failed to get data");
    display.setCursor(0,20);
    display.println(String(F("Error getting data")));
  }
  
  display.setCursor(120,24); 
  display.println(String(F("X")));

  display.display();
  delay(1000);
  display.clearDisplay();
}
