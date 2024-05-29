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
	//				 ������� ������������� SPI, ����� � ����� SPI
	//==============================================================================
	
	void init(void)
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
		
		
		// ������� ������
		RESET_HIGH;
		_delay_ms(20);
		RESET_LOW;
		_delay_ms(5);
		RESET_HIGH;
		_delay_ms(20);
		
		
		// ��������� ������
		sendCommand(0x01);			// Software Reset
		_delay_ms(10);
		sendCommand(0x11);			// Sleep out - ����������
		_delay_ms(10);
	
		sendCommand(0x3A);			// ����� �����:
		sendData(0x06);				// 18 ���			Write data for 18-bit/pixel (RGB-6-6-6-bit input), 262K-Colors, 3Ah=�06h�
		_delay_ms(10);
	
		sendCommand(0x36);			// (36h) Memory data access control
		sendData(0b00000000);		// 0x20 = 0b00100000; MY0(����/��� �� Y) MX0(�����/���� �� X) MV1(vertical addressing mode(��������� ������ ������ ��� ������ ����) ML0 RGB0 0 0 0
		_delay_ms(10);
	
		sendCommand(0x2A);			// Column address set
		sendData(0);				// XS highpart	(������� ����� ���������� ��������� �������)
		sendData(0);				// XS lowpart	(������� ����� ���������� ��������� �������)
		sendData(0);				// XE highpart	(������� ����� ��������� ��������� �������)
		sendData(240);				// XE highpart	(������� ����� ��������� ��������� �������)
		_delay_ms(10);
	
		sendCommand(0x2B);			// Row address set
		sendData(0);				// YS highpart	(������� ����� ���������� ��������� ����)
		sendData(0);				// YS lowpart	(������� ����� ���������� ��������� ����)
		sendData(0);				// YE highpart	(������� ����� ��������� ��������� ����)
		sendData(240);				// YE highpart	(������� ����� ��������� ��������� ����)
		_delay_ms(10);
	
		sendCommand(0x21);			// Display inversion - on
		_delay_ms(10);
	
		sendCommand(0x29);			// Display on - �������� �����������
		_delay_ms(10);
	}


	//==============================================================================
	//							������� ������ ST7789
	//==============================================================================
		
	void fillScreen(char red, char green, char blue)
	{
		sendCommand(0x2A);			// Column address set
		sendData(0);				// XS highpart	(������� ����� ���������� ��������� �������)
		sendData(0);				// XS lowpart	(������� ����� ���������� ��������� �������)
		sendData(240 >> 8);			// XE highpart	(������� ����� ��������� ��������� �������)
		sendData(240);				// XE highpart	(������� ����� ��������� ��������� �������)


		sendCommand(0x2B);			// Row address set
		sendData(0);				// YS highpart	(������� ����� ���������� ��������� ����)
		sendData(0);				// YS lowpart	(������� ����� ���������� ��������� ����)
		sendData(240 >> 8);			// YE highpart	(������� ����� ��������� ��������� ����)
		sendData(240);				// YE highpart	(������� ����� ��������� ��������� ����)

		sendCommand(0x2C);			// �� ����� ��� ���?
		
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