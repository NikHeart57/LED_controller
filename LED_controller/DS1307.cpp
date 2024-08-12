/*
 * DS1307.cpp
 *
 * Created: 15.05.2024 23:06:47
 *  Author: KolyaPC
 */ 

#include <avr/io.h>
#include "DS1307.h"
#include "ST7789.h"



//==============================================================================
//							������� ��������� I2C
//==============================================================================
// ���������� ������
void I2CErrorHandler(void)
{
	
}


//  ������� ������� ������������  //
void I2CMTInit(void)
{
	TWBR = 66;												// ������ �������� ����������. ��� ������ ��� ���������; 255 -> 28����/�; 66 -> 100����/�; 11 -> 388����/�
	TWSR |= (0 << TWPS1)|(0 << TWPS0);						// ������ �������� ����������
}


void I2CMTStart(void)
{
	TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);			// Send START condition
	while (!(TWCR & (1 << TWINT)));							// Wait for TWINT Flag set. This indicates that the START condition has been transmitted
	if ((TWSR & 0xF8) != MT_START)							// Check value of TWI Status Register. Mask prescaler bits. If status different from MT_START go to ERROR
	{
		I2CErrorHandler();
	}
}

void I2CMTSendSLA(uint8_t sla)
{
	TWDR = sla;												// Load data into TWDR Register.
	TWCR = (1 << TWINT)|(1 << TWEN);						// Clear TWINT bit in TWCR to start transmission of data
	while (!(TWCR & (1 << TWINT)));							// Wait for TWINT Flag set. This indicates that the SLA has been transmitted
	if ((TWSR & 0xF8) != MT_DATA_TRANSMITED_ACK_RECIEVED)	// Check value of TWI Status Register. Mask	prescaler bits. If status different from MT_DATA_TRANSMITED_ACK_RECIEVED go to ERROR
	{
		I2CErrorHandler();
	}
}

void I2CMTSendData(uint8_t data)
{
	TWDR = data;											// Load data into TWDR Register.
	TWCR = (1 << TWINT)|(1 << TWEN);						// Clear TWINT bit in TWCR to start transmission of data
	while (!(TWCR & (1 << TWINT)));							// Wait for TWINT Flag set. This indicates that the DATA has been transmitted
	if ((TWSR & 0xF8) != MT_DATA_TRANSMITED_ACK_RECIEVED)	// Check value of TWI Status Register. Mask	prescaler bits. If status different from MT_DATA_TRANSMITED_ACK_RECIEVED go to ERROR
	{
		I2CErrorHandler();
	}
}

void I2CMTStop(void)
{
	TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWSTO);			// Transmit STOP condition
}




//  ������� ������� ���������  //
void I2CMRInit(void)
{
	TWBR = 66;												// ������ �������� ����������. ��� ������ ��� ���������; 255 -> 28����/�; 66 -> 100����/�; 11 -> 388����/�
	TWSR |= (0 << TWPS1)|(0 << TWPS0);						// ������ �������� ����������.
}

void I2CMRStart(void)
{
	TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);			// Send START condition
	while (!(TWCR & (1 << TWINT)));							// Wait for TWINT Flag set. This indicates that the START condition has been transmitted
	if ((TWSR & 0xF8) != MR_START)							// Check value of TWI Status Register. Mask prescaler bits. If status different from MR_START go to ERROR
	{
		I2CErrorHandler();
	}
}

void I2CMRSendSLA(uint8_t sla)
{
	TWDR = sla;												// Load data into TWDR Register.
	TWCR = (1 << TWINT)|(1 << TWEN);						// Clear TWINT bit in TWCR to start transmission of data
	while (!(TWCR & (1 << TWINT)));							// Wait for TWINT Flag set. This indicates that the SLA has been transmitted
	if ((TWSR & 0xF8) != MR_SLA_R_TRANSMITED_ACK_RECIEVED)	// Check value of TWI Status Register. Mask	prescaler bits. If status different from MR_SLA_R_TRANSMITED_ACK_RECIEVED go to ERROR
	{
		I2CErrorHandler();
	}
}

