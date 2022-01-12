#include <ssd1306.h>
#include <ssd1306_fonts.h>
#include <SPI.h>
#include <VescUart.h>

// Button
#define BUTTON_PIN 4

// Ui
#define MAIN_Y_POS 22

// Calculations
#define POLES 46
#define WHEEL_DIAMETER 0.72 // in meter
#define BAT_MIN_VOLTAGE 48.0 // Empty
#define BAT_MAX_VOLTAGE 67.2 // Full

VescUart UART;

// String formats
const char* const PROGMEM timeFormat = "%.2d:%.2ld";

//Titles
const char* const PROGMEM speedTitle = "SPEED";
const char* const PROGMEM tempTitle = "TEMP";
const char* const PROGMEM infoTitle = "INFO";
const char* const PROGMEM timeTitle = "TIME";
const char* const PROGMEM tripTitle = "TRIP";

// Messages
const char* const PROGMEM notAvailable = "n.A.";
const char* const PROGMEM errorStr = "Error";

// Units
const char* const PROGMEM kmhString = "KMH";
const char* const PROGMEM gradString = "GRAD C";
const char* const PROGMEM kmString = "KM";
const char* const PROGMEM rpmString = "RPM: ";


// Menu State Management
int controlState = 0;
int nStates = 5;

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
    // Select the font to use with menu and all font functions
    ssd1306_setFixedFont(ssd1306xled_font6x8);

    //ssd1306_128x64_i2c_init();
//    ssd1306_128x64_spi_init(-1, 0, 1);  // Use this line for nano pi (RST not used, 0=CE, gpio1=D/C)
    ssd1306_128x64_spi_init(10,8,9);     // Use this line for Atmega328p (3=RST, 4=CE, 5=D/C)
//    ssd1306_128x64_spi_init(24, 0, 23); // Use this line for Raspberry  (gpio24=RST, 0=CE, gpio23=D/C)
//    ssd1306_128x64_spi_init(22, 5, 21); // Use this line for ESP32 (VSPI)  (gpio22=RST, gpio5=CE for VSPI, gpio21=D/C)
//    composite_video_128x64_mono_init(); // Use this line for ESP32 with Composite video support

    ssd1306_clearScreen();

    Serial.begin(115200);
    UART.setSerialPort(&Serial);

    // setup button
    pinMode(BUTTON_PIN, INPUT);
    
}

char* getTime(char* timeString) {
  unsigned long myTime = millis();
  unsigned long seconds = myTime / 1000;
  int minutes = seconds / 60;
  //int hours = minutes / 60;

  sprintf(timeString, timeFormat, minutes % 60, seconds % 60);

  return timeString;
}

void showTitle(const char* text) {
  ssd1306_setFixedFont(ssd1306xled_font8x16 );
  ssd1306_printFixed(0, 0, text, STYLE_NORMAL);
}

void showBattery() {

  ssd1306_setFixedFont(ssd1306xled_font8x16 );
  if ( UART.getVescValues() ) {
        
        float voltage = (UART.data.inpVoltage);

        Serial.println("Voltage: " + String(voltage));

        float batteryPercent = ((voltage - BAT_MIN_VOLTAGE) / (BAT_MAX_VOLTAGE - BAT_MIN_VOLTAGE)) * 100;

        char PROGMEM batteryString[5];
        dtostrf(batteryPercent, 3, 0, batteryString);
        strcat(batteryString, "%");        
        ssd1306_printFixed(88, 2, batteryString, STYLE_NORMAL);
      } else {
        //Serial.println("Failed to get data");
        ssd1306_printFixed(95,  0, notAvailable, STYLE_NORMAL);
      }
}

void showSpeed() {
  if ( UART.getVescValues() ) {
        
        int rpm = (UART.data.rpm) / (POLES / 2);

        Serial.println(rpmString + String(UART.data.rpm));
        
        // We do not need to pay attention to the translation because it is 1 to 1.
        float velocity = abs(rpm) * PI * (60.0 / 1000.0) * WHEEL_DIAMETER;

        if (velocity > 999) {
          velocity = 999;
        } 

        char PROGMEM speedString[4];
        dtostrf(velocity, 3, 0, speedString);

        ssd1306_setFixedFont(comic_sans_font24x32_123);
        ssd1306_printFixed(0, MAIN_Y_POS, speedString, STYLE_BOLD);
      } else {
          //Serial.println("Failed to get data");
          ssd1306_printFixed(0,  MAIN_Y_POS, notAvailable, STYLE_NORMAL);
      }
  
  ssd1306_setFixedFont(ssd1306xled_font8x16);
  ssd1306_printFixed(104, 53, kmhString, STYLE_NORMAL);
}

