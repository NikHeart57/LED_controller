/*
 * ST7789.cpp
 *
 * Created: 15.05.2024 23:13:57
 *  Author: KolyaPC
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "ST7789.h"


//==============================================================================
//				  Функции инициализации SPI и инициализации экрана
//==============================================================================
	
void ST7789_InitSPI(void)
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
}
	
void ST7789_InitST7789(void)
{	
	// Рестарт экрана по сигналу от МК
	RESET_HIGH;
	_delay_ms(5);
	RESET_LOW;
	_delay_ms(5);
	RESET_HIGH;
	_delay_ms(5);
		
		
	// Настройка экрана
	SendCommand(CMD_SWRESET);		// Software Reset
	_delay_ms(10);
		
	SendCommand(CMD_SLPOUT);		// Sleep out - Проснулись
	_delay_ms(10);
	
	SendCommand(CMD_COLMOD);		// Interface pixel format - Режим цвета
	SendData(0x05);					// 16 бит			Write data for 16-bit/pixel (RGB-5-6-5-bit input), 65K-Colors, 3Ah=”05h”
	_delay_ms(10);
	
	SendCommand(CMD_MADCTL);		// Memory data access control
	SendData(0b00000000);			// 0x20 = 0b00100000; MY0(верх/низ по Y) MX0(право/лево по X) MV1(vertical addressing mode(развертка справа налево или сверху вниз) ML0 RGB0 0 0 0
	_delay_ms(10);
	
	SendCommand(CMD_CASET);			// Column address set
	SendData(0);					// XS highpart	(Старшая часть начального положения Колонки)
	SendData(0);					// XS lowpart	(Младшая часть начального положения Колонки)
	SendData(0);					// XE highpart	(Старшая часть конечного положения Колонки)
	SendData(240);					// XE highpart	(Младшая часть конечного положения Колонки)
	_delay_ms(10);
	
	SendCommand(CMD_RASET);			// Row address set
	SendData(0);					// YS highpart	(Старшая часть начального положения Ряда)
	SendData(0);					// YS lowpart	(Младшая часть начального положения Ряда)
	SendData(0);					// YE highpart	(Старшая часть конечного положения Ряда)
	SendData(240);					// YE highpart	(Младшая часть конечного положения Ряда)
	_delay_ms(10);
	
	SendCommand(CMD_INVON);			// Display inversion - on
	_delay_ms(10);
	
	SendCommand(CMD_DISPON);		// Display on - включаем изображение
	_delay_ms(10);
}


//==============================================================================
//				Основные функции для вывода на экран слов и геометрии
//==============================================================================
	
// Нарисовать квадрат
void ST7789_DrawSquare(char x, char y, char red, char green, char blue, char size)
{
	if (x > 240 || y > 240 || x < 0 || y < 0)		// Проверка чтобы ввод не выходил за границы экрана
	{
		return;
	}

	COMMAND
	Send(0x2A);								// Column address set
	DATA
	Send(x >> 8);							// XS highpart	(Старшая часть начального положения Колонки)
	Send(x);								// XS lowpart	(Младшая часть начального положения Колонки)
	Send((x + size - 1) >> 8);				// XE highpart	(Старшая часть конечного положения Колонки)
	Send(x + size - 1);						// XE highpart	(Младшая часть конечного положения Колонки)

	COMMAND
	Send(0x2B);								// Row address set
	DATA
	Send(y >> 8);							// YS highpart	(Старшая часть начального положения Ряда)
	Send(y);								// YS lowpart	(Младшая часть начального положения Ряда)
	Send((y + size - 1) >> 8);				// YE highpart	(Старшая часть конечного положения Ряда)
	Send(y + size - 1);						// YE highpart	(Младшая часть конечного положения Ряда)
		
	COMMAND
	Send(0x2C);				// Memory write
		
	DATA
	for(int i = 0; i < (int)(size*size); i++)		// Отрисовка
	{
		Send((red << 3) | green >> 3);
		Send((green << 5) | blue);
	}
}
	
void ST7789_DrawSquare(char x, char y, colour paint, char size)
{
	ST7789_DrawSquare(x, y, paint.red, paint.green, paint.blue, size);
}

// Залить экран цветом
void ST7789_FillScreen(char red, char green, char blue)
{
	ST7789_DrawSquare(0, 0, red, green, blue, 240);
}
	
void ST7789_FillScreen(colour paint)
{
	ST7789_DrawSquare(0, 0, paint.red, paint.green, paint.blue, 240);
}



// Написать символ ASCII
// на цветном фоне цветной символ
void ST7789_PrintChar(char letter, char* xpos, char* ypos, char red_font, char green_font, char blue_font, char red_background, char green_background, char blue_background, char size)
{
	for(char i = 0; i < 6; i++)
	{
		char mask;
		for (char y = 0; y < 8; y++)
		{
			mask = 1;
			if(monocraft[letter - 32][i] & (mask << (7 - y)))
			{
				ST7789_DrawSquare(*xpos, *ypos + (y * size), red_font, green_font, blue_font, size);
			}
			else
			{
				ST7789_DrawSquare(*xpos, *ypos + (y * size), red_background, green_background, blue_background, size);
			}
		}
		
		*xpos += size;
		
		if (*xpos >= 240)
		{
			*xpos = 0;
			*ypos += 8 * size;
		}
		
		if (*ypos >= 240)
		{
			*xpos = 0;
			*ypos = 0;
		}
	}
}

// на цветном фоне цветной символ (перегрузка)
void ST7789_PrintChar(char letter, char* xpos, char* ypos, colour colour_font, colour colour_background, char size)
{
	ST7789_PrintChar(letter, xpos, ypos, colour_font.red, colour_font.green, colour_font.blue, colour_background.red, colour_background.green, colour_background.blue, size);
}
	
// на черном фоне цветной символ
void ST7789_PrintChar(char letter, char* xpos, char* ypos, char red_font, char green_font, char blue_font, char size)
{
	ST7789_PrintChar(letter, xpos, ypos, red_font, green_font, blue_font, 0, 0, 0, size);
}

// на черном фоне цветной символ (перегрузка)
void ST7789_PrintChar(char letter, char* xpos, char* ypos, colour paint, char size)
{
	ST7789_PrintChar(letter, xpos, ypos, paint.red, paint.green, paint.blue, size);
}

	
	
// Написать строку ASCII
// на цветном фоне цветная строка
void ST7789_PrintString(char* string, char* xpos, char* ypos, char red_font, char green_font, char blue_font, char red_background, char green_background, char blue_background, char size)
{
	char counter = 0;
	
	while (string[counter] != '\0')
	{
		ST7789_PrintChar(string[counter], xpos, ypos, red_font, green_font, blue_font, red_background, green_background, blue_background, size);
		counter++;
	}
}

// на цветном фоне цветная строка (перегрузка)	
void ST7789_PrintString(char* string, char* xpos, char* ypos, colour colour_font, colour colour_background, char size)
{
	ST7789_PrintString(string, xpos, ypos, colour_font.red, colour_font.green, colour_font.blue, colour_background.red, colour_background.green, colour_background.blue, size);
}
	
// на черном фоне цветная строка
void ST7789_PrintString(char* string, char* xpos, char* ypos, char red_font, char green_font, char blue_font, char size)
{
	ST7789_PrintString(string, xpos, ypos, red_font, green_font, blue_font, 0, 0, 0, size);
}

// на черном фоне цветная строка (перегрузка)
void ST7789_PrintString(char* string, char* xpos, char* ypos, colour colour_font, char size)
{
	ST7789_PrintString(string, xpos, ypos, colour_font.red, colour_font.green, colour_font.blue, 0, 0, 0, size);
}
	
	

