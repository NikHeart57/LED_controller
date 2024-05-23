/*
 * DS1307.cpp
 *
 * Created: 15.05.2024 23:06:47
 *  Author: KolyaPC
 */ 

#include <avr/io.h>
#include "DS1307.h"
#include "ST7789.h"

namespace DS1307
{
	//==============================================================================
	//							������� ������� � DS1307
	//==============================================================================
	// ����� ����� ����� �� ������ W	0b11010000
	// ����� ����� ����� �� ������ R	0b11010001
	
	void writeTime(char time[])
	{
		// ����� ����� ����� ������ W	0b11010000
		// ����� ����� ����� ������	R	0b11010001
		
		I2CMTinit();							// ������������� �������							// ����� TWSR: 11111 = 0xf8 - No relevant state information	available; TWINT = �0�
		I2CMTstart();							// ������� ���������� ������� ��������				// 00001 = 0x08 - A START condition has been transmitted
		I2CMTsend(DS1307_WRITE_ADRESS);			// �������� ������									// 00011 = 0x18 - SLA+W has been transmitted;		ACK has been received
		I2CMTsend(0x00);						// �������� ������(������ ������ � ������ DS1307)	// 00101 = 0x28 - Data byte has been transmitted;	ACK has been received
		
		// �������� ������ DATA					// 00101 = 0x28 - Data byte has been transmitted;	ACK has been received
		I2CMTsend(DecToBin(time[0]));			// 0x00	7-CH	6-10Sec		5-10Sec		4-10Sec		3-Sec	2-Sec	1-Sec	0-Sec	(CH-0 - ��� ���������, CH-1 - ���� ���������)
		I2CMTsend(DecToBin(time[1]));			// 0x01	7-0		6-10Min		5-10Min		4-10Min		3-Min	2-Min	1-Min	0-Min
		I2CMTsend(DecToBin(time[2]));			// 0x02	7-0		6-12/24		5-10H/AMPM	4-10Hour	3-Hour	2-Hour	1-Hour	0-Hour
		I2CMTsend(DecToBin(time[3]));			// 0x03	7-0		6-0			5-0			4-0			3-0		2-Day	1-Day	0-Day	(���� ������)
		I2CMTsend(DecToBin(time[4]));			// 0x04	7-0		6-0			5-10Date	4-10Date	3-Date	2-Date	1-Date	0-Date	(���� ������)
		I2CMTsend(DecToBin(time[5]));			// 0x05	7-0		6-0			5-0			4-10Month	3-Month	2-Month	1-Month	0-Month
		I2CMTsend(DecToBin(time[6]));			// 0x06	7-10Y	6-10Y		5-10Y		4-10Y		3-Y		2-Y		1-Y		0-Y
		I2CMTsend(0b00010000);					// 0x07	7-OUT	6-0			5-0			4-SQWE		3-0		2-0		1-RS1	0-RS0	(OUT - ������ �� ������; SQWE - ��������� �� ������; RS1..0 - ������������ ������� ���������� SQWE)
		I2CMTstop();							// ���� �� �������									// 11111 = 0xf8 - No relevant state information	available; TWINT = �0�
	}

	void readTime(char time[])
	{
		// ����� ����� ����� ������		0b11010000
		// ����� ����� ����� ������		0b11010001
	
		// ===== ������ � ��������� ���������� ������ � �������� ���������� ������ =====
		I2CMRinit();							// ������������� �������							// ����� TWSR: 11111 = 0xf8 - No relevant state information	available; TWINT = �0�
		I2CMRstart();							// ������� ���������� ������� ��������				// 00001 = 0x08 - A START condition has been transmitted
		I2CMRsend(DS1307_WRITE_ADRESS);			// �������� ������ ������
		I2CMRsend(0x00);						// �������� ������ ������
		
		I2CMRstart();							// ��������� �����
		I2CMRsend(DS1307_READ_ADRESS);			// �������� ������ ������							// 01000 = 0x40 - SLA+R has been transmitted;	ACK has been received
		time[0] = BinToDec(I2CMRread());		// ������ ������   ACK								// 01010 = Data byte has been received;			ACK has been returned
		time[1] =  BinToDec(I2CMRread());
		time[2] = BinToDec(I2CMRread());
		time[3] = BinToDec(I2CMRread());
		time[4] = BinToDec(I2CMRread());
		time[5] = BinToDec(I2CMRread());
		time[6] = BinToDec(I2CMRreadLast());	// ������ ������ NOACK								// 01011 = 0x58 - Data byte has been received;	NOT ACK has been returned
				
		I2CMRstop();							// ���� �� �������									// 11111 = 0xf8 - No relevant state information	available; TWINT = �0�		
	}
}