void showTemperature() {
  if ( UART.getVescValues() ) {
        float tempMotor = UART.data.tempMotor;

        char PROGMEM tempMotorString[4];
        dtostrf(tempMotor, 3, 0, tempMotorString);

        ssd1306_setFixedFont(comic_sans_font24x32_123);
        ssd1306_printFixed(0, MAIN_Y_POS, tempMotorString, STYLE_BOLD);
      } else {
          //Serial.println("Failed to get data");
          ssd1306_printFixed(0,  MAIN_Y_POS, notAvailable, STYLE_NORMAL);
      }
  ssd1306_setFixedFont(ssd1306xled_font8x16);
  ssd1306_printFixed(80, 53, gradString, STYLE_BOLD);
}

void showTrip() {

  if ( UART.getVescValues() ) {
        
        float tach = (UART.data.tachometerAbs) / POLES * 3;

        Serial.println(rpmString + String(UART.data.rpm));
        
        // We do not need to pay attention to the translation because it is 1 to 1.
        float distance = tach * PI * (1.0/1000.0) * WHEEL_DIAMETER;

        if (distance > 99) {
          distance = 99;
        } 

        char PROGMEM distanceString[5];
        dtostrf(distance, 2, 1, distanceString);

        ssd1306_setFixedFont(comic_sans_font24x32_123);
        ssd1306_printFixed(0, MAIN_Y_POS, distanceString, STYLE_BOLD);
      } else {
          //Serial.println("Failed to get data");
          ssd1306_printFixed(0,  MAIN_Y_POS, notAvailable, STYLE_NORMAL);
      }

  ssd1306_setFixedFont(ssd1306xled_font8x16 );
  ssd1306_printFixed(108, 53, kmString, STYLE_NORMAL);
}

void showTime() {
  char PROGMEM timeString[7] = "";
  getTime(timeString);
  ssd1306_setFixedFont(comic_sans_font24x32_123 );
  ssd1306_printFixed(0, MAIN_Y_POS, timeString, STYLE_NORMAL);
}

void loop()
{
  int buttonState = digitalRead(BUTTON_PIN);

  if (lastButtonState != buttonState) {
    if (lastButtonState == 0 && buttonState == 1) {
      changeState();
      ssd1306_clearScreen();
    }
    lastButtonState = buttonState;
  }

  showBattery();

  if(controlState == 0) {
    showTitle(speedTitle);
    showSpeed();
  } else if(controlState == 1) {
    showTitle(tempTitle);
    showTemperature();
  } else if (controlState == 2) {
    showTitle(timeTitle);
    showTime();
  } else if (controlState == 3) {
    showTitle(tripTitle);
    showTrip();
  } else {
    showTitle(infoTitle);
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    if ( UART.getVescValues() ) {
        char PROGMEM inpVoltage[6];
        char PROGMEM rpm[6];
        char PROGMEM avgInpCurrent[6];
        char PROGMEM tachometerAbs[6];
        char PROGMEM ampHours[6];
        itoa(UART.data.inpVoltage, inpVoltage, 10);
        itoa(UART.data.rpm, rpm, 10);
        itoa(UART.data.avgInputCurrent, avgInpCurrent, 10);
        itoa(UART.data.tachometerAbs, tachometerAbs, 10);
        itoa(UART.data.ampHours, ampHours, 10);
        
        ssd1306_printFixed(0,  16, "inpVolt:", STYLE_NORMAL);
        ssd1306_printFixed(0,  24, "rpm:", STYLE_NORMAL);
        ssd1306_printFixed(0,  32, "avgInpCurr:", STYLE_NORMAL);
        ssd1306_printFixed(0,  40, "tachoAbs:", STYLE_NORMAL);
        ssd1306_printFixed(0,  48, "ampHours:", STYLE_NORMAL);
        ssd1306_printFixed(88,  16, inpVoltage, STYLE_NORMAL);
        ssd1306_printFixed(88,  24, rpm, STYLE_NORMAL);
        ssd1306_printFixed(88,  32, avgInpCurrent, STYLE_NORMAL);
        ssd1306_printFixed(88,  40, tachometerAbs, STYLE_NORMAL);
        ssd1306_printFixed(88,  48, ampHours, STYLE_NORMAL);
      } else {
          //Serial.println("Failed to get data");
          ssd1306_printFixed(0,  30, errorStr, STYLE_NORMAL);
      } 
  }

  delay(10);
    
}




