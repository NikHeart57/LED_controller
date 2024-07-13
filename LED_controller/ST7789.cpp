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
//							���������� ������� ������
//==============================================================================

char xpos = 0;
char ypos = 0;


//==============================================================================
//				  ������� ������������� SPI � ������������� ������
//==============================================================================
	
void ST7789_InitSPI(void)
{
	// ��������� ����� SPI
	DDR_SPI  |= (1 << MOSI)|(1 << SCK)|(1 << SS)|(0 << MISO);		// ������������ ���������! ��� ���� 4-� ����� SPI!
	PORT_SPI |= (1 << MOSI)|(1 << SCK)|(1 << SS)|(1 << MISO);		//
			
	// ��������� ������� SPI
	// ���������� SPI - (SPE), ������� ��� ������ - (DORD), ������ - (MSTR), ����� ? ((1 << CPHA)|(0 << SPR1)), �������� ������� - 2 (0 = SPR1, 0 = SPR0 + 0 = SPIX) (~7372800 ���/� = 921600����/�)
	SPCR = (1 << SPE)|(0 << DORD)|(1 << MSTR)|(1 << CPOL)|(1 << CPHA)|(0 << SPR1)|(0 << SPR0);	// (1 << CPHA)|(1 << SPR1) - ����������� ��� ST7789!!!
	SPSR = (0 << SPI2X);
			
	// ��������� ���� RES ST7789
	DDR_RES  |= (1 << RES);											// ������ (1)
	PORT_RES |= (1 << RES);											// ���������� ������� ������� RES
			
	// ��������� ���� ������� ST7789
	DDR_DC  |= (1 << DC);											// ������ (1)
	PORT_DC |= (1 << DC);											// ���������� ������� ������� DC
}
	
void ST7789_InitST7789(void)
{	
	// ������� ������ �� ������� �� ��
	RESET_HIGH;
	_delay_ms(5);
	RESET_LOW;
	_delay_ms(5);
	RESET_HIGH;
	_delay_ms(5);
		
		
	// ��������� ������
	SendCommand(CMD_SWRESET);		// Software Reset
	_delay_ms(10);
		
	SendCommand(CMD_SLPOUT);		// Sleep out - ����������
	_delay_ms(10);
	
	SendCommand(CMD_COLMOD);		// Interface pixel format - ����� �����
	SendData(0x05);					// 16 ���			Write data for 16-bit/pixel (RGB-5-6-5-bit input), 65K-Colors, 3Ah=�05h�
	_delay_ms(10);
	
	SendCommand(CMD_MADCTL);		// Memory data access control
	SendData(0b00000000);			// 0x20 = 0b00100000; MY0(����/��� �� Y) MX0(�����/���� �� X) MV1(vertical addressing mode(��������� ������ ������ ��� ������ ����) ML0 RGB0 0 0 0
	_delay_ms(10);
	
	SendCommand(CMD_CASET);			// Column address set
	SendData(0);					// XS highpart	(������� ����� ���������� ��������� �������)
	SendData(0);					// XS lowpart	(������� ����� ���������� ��������� �������)
	SendData(0);					// XE highpart	(������� ����� ��������� ��������� �������)
	SendData(240);					// XE highpart	(������� ����� ��������� ��������� �������)
	_delay_ms(10);
	
	SendCommand(CMD_RASET);			// Row address set
	SendData(0);					// YS highpart	(������� ����� ���������� ��������� ����)
	SendData(0);					// YS lowpart	(������� ����� ���������� ��������� ����)
	SendData(0);					// YE highpart	(������� ����� ��������� ��������� ����)
	SendData(240);					// YE highpart	(������� ����� ��������� ��������� ����)
	_delay_ms(10);
	
	SendCommand(CMD_INVON);			// Display inversion - on
	_delay_ms(10);
	
	SendCommand(CMD_DISPON);		// Display on - �������� �����������
	_delay_ms(10);
}


//==============================================================================
//								������� � �������
//==============================================================================

void ST7789_SetXpos(char x)
{
	xpos = x;
}

void ST7789_SetYpos(char y)
{
	ypos = y;
}

void ST7789_SetXYpos(char x, char y)
{
	xpos = x;
	ypos = y;
}