uint8_t I2CMRRead(void)
{
	TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWEA);			// Clear TWINT bit in TWCR to start receiving of data, set bit in TWEA to return ASK bit
	while (!(TWCR & (1 << TWINT)));							// Wait for TWINT Flag set. This indicates that the data has been recieved
	if ((TWSR & 0xF8) != MR_DATA_RECIEVED_ACK_RETURNED)		// Check value of TWI Status Register. Mask prescaler bits. If status different from MR_DATA_RECIEVED_ACK_RETURNED go to ERROR
	{
		I2CErrorHandler();
	}
	return TWDR;											// ������� ������
}

uint8_t I2CMRReadLast(void)
{
	TWCR = (1 << TWINT)|(1 << TWEN);						// Clear TWINT bit in TWCR to start receiving of data, DONT set bit in TWEA to return NO ASK bit
	while (!(TWCR & (1 << TWINT)));							// Wait for TWINT Flag set. This indicates that the data has been recieved
	if ((TWSR & 0xF8) != MR_DATA_RECIEVED_NOT_ACK_RETURNED)	// Check value of TWI Status Register. Mask prescaler bits. If status different from MR_DATA_RECIEVED_NOT_ACK_RETURNED go to ERROR
	{
		I2CErrorHandler();
	}
	return TWDR;											// ������� ������
}

void I2CMRStop(void)
{
	TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWSTO);			// Transmit STOP condition
}



//==============================================================================
//		������� ����������� �� ����������� ����� � �������-����������
//==============================================================================

uint8_t BinToDec(uint8_t binary)							// ������� �������-����������� ����� � ����������
{
	uint8_t decimal = ((binary >> 4) * 10 + (0b00001111 & binary));
	return decimal;
}

uint8_t DecToBin(uint8_t decimal)							// ������� ����������� ����� � �������-����������
{
	uint8_t binary = ((decimal / 10) << 4)|(decimal % 10);
	return binary;
}


//==============================================================================
//							������� ������� � DS1307
//==============================================================================
// ����� ����� ����� �� ������ W	0b11010000
// ����� ����� ����� �� ������ R	0b11010001
	
void DS1307_WriteTime(uint8_t time[])
{
	// ===== ��������� I2C =====
	I2CMTInit();							// ������������� �������								// ����� TWSR: 11111 = 0xf8 - No relevant state information	available; TWINT = �0�
	I2CMTStart();							// ������� ���������� ������� ��������					// ����� TWSR: 00001 = 0x08 - A START condition has been transmitted
	
	// ===== ������ �������� � �������� ���������� ������ � ������� �������� ������ =====
	I2CMTSendSLA(DS1307_WRITE_ADRESS);		// �������� ������ ������								// ����� TWSR: 00011 = 0x18 - SLA+W has been transmitted;		ACK has been received
	I2CMTSendData(0x00);					// �������� ������ ������ � �������� �������� ������	// ����� TWSR: 00101 = 0x28 - Data byte has been transmitted;	ACK has been received
		
	// ===== �������� ������ =====			// ������ TWSR: 00101 = 0x28 - Data byte has been transmitted;	ACK has been received
	I2CMTSendData(DecToBin(time[0]));		// ����� - 0x00	7-CH	6-10Sec		5-10Sec		4-10Sec		3-Sec	2-Sec	1-Sec	0-Sec	(CH-0 - ��� ���������, CH-1 - ���� ���������)
	I2CMTSendData(DecToBin(time[1]));		// ����� - 0x01	7-0		6-10Min		5-10Min		4-10Min		3-Min	2-Min	1-Min	0-Min
	I2CMTSendData(DecToBin(time[2]));		// ����� - 0x02	7-0		6-12/24		5-10H/AMPM	4-10Hour	3-Hour	2-Hour	1-Hour	0-Hour
	I2CMTSendData(DecToBin(time[3]));		// ����� - 0x03	7-0		6-0			5-0			4-0			3-0		2-Day	1-Day	0-Day	(���� ������)
	I2CMTSendData(DecToBin(time[4]));		// ����� - 0x04	7-0		6-0			5-10Date	4-10Date	3-Date	2-Date	1-Date	0-Date	(���� ������)
	I2CMTSendData(DecToBin(time[5]));		// ����� - 0x05	7-0		6-0			5-0			4-10Month	3-Month	2-Month	1-Month	0-Month
	I2CMTSendData(DecToBin(time[6]));		// ����� - 0x06	7-10Y	6-10Y		5-10Y		4-10Y		3-Y		2-Y		1-Y		0-Y
	I2CMTSendData(0b00010000);				// ����� - 0x07	7-OUT	6-0			5-0			4-SQWE		3-0		2-0		1-RS1	0-RS0	(OUT - ������ �� ������; SQWE - ��������� �� ������; RS1..0 - ������������ ������� ���������� SQWE)
	
	// ===== ���������� I2C =====
	I2CMTStop();							// ���� �� �������										// ����� TWSR: 11111 = 0xf8 - No relevant state information	available; TWINT = �0�
}

