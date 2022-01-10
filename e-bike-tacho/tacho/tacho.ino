
#include <font6x8.h>
#include <nano_engine.h>
#include <nano_gfx.h>
#include <nano_gfx_types.h>
#include <sprite_pool.h>
#include <ssd1306.h>
#include <ssd1306_16bit.h>
#include <ssd1306_1bit.h>
#include <ssd1306_8bit.h>
#include <ssd1306_console.h>
#include <ssd1306_fonts.h>
#include <ssd1306_generic.h>
#include <ssd1306_uart.h>
#include <SPI.h>
#include <VescUart.h>

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
    delay(2000);
}


void setup()
{
    /* Select the font to use with menu and all font functions */
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
}

void loop()
{
    textDemo();

    ssd1306_clearScreen();
    if ( UART.getVescValues() ) {
        char inpVoltage[16];
        char rpm[16];
        char ampHours[16];
        char tachometerAbs[16];

        itoa(UART.data.inpVoltage, inpVoltage, 10);
        itoa(UART.data.rpm, rpm, 10);
        itoa(UART.data.ampHours, ampHours, 10);
        itoa(UART.data.tachometerAbs, tachometerAbs, 10);

        ssd1306_printFixed(0,  8, "inpVoltage:", STYLE_NORMAL);
        ssd1306_printFixed(0,  16, "rpm:", STYLE_NORMAL);
        ssd1306_printFixed(0,  24, "ampHours:", STYLE_NORMAL);
        ssd1306_printFixed(0,  32, "tachometerAbs:", STYLE_NORMAL);

        ssd1306_printFixed(90,  8, inpVoltage, STYLE_NORMAL);
        ssd1306_printFixed(90,  16, rpm, STYLE_NORMAL);
        ssd1306_printFixed(90,  24, ampHours, STYLE_NORMAL);
        ssd1306_printFixed(90,  32, tachometerAbs, STYLE_NORMAL);
    } else {
        //Serial.println("Failed to get data");
        ssd1306_printFixed(0,  8, "Error", STYLE_NORMAL);
    }
    delay(5000);
    ssd1306_clearScreen();
}


