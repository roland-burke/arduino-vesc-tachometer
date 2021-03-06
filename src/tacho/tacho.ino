#include <ssd1306.h>
#include <SPI.h>
#include <VescUart.h>
#include "chakra14pt.h" // Unit
#include "chakra50pt.h" // main
#include "chakra10pt.h" // title
#include "chakra35pt.h" // main decimal
#include "chakra32pt.h" // time

#define BUTTON_UP 0
#define BUTTON_DOWN 1

// Pins
#define BUTTON_PIN 4
#define RESET_PIN 10
#define CES_PIN 8
#define DC_PIN 9

// Ui
#define MAIN_Y_POS 23
#define DISPLAY_REFRESH_RATE 5 // 5ms

// Calculations
#define POLES 46.0
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
const char* const PROGMEM notAvailable = "N.A.";

// Units
const char* const PROGMEM kmhString = "KMH";
const char* const PROGMEM gradString = "°C";
const char* const PROGMEM kmString = "KM";

// Menu State Management
const int PROGMEM nStates = 5;

int controlState = 0;
int buttonState = 0;
int lastButtonState = 0;

// Refresh Rate Management
const int PROGMEM updateDelay = 700; //ms
int delayCount = updateDelay + 1;

void setup()
{
    ssd1306_128x64_spi_init(RESET_PIN, CES_PIN, DC_PIN);
    ssd1306_clearScreen();

    initialize();

    Serial.begin(115200);
    UART.setSerialPort(&Serial);

    // setup button
    pinMode(BUTTON_PIN, INPUT);
}

