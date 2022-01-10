# Techometer for E-Bike

Basic overview of the tachometer controlled by arduino nano.

## Connections

Hardware used:
- Arduino Nano (Atmega328p)
- SSD1306 (182x64) Display

Display		Arduino

	GND		-->	GND
	VDD		--> 3v3
	SCK		-->	SCK (D13)
	SDA		--> MOSI (D12)
	RES		--> SS (D10)
	DC		--> D9
	CS 		--> D8