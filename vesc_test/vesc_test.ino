#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <VescUart.h>


// Todo: check reset pin: 4 try 0 or -1
Adafruit_SSD1306 display(128, 32, &Wire, 4);

VescUart UART;

void setup() {
  
  //Serial.begin(9600);
  
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();


  /** Define which ports to use as UART */
  UART.setSerialPort(&Serial);

}

void loop() {
    Serial.println(F("Hello World"));
    
    if ( UART.getVescValues() ) {
      
      display.setTextSize(1.2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(0,0);             // Start at top-left corner
      display.println(String(F("volt:")));
      display.setCursor(42,0);
      display.println(String(UART.data.inpVoltage));
      
      //display.setCursor(0,12);             // Start at top-left corner
      //display.println(String("rpm:"));
      //display.setCursor(42,12);
      //display.println(String(UART.data.rpm));
        

      Serial.println(UART.data.rpm);
      //Serial.println(UART.data.inpVoltage);
      Serial.println(UART.data.ampHours);
      //Serial.println(UART.data.tachometerAbs);

  } else {
    //Serial.println("Failed to get data");
    display.println(String(F("Failed to get data!")));
  }
  
  
  display.setTextSize(1.2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0); 
  display.println(String(F("Output")));

  display.display();
  delay(2000);
}