void ST7789_GetXpos(char* x)
{
	*x = xpos;
}

void ST7789_GetYpos(char* y)
{
	*y = ypos;
}

void ST7789_GetXYpos(char* x, char* y)
{
	*x = xpos;
	*y = ypos;
}


//==============================================================================
//				�������� ������� ��� ������ �� ����� ���� � ���������
//==============================================================================

// ���������� �������
void ST7789_DrawRectangle(char x, char y, char widtgh, char height, char red, char green, char blue)
{
	if (x > 240 || y > 240 || x < 0 || y < 0)		// �������� ����� ���� �� ������� �� ������� ������
	{
		return;
	}

	COMMAND
	Send(0x2A);								// Column address set
	DATA
	Send(x >> 8);							// XS highpart	(������� ����� ���������� ��������� �������)
	Send(x);								// XS lowpart	(������� ����� ���������� ��������� �������)
	Send((x + widtgh - 1) >> 8);			// XE highpart	(������� ����� ��������� ��������� �������)
	Send(x + widtgh - 1);					// XE highpart	(������� ����� ��������� ��������� �������)

	COMMAND
	Send(0x2B);								// Row address set
	DATA
	Send(y >> 8);							// YS highpart	(������� ����� ���������� ��������� ����)
	Send(y);								// YS lowpart	(������� ����� ���������� ��������� ����)
	Send((y + height - 1) >> 8);			// YE highpart	(������� ����� ��������� ��������� ����)
	Send(y + height - 1);					// YE highpart	(������� ����� ��������� ��������� ����)
		
	COMMAND
	Send(0x2C);								// Memory write
		
	DATA
	for(int i = 0; i < (int)((int)height*(int)widtgh); i++)		// ���������
	{
		Send((red << 3) | green >> 3);
		Send((green << 5) | blue);
	}
}

void ST7789_DrawRectangle(char x, char y, char widtgh, char height, colour paint)
{
	ST7789_DrawRectangle(x, y, widtgh, height, paint.red, paint.green, paint.blue);
}

////////////////////////////////////////////////////////////////////////////////
// ���������� �������
void ST7789_DrawSquare(char x, char y, char size, char red, char green, char blue)
{
	ST7789_DrawRectangle(x, y, size, size, red, green, blue);
}
	
void ST7789_DrawSquare(char x, char y, char size, colour paint)
{
	ST7789_DrawRectangle(x, y, size, size, paint.red, paint.green, paint.blue);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ������ ����� ������
void ST7789_FillScreen(char red, char green, char blue)
{
	ST7789_DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, red, green, blue);
}
	
void ST7789_FillScreen(colour paint)
{
	ST7789_DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, paint.red, paint.green, paint.blue);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �������� ������ ASCII
// ������������� ������
	// �� ������� ���� ������� ������
	// �� RGB	� ��� ���������
void ST7789_PrintChar(char letter, char red_font, char green_font, char blue_font, char red_background, char green_background, char blue_background, char size)
{	
	if (xpos + 6 * size > SCREEN_WIDTH)												// �������� �� ��, ����� ������ �� ������� �� ������� ������ �� ���� ������. ���� ������ �� ��������� �� ���� ������� ��� ���������� ��� �������
	{
		xpos = 0;
		ypos += 8 * size;
	}
	
	for(uint8_t i = 0; i < 6; i++)														// ���������� ��������� �������. ���� ���� ���������� ����� ������� ("�� ����������")
	{		
		char mask;
		for (uint8_t y = 0; y < 8; y++)												// ���� ���� ���������� ���� �������
		{
			mask = 1;
			if(monocraft[letter - 32][i] & (mask << (7 - y)))
			{
				ST7789_DrawRectangle(xpos, ypos + (y * size), size, size, red_font, green_font, blue_font);
			}
			else
			{
				ST7789_DrawRectangle(xpos, ypos + (y * size), size, size, red_background, green_background, blue_background);
			}
		}
		
		xpos += size;
		
		if (xpos >= SCREEN_WIDTH)
		{
			xpos = 0;
			ypos += 8 * size;
		}
		
		if (ypos >= SCREEN_HEIGHT)
		{
			xpos = 0;
			ypos = 0;
		}
	}
}

	// �� ������� ���� ������� ������
	// �� �����	� ��� ���������
