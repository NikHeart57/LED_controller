/*
 * ST7789.cpp
 *
 * Created: 15.05.2024 23:13:57
 *  Author: KolyaPC
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "ST7789.h"

namespace ST7789
{
	//==============================================================================
	//				 Функция инициализации SPI, порта и пинов SPI
	//==============================================================================
	
	void init(void)
	{
		// Настройка пинов SPI
		DDR_SPI  |= (1 << MOSI)|(1 << SCK)|(1 << SS)|(0 << MISO);		// Обязательная настройка! Для всех 4-х пинов SPI!
		PORT_SPI |= (1 << MOSI)|(1 << SCK)|(1 << SS)|(1 << MISO);		//
			
		// Настройка реистра SPI
		// Разрешение SPI - (SPE), старший бит вперед - (DORD), мастер - (MSTR), режим ? ((1 << CPHA)|(0 << SPR1)), делитель частоты - 2 (0 = SPR1, 0 = SPR0 + 0 = SPIX) (~7372800 бит/с = 921600байт/с)
		SPCR = (1 << SPE)|(0 << DORD)|(1 << MSTR)|(1 << CPOL)|(1 << CPHA)|(0 << SPR1)|(0 << SPR0);	// (1 << CPHA)|(1 << SPR1) - обязательно для ST7789!!!
		SPSR = (0 << SPI2X);
			
		// Настройка пина RES ST7789
		DDR_RES  |= (1 << RES);											// Выходы (1)
		PORT_RES |= (1 << RES);											// Изначально высокий уровень RES
			
		// Настройка пина защёлки ST7789
		DDR_DC  |= (1 << DC);											// Выходы (1)
		PORT_DC |= (1 << DC);											// Изначально высокий уровень DC
		
		
		// Рестарт экрана
		RESET_HIGH;
		_delay_ms(20);
		RESET_LOW;
		_delay_ms(5);
		RESET_HIGH;
		_delay_ms(20);
		
		
		// Настройка экрана
		sendCommand(0x01);			// Software Reset
		_delay_ms(10);
		sendCommand(0x11);			// Sleep out - проснулись
		_delay_ms(10);
	
		sendCommand(0x3A);			// режим цвета:
		sendData(0x06);				// 18 бит			Write data for 18-bit/pixel (RGB-6-6-6-bit input), 262K-Colors, 3Ah=”06h”
		_delay_ms(10);
	
		sendCommand(0x36);			// (36h) Memory data access control
		sendData(0b00000000);		// 0x20 = 0b00100000; MY0(верх/низ по Y) MX0(право/лево по X) MV1(vertical addressing mode(развертка справа налево или сверху вниз) ML0 RGB0 0 0 0
		_delay_ms(10);
	
		sendCommand(0x2A);			// Column address set
		sendData(0);				// XS highpart	(Старшая часть начального положения Колонки)
		sendData(0);				// XS lowpart	(Младшая часть начального положения Колонки)
		sendData(0);				// XE highpart	(Старшая часть конечного положения Колонки)
		sendData(240);				// XE highpart	(Младшая часть конечного положения Колонки)
		_delay_ms(10);
	
		sendCommand(0x2B);			// Row address set
		sendData(0);				// YS highpart	(Старшая часть начального положения Ряда)
		sendData(0);				// YS lowpart	(Младшая часть начального положения Ряда)
		sendData(0);				// YE highpart	(Старшая часть конечного положения Ряда)
		sendData(240);				// YE highpart	(Младшая часть конечного положения Ряда)
		_delay_ms(10);
	
		sendCommand(0x21);			// Display inversion - on
		_delay_ms(10);
	
		sendCommand(0x29);			// Display on - включаем изображение
		_delay_ms(10);
	}


	//==============================================================================
	//							Функции экрана ST7789
	//==============================================================================
		
	void fillScreen(char red, char green, char blue)
	{
		sendCommand(0x2A);			// Column address set
		sendData(0);				// XS highpart	(Старшая часть начального положения Колонки)
		sendData(0);				// XS lowpart	(Младшая часть начального положения Колонки)
		sendData(240 >> 8);			// XE highpart	(Старшая часть конечного положения Колонки)
		sendData(240);				// XE highpart	(Младшая часть конечного положения Колонки)


		sendCommand(0x2B);			// Row address set
		sendData(0);				// YS highpart	(Старшая часть начального положения Ряда)
		sendData(0);				// YS lowpart	(Младшая часть начального положения Ряда)
		sendData(240 >> 8);			// YE highpart	(Старшая часть конечного положения Ряда)
		sendData(240);				// YE highpart	(Младшая часть конечного положения Ряда)

		sendCommand(0x2C);			// НЕ ПОМНЮ ЧТО ЭТО?
		
		for(long int i = 0; i < 57600; i++)
		{
			sendData(red << 2);
			sendData(green << 2);
			sendData(blue << 2);
		}
	}

	void printString(char string[], char* xpos, char* ypos, char red, char green, char blue, char size)
	{
		char counter = 0;
	
		while (string[counter] != '\0')
		{
			printChar(string[counter], xpos, ypos, red, green, blue, size);
			counter++;
		}
	}
}