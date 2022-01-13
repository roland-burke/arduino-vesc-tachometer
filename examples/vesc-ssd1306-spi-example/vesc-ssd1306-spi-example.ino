#include <ssd1306.h>
#include <ssd1306_fonts.h>
#include <SPI.h>
#include <VescUart.h>

#define RESET_PIN 10
#define CES_PIN 8
#define DC_PIN 9

VescUart UART;

static void textDemo()
{
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen();
    ssd1306_printFixed(0,  8, "Normal text", STYLE_NORMAL);
    ssd1306_printFixed(0, 16, "Bold text", STYLE_BOLD);
    ssd1306_printFixed(0, 24, "Italic text", STYLE_ITALIC);
    ssd1306_negativeMode();
    ssd1306_printFixed(0, 32, "Inverted bold", STYLE_BOLD);
    ssd1306_positiveMode();
    delay(3000);
}


void setup()
{
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_128x64_spi_init(RESET_PIN, CES_PIN, DC_PIN);

    ssd1306_clearScreen();

    Serial.begin(115200);
    UART.setSerialPort(&Serial);
}

void loop()
{
  textDemo();

  delay(2000);
  ssd1306_clearScreen();

  if ( UART.getVescValues() ) {
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
    ssd1306_printFixed(0,  8, "Error", STYLE_NORMAL);
  }

  delay(2000);
  ssd1306_clearScreen();
}