void ST7789_PrintChar(char letter, colour colour_font, colour colour_background, char size)
{
	ST7789_PrintChar(letter, colour_font.red, colour_font.green, colour_font.blue, colour_background.red, colour_background.green, colour_background.blue, size);
}
	// �� ������� ���� ������� ������
	// �� RGB	� � ������������
void ST7789_PrintChar(char letter, char xcur, char ycur, char red_font, char green_font, char blue_font, char red_background, char green_background, char blue_background, char size)
{
	xpos = xcur;
	ypos = ycur;
	
	ST7789_PrintChar(letter, red_font, green_font, blue_font, red_background, green_background, blue_background, size);
}
	// �� ������� ���� ������� ������
	// �� ����� � � ������������
void ST7789_PrintChar(char letter, char xcur, char ycur, colour colour_font, colour colour_background, char size)
{
	xpos = xcur;
	ypos = ycur;
	
	ST7789_PrintChar(letter, colour_font.red, colour_font.green, colour_font.blue, colour_background.red, colour_background.green, colour_background.blue, size);
}

	
	// �� ��������� ���� ������� ������
	// �� RGB	� ��� ���������
void ST7789_PrintChar(char letter, char red_font, char green_font, char blue_font, char size)
{
	ST7789_PrintChar(letter, red_font, green_font, blue_font, RED_BACKGROUND, GREEN_BACKGROUND, BLUE_BACKGROUND, size);
}

	// �� ��������� ���� ������� ������
	// �� �����	� ��� ���������
void ST7789_PrintChar(char letter, colour paint, char size)
{
	ST7789_PrintChar(letter, paint.red, paint.green, paint.blue, size);
}
	// �� ��������� ���� ������� ������
	// �� RGB	� � ������������
void ST7789_PrintChar(char letter, char xcur, char ycur, char red_font, char green_font, char blue_font, char size)
{
	xpos = xcur;
	ypos = ycur;
	
	ST7789_PrintChar(letter, red_font, green_font, blue_font, RED_BACKGROUND, GREEN_BACKGROUND, BLUE_BACKGROUND, size);
}
	// �� ��������� ���� ������� ������
	// �� �����	� � ������������
void ST7789_PrintChar(char letter, char xcur, char ycur, colour paint, char size)
{
	xpos = xcur;
	ypos = ycur;
	
	ST7789_PrintChar(letter, paint.red, paint.green, paint.blue, size);
}


	// ����������� �����
	// ��� ���������
void ST7789_PrintChar(char letter, char size)
{
	ST7789_PrintChar(letter, RED_FONT, GREEN_FONT, BLUE_FONT, RED_BACKGROUND, GREEN_BACKGROUND, BLUE_BACKGROUND, size);
}

	// ����������� �����
	// ��� ���������
void ST7789_PrintChar(char letter)
{
	ST7789_PrintChar(letter, RED_FONT, GREEN_FONT, BLUE_FONT, RED_BACKGROUND, GREEN_BACKGROUND, BLUE_BACKGROUND, FONT_SIZE);
}

	// ����������� �����
	// � ������������
void ST7789_PrintChar(char letter, char xcur, char ycur, char size)
{
	xpos = xcur;
	ypos = ycur;
		
	ST7789_PrintChar(letter, RED_FONT, GREEN_FONT, BLUE_FONT, RED_BACKGROUND, GREEN_BACKGROUND, BLUE_BACKGROUND, size);
}

	// ����������� �����
	// � ������������
void ST7789_PrintChar(char letter, char xcur, char ycur)
{
	xpos = xcur;
	ypos = ycur;
	
	ST7789_PrintChar(letter, RED_FONT, GREEN_FONT, BLUE_FONT, RED_BACKGROUND, GREEN_BACKGROUND, BLUE_BACKGROUND, FONT_SIZE);
}
	
	
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
// �������� ������ ASCII
	// ������������� ������
		// �� ������� ���� ������� ������
		// �� RGB	� ��� ���������
void ST7789_PrintString(char* string, char red_font, char green_font, char blue_font, char red_background, char green_background, char blue_background, char size)
{
	int counter = 0;
	
	while (string[counter] != '\0')
	{
		ST7789_PrintChar(string[counter], red_font, green_font, blue_font, red_background, green_background, blue_background, size);
		counter++;
	}
}
		// �� ������� ���� ������� ������
		// �� �����	� ��� ���������	
void ST7789_PrintString(char* string, colour colour_font, colour colour_background, char size)
{
	ST7789_PrintString(string, colour_font.red, colour_font.green, colour_font.blue, colour_background.red, colour_background.green, colour_background.blue, size);
}
		// �� ������� ���� ������� ������
		// �� RGB	� � ������������
void ST7789_PrintString(char* string, char xcur, char ycur, char red_font, char green_font, char blue_font, char red_background, char green_background, char blue_background, char size)
{
	xpos = xcur;
	ypos = ycur;
		
	ST7789_PrintString(string, red_font, green_font, blue_font, red_background, green_background, blue_background, size);
}
		// �� ������� ���� ������� ������
		// �� �����	� � ������������
void ST7789_PrintString(char* string, char xcur, char ycur, colour colour_font, colour colour_background, char size)
{
	xpos = xcur;
	ypos = ycur;
		
	ST7789_PrintString(string, colour_font.red, colour_font.green, colour_font.blue, colour_background.red, colour_background.green, colour_background.blue, size);
}


		// �� ��������� ���� ������� ������
		// �� RGB	� ��� ���������
void ST7789_PrintString(char* string, char red_font, char green_font, char blue_font, char size)
{
	ST7789_PrintString(string, red_font, green_font, blue_font, RED_BACKGROUND, GREEN_BACKGROUND, BLUE_BACKGROUND, size);
}

		// �� ��������� ���� ������� ������
		// �� ����� � ��� ���������
void ST7789_PrintString(char* string, colour colour_font, char size)
{
	ST7789_PrintString(string, colour_font.red, colour_font.green, colour_font.blue, RED_BACKGROUND, GREEN_BACKGROUND, BLUE_BACKGROUND, size);
}

		// �� ��������� ���� ������� ������
		// �� RGB	� � ������������
void ST7789_PrintString(char* string, char xcur, char ycur, char red_font, char green_font, char blue_font, char size)
{
	xpos = xcur;
	ypos = ycur;
		
	ST7789_PrintString(string, red_font, green_font, blue_font, RED_BACKGROUND, GREEN_BACKGROUND, BLUE_BACKGROUND, size);
}

		// �� ��������� ���� ������� ������
		// �� ����� � � ������������
void ST7789_PrintString(char* string, char xcur, char ycur, colour colour_font, char size)
{
	xpos = xcur;
	ypos = ycur;
		
	ST7789_PrintString(string, colour_font.red, colour_font.green, colour_font.blue, RED_BACKGROUND, GREEN_BACKGROUND, BLUE_BACKGROUND, size);
}

		// ����������� �����
		// ��� ���������
void ST7789_PrintString(char* string, char size)
{
	ST7789_PrintString(string, RED_FONT, GREEN_FONT, BLUE_FONT, RED_BACKGROUND, GREEN_BACKGROUND, BLUE_BACKGROUND, size);
}
		// ����������� �����
		// ��� ���������
void ST7789_PrintString(char* string)
{
	ST7789_PrintString(string, RED_FONT, GREEN_FONT, BLUE_FONT, RED_BACKGROUND, GREEN_BACKGROUND, BLUE_BACKGROUND, FONT_SIZE);
}
		// ����������� �����
		// � ������������
void ST7789_PrintString(char* string, char xcur, char ycur, char size)
{
	xpos = xcur;
	ypos = ycur;
	
	ST7789_PrintString(string, RED_FONT, GREEN_FONT, BLUE_FONT, RED_BACKGROUND, GREEN_BACKGROUND, BLUE_BACKGROUND, size);	
}
		// ����������� �����
		// � ������������
void ST7789_PrintString(char* string, char xcur, char ycur)
{
	xpos = xcur;
	ypos = ycur;
	
	ST7789_PrintString(string, RED_FONT, GREEN_FONT, BLUE_FONT, RED_BACKGROUND, GREEN_BACKGROUND, BLUE_BACKGROUND, FONT_SIZE);
}

	
	
	