void DS1307_ReadTime(uint8_t time[])
{
	// ===== ��������� I2C =====
	I2CMTInit();							// ������������� �������								// ����� TWSR: 11111 = 0xf8 - No relevant state information	available; TWINT = �0�
	I2CMTStart();							// ������� ���������� ������� ��������					// ����� TWSR: 00001 = 0x08 - A START condition has been transmitted
	
	// ===== ������ �������� � �������� ���������� ������ � ������� �������� ������ =====
	I2CMTSendSLA(DS1307_WRITE_ADRESS);		// �������� ������ ������								// ����� TWSR: 00011 = 0x18 - SLA+W has been transmitted;		ACK has been received
	I2CMTSendData(0x00);					// �������� ������ ������ � �������� �������� ������	// ����� TWSR: 00101 = 0x28 - Data byte has been transmitted;	ACK has been received
		
	// ===== ������ ������ =====	
	I2CMRStart();							// ��������� �����										// ����� TWSR: 00010 = 0x10 - A repeated START condition has been transmitted
	I2CMTSendSLA(DS1307_READ_ADRESS);		// �������� ������ ������								// ����� TWSR: 01000 = 0x40 - SLA+R has been transmitted;	ACK has been received
	time[0] = BinToDec(I2CMRRead());		// ������ ������ � ACK									// ����� TWSR: 01010 = 0x50 - Data byte has been received;	ACK has been returned
	time[1] = BinToDec(I2CMRRead());		// ������ ������ � ACK									// ����� TWSR: 01010 = 0x50 - Data byte has been received;	ACK has been returned
	time[2] = BinToDec(I2CMRRead());		// ������ ������ � ACK									// ����� TWSR: 01010 = 0x50 - Data byte has been received;	ACK has been returned
	time[3] = BinToDec(I2CMRRead());		// ������ ������ � ACK									// ����� TWSR: 01010 = 0x50 - Data byte has been received;	ACK has been returned
	time[4] = BinToDec(I2CMRRead());		// ������ ������ � ACK									// ����� TWSR: 01010 = 0x50 - Data byte has been received;	ACK has been returned
	time[5] = BinToDec(I2CMRRead());		// ������ ������ � ACK									// ����� TWSR: 01010 = 0x50 - Data byte has been received;	ACK has been returned
	time[6] = BinToDec(I2CMRReadLast());	// ������ ������ � NO ACK								// ����� TWSR: 01011 = 0x58 - Data byte has been received;	NOT ACK has been returned
	
	// ===== ���������� I2C =====			
	I2CMRStop();							// ���� �� �������										// ����� TWSR: 11111 = 0xf8 - No relevant state information	available; TWINT = �0�		
}

