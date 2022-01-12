#include <ssd1306.h>
#include <ssd1306_fonts.h>
#include <SPI.h>
#include <VescUart.h>

// button
#define BUTTON_PIN 4

// ui
#define MAIN_Y_POS 22

// calculations
#define POLE_PAIRS 21
#define WHEEL_DIAMETER 0.72 // in meter
#define MOTOR_PULLEY 16.0
#define WHEEL_PULLEY 185.0
#define MIN_VOLTAGE 36
#define NUM_OF_CELLS 12

VescUart UART;

const char* const PROGMEM timeFormat = "%.2d:%.2ld";
const char* const PROGMEM batteryFormat = "%.1f%%";
const char* const PROGMEM notAvailable = "n.A.";
const char* const PROGMEM speedTitle = "SPEED";
const char* const PROGMEM tempTitle = "TEMP";
const char* const PROGMEM statusTitle = "STATUS";
const char* const PROGMEM timeTitle = "TIME";
const char* const PROGMEM tripTitle = "TRIP";
const char* const PROGMEM errorStr = "Error";
const char* const PROGMEM kmhString = "KMH";
const char* const PROGMEM gradString = "GRAD C";
const char* const PROGMEM kmString = "KM";
const char* const PROGMEM speedFormat = "%.1f%%";
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

    Serial.begin(11520);
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

        float battery = ((voltage - 38.4) / 12) * 100;

        char PROGMEM batteryString[10];
        sprintf(batteryString, batteryFormat, battery);

        ssd1306_printFixed(95, 2, batteryString, STYLE_NORMAL);
      } else {
        //Serial.println("Failed to get data");
        ssd1306_printFixed(95,  0, notAvailable, STYLE_NORMAL);
      }
}

void showSpeed() {
  if ( UART.getVescValues() ) {
        
        int rpm = (UART.data.rpm) / POLE_PAIRS;

        Serial.println(rpmString + String(UART.data.rpm));

        float velocity = rpm * PI * (60.0 / 1000.0) * WHEEL_DIAMETER * (MOTOR_PULLEY / WHEEL_PULLEY);

        char PROGMEM speedString[10];
        sprintf(speedString, speedFormat, velocity);

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
  ssd1306_setFixedFont(comic_sans_font24x32_123);
  ssd1306_printFixed(0, MAIN_Y_POS, "56", STYLE_BOLD);
  ssd1306_setFixedFont(ssd1306xled_font8x16);
  ssd1306_printFixed(80, 53, gradString, STYLE_BOLD);
}

void showTrip() {
  ssd1306_setFixedFont(comic_sans_font24x32_123);
  ssd1306_printFixed(0, MAIN_Y_POS, "23.4", STYLE_BOLD);
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

  if (lastButtonState == 0 && buttonState == 1) {
    changeState();
    ssd1306_clearScreen();
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
    showTitle(statusTitle);
    if ( UART.getVescValues() ) {
        char PROGMEM inpVoltage[6];
        char PROGMEM rpm[6];
        char PROGMEM ampHours[6];
        char PROGMEM tachometerAbs[6];
        itoa(UART.data.inpVoltage, inpVoltage, 10);
        itoa(UART.data.rpm, rpm, 10);
        itoa(UART.data.ampHours, ampHours, 10);
        itoa(UART.data.tachometerAbs, tachometerAbs, 10);
        
        ssd1306_printFixed(0,  16, "inpVolt:", STYLE_NORMAL);
        ssd1306_printFixed(0,  24, "rpm:", STYLE_NORMAL);
        ssd1306_printFixed(0,  32, "ampHours:", STYLE_NORMAL);
        ssd1306_printFixed(0,  40, "tachoAbs:", STYLE_NORMAL);
        ssd1306_printFixed(90,  16, inpVoltage, STYLE_NORMAL);
        ssd1306_printFixed(90,  24, rpm, STYLE_NORMAL);
        ssd1306_printFixed(90,  32, ampHours, STYLE_NORMAL);
        ssd1306_printFixed(90,  40, tachometerAbs, STYLE_NORMAL);
      } else {
          //Serial.println("Failed to get data");
          ssd1306_printFixed(0,  30, errorStr, STYLE_NORMAL);
      } 
  }

  delay(100);
    
}




