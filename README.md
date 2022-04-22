# Techometer for E-Bike

Basic overview of the tachometer controlled by arduino nano. It shows Speed (kmh), Motor Temperature (°C), Distance driven (km), (kmh), time elapsed and Battery Percentage. It also shows some raw output from the VESC.

With the button you can switch between the different information displayed.

## Libraries used
- https://github.com/lexus2k/ssd1306 (MIT License)
- https://github.com/SolidGeek/VescUart (GNU v3.0 License)

The ssd1306 is great, becuase it needs very little RAM.

## Hardware used
- Arduino Nano (Atmega328p)
- SSD1306 (128x64) Display
- Button

## Connections

The used protocol for communicating with the display is SPI. A and B are the opposite connctions on the button.

	Display - Arduino

	GND  -->  GND
	VDD  -->  3v3
	SCK  -->  SCK (D13)
	SDA  -->  MOSI (D11)
	RES  -->  SS (D10)
	DC   -->  D9
	CS   -->  D8

	Vesc - Arduino

	5v   -->  5v
	GND  -->  GND
	TX   -->  RX (D1)
	RX   -->  TX (D0)

	Button - Arduino

	A1   -->  GND (with 10k resistance)
	A2   -->  D4
	B1   -->  5v5
