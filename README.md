# Techometer for E-Bike

Basic overview of the tachometer controlled by arduino nano.

## Hardware used
- Arduino Nano (Atmega328p)
- SSD1306 (182x64) Display

## Connections

The used protocol for communicating with the display is SPI

	Display - Arduino

	GND  -->  GND<br>
	VDD  -->  3v3<br>
	SCK  -->  SCK (D13)<br>
	SDA  -->  MOSI (D12)<br>
	RES  -->  SS (D10)<br>
	DC   -->  D9<br>
	CS   -->  D8<br>

	Vesc - Arduino

	5v   -->  5v<br>
	GND  -->  GND<br>
	TX   -->  RX (D1)<br>
	RX   -->  TX (D0)<br>