const  uint8_t logo [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xf0, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xe0, 0xf8, 0xff, 0xff, 0xff, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xe0, 0xf8, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0x70, 0x10, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x08, 0x0e, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0xff, 
	0xff, 0xff, 0xff, 0xf0, 0xf0, 0xf0, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 
	0xff, 0xff, 0x7f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 
	0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void initialize() {
  ssd1306_drawBitmap(0, 0, 128, 64, logo);
  delay(3000);
  ssd1306_clearScreen();
}

void changeState() {
  if(controlState == nStates - 1) {
    controlState = 0;
  } else {
    controlState++;
  }
}

char* getTime(char* timeString) {
  unsigned long myTime = millis();
  unsigned long seconds = myTime / 1000;
  int minutes = seconds / 60;

  sprintf(timeString, timeFormat, minutes % 60, seconds % 60);

  return timeString;
}

float getBatteryPercentage() {
  return ((UART.data.inpVoltage - BAT_MIN_VOLTAGE) / (BAT_MAX_VOLTAGE - BAT_MIN_VOLTAGE)) * 100;
}

float getSpeed() {
  int rpm = (UART.data.rpm) / (POLES / 2);
        
  // We do not need to pay attention to the translation because it is 1 to 1.
  float velocity = abs(rpm) * PI * (60.0 / 1000.0) * WHEEL_DIAMETER;

  if (velocity > 99) {
    return 99.0;
  }
  return velocity;
}

float getTrip() {
  float tach = (UART.data.tachometerAbs) / (POLES * 3.0);
        
  // We do not need to pay attention to the translation because it is 1 to 1.
  float distance = tach * PI * (1.0/1000.0) * WHEEL_DIAMETER;

  if (distance > 99) {
    return 99.0;
  }
  return distance;
}

void showTitle(const char* text) {
  ssd1306_setFixedFont(ChakraPetch_SemiBold11x14 );
  ssd1306_printFixed(0, 0, text, STYLE_NORMAL);
}

void showBattery() {
  ssd1306_setFixedFont(ChakraPetch_SemiBold11x14);
  if ( UART.getVescValues() ) {
      char batteryString[5];
      dtostrf(getBatteryPercentage(), 3, 0, batteryString);
      strcat(batteryString, "%");        
      ssd1306_printFixed(80, 2, batteryString, STYLE_NORMAL);
    } else {
      ssd1306_printFixed(80,  0, notAvailable, STYLE_NORMAL);
    }
}

void showSpeed() {
  if ( UART.getVescValues() ) {
      char speedString[4];
      dtostrf(getSpeed(), 2, 0, speedString);

      ssd1306_setFixedFont(ChakraPetch_Bold41x46);
      ssd1306_printFixed(0, MAIN_Y_POS, speedString, STYLE_BOLD);
    } else {
        ssd1306_printFixed(0,  MAIN_Y_POS, notAvailable, STYLE_NORMAL);
    }
  
  ssd1306_setFixedFont(ChakraPetch_SemiBold16x19);
  ssd1306_printFixed(80, 49, kmhString, STYLE_NORMAL);
}

void showTemperature() {
  if ( UART.getVescValues() ) {
      char tempMotorString[4];
      dtostrf(UART.data.tempMotor, 2, 0, tempMotorString);

      ssd1306_setFixedFont(ChakraPetch_Bold41x46);
      ssd1306_printFixed(0, MAIN_Y_POS, tempMotorString, STYLE_BOLD);
    } else {
        ssd1306_printFixed(0,  MAIN_Y_POS, notAvailable, STYLE_NORMAL);
    }
  ssd1306_setFixedFont(ChakraPetch_SemiBold16x19);
  ssd1306_printFixed(80, 49, gradString, STYLE_BOLD);
}

void showTrip() {
  if ( UART.getVescValues() ) {
      char distanceString[5];
      dtostrf(getTrip(), 4, 1, distanceString);

      ssd1306_setFixedFont(ChakraPetch_Bold28x32);
      ssd1306_printFixed(0, MAIN_Y_POS, distanceString, STYLE_BOLD);
    } else {
      ssd1306_printFixed(0,  MAIN_Y_POS, notAvailable, STYLE_NORMAL);
    }

  ssd1306_setFixedFont(ChakraPetch_SemiBold16x19);
  ssd1306_printFixed(80, 49, kmString, STYLE_NORMAL);
}

void showTime() {
  char timeString[7] = "";
  getTime(timeString);
  ssd1306_setFixedFont(ChakraPetch_Bold25x30);
  ssd1306_printFixed(0, MAIN_Y_POS + 5, timeString, STYLE_NORMAL);
}

void printInfo(const char* inpVoltage, const char* rpm, const char* avgInpCurrent, const char* tachometerAbs, const char* ampHours) {
  ssd1306_setFixedFont(ssd1306xled_font6x8);
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
}

void showInfo() {
  if ( UART.getVescValues() ) {
      printInfo("     ", "     ", "     ", "     ", "     ");
      char inpVoltage[6];
      char rpm[6];
      char avgInpCurrent[6];
      char tachometerAbs[6];
      char ampHours[6];
      itoa(UART.data.inpVoltage, inpVoltage, 10);
      itoa(UART.data.rpm, rpm, 10);
      itoa(UART.data.avgInputCurrent, avgInpCurrent, 10);
      itoa(UART.data.tachometerAbs, tachometerAbs, 10);
      itoa(UART.data.ampHours, ampHours, 10);
      
      printInfo(inpVoltage, rpm, avgInpCurrent, tachometerAbs, ampHours);
    } else {
      printInfo(notAvailable, notAvailable, notAvailable, notAvailable, notAvailable);
    }
}

void loop()
{
  int buttonState = digitalRead(BUTTON_PIN);

  // Control the button press, so that it doesn't keep switching if you keep the button pressed
  if (lastButtonState != buttonState) {
    if (lastButtonState == BUTTON_UP && buttonState == BUTTON_DOWN) {
      changeState();
      ssd1306_clearScreen();
      delayCount = updateDelay + 1; // make it bigger to trigger an update
    }
    lastButtonState = buttonState;
  }

  // Skip display update if delay is not reached
  if (delayCount > updateDelay) {
    showBattery();
    if(controlState == 0) {
      showTitle(speedTitle);
      showSpeed();
    } else if(controlState == 1) {
      showTitle(tempTitle);
      showTemperature();
    } else if (controlState == 3) {
      showTitle(tripTitle);
      showTrip();
    } else if (controlState == 4) {
      showTitle(infoTitle);
      showInfo();
    }
    delayCount = 0;
  }

  // exclude time from the if branching, because it updates itself just every second
  if (controlState == 2) {
      showTitle(timeTitle);
      showTime();
  }
  
  delay(DISPLAY_REFRESH_RATE); // 0.005s
  delayCount = delayCount + DISPLAY_REFRESH_RATE;
}
