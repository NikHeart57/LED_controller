/*
 * DS1307.h
 *
 * Created: 15.05.2024 23:07:30
 *  Author: KolyaPC
 */ 


/* 
 * TODO I2CMRinit(void) = I2CMTinit(void) ��� ����������
 * 
 * 
 */

#ifndef DS1307_H_
#define DS1307_H_

#define DS1307_WRITE_ADRESS		0b11010000
#define DS1307_READ_ADRESS		0b11010001

namespace DS1307
{
	/*
	 *	https://robotchip.ru/obzor-chasov-realnogo-vremeni-ds1307/
	 */
	//==============================================================================
	//							������� ������� � DS1307
	//==============================================================================
	// ����� ����� ����� �� ������ W	0b11010000
	// ����� ����� ����� �� ������ R	0b11010001
		
	void WriteTime(char time[]);
	void ReadTime(char time[]);
		
				
	//==============================================================================
	//	INLINE	������� ����������� �� ����������� ����� � �������-����������
	//==============================================================================
		
	inline char BinToDec(unsigned char c);				// ������� �������-����������� ����� � ����������
	inline char DecToBin(unsigned char c);				// ������� ����������� ����� � �������-����������
				
		
	//==============================================================================
	//	INLINE					������� ��������� I2C
	//==============================================================================
		
	//  ������� ������� ������������  //
	inline void I2CMTinit(void);								// ������ �������� ����������
	inline void I2CMTstart(void);								// Send START condition
	inline void I2CMTsend(char data);							// Send START condition
	inline void I2CMTstop(void);
		
	//  ������� ������� ���������  //
	inline void I2CMRinit(void);
	inline void I2CMRstart(void);
	inline void I2CMRsend(char data);
	inline char I2CMRread(void);
	inline char I2CMRreadLast(void);
	inline void I2CMRstop(void);
		
		
		
	//==============================================================================
	//	INLINE	������� ����������� �� ����������� ����� � �������-����������
	//==============================================================================
		
	inline char BinToDec(char c)					// ������� �������-����������� ����� � ����������
	{
		unsigned char ch = ((c >> 4) * 10 + (0b00001111 & c));
		return ch;
	}

	inline char DecToBin(char c)					// ������� ����������� ����� � �������-����������
	{
		unsigned char ch = ((c / 10) << 4)|(c % 10);
		return ch;
	}	
		
		
	//==============================================================================
	//	INLINE					������� ��������� I2C
	//==============================================================================
		
	//  ������� ������� ������������  //
		
	inline void I2CMTinit(void)
	{
		TWBR = 40;										// ������ �������� ����������. ��� ������ ��� ���������; 255 -> 28����/�; 66 -> 100����/�; 11 -> 388����/�
		TWSR |= (0 << TWPS1)|(0 << TWPS0);				// ���������� (00 - 11)
	}

	inline void I2CMTstart(void)
	{
		TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);	// Send START condition
		while (!(TWCR & (1 << TWINT)));					// Wait for TWINT Flag set. This indicates that the START condition has been transmitted
	}

	inline void I2CMTsend(char data)
	{
		TWDR = data;									// Load data into TWDR Register.
		TWCR = (1 << TWINT)|(1 << TWEN);				// Clear TWINT bit in TWCR to start transmission of data
		while (!(TWCR & (1 << TWINT)));					// �������� ���������� ��������
	}

	inline void I2CMTstop(void)
	{
		TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWSTO);	// Transmit STOP condition
	}
		
		
	//  ������� ������� ���������  //
		
	inline void I2CMRinit(void)
	{
		TWBR = 66;										// ������ �������� ����������. ��� ������ ��� ���������; 255 -> 28����/�; 66 -> 100����/�
		TWSR |= (0 << TWPS1)|(0 << TWPS0);				// ���������� (00 - 11)
	}
		
	inline void I2CMRstart(void)
	{
		TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);	// Send START condition
		while (!(TWCR & (1 << TWINT)));					// Wait for TWINT Flag set. This indicates that the START condition has been transmitted
	}

	inline void I2CMRsend(char data)
	{
		TWDR = data;									// Load data into TWDR Register.
		TWCR = (1 << TWINT)|(1 << TWEN);				// Clear TWINT bit in TWCR to start transmission of data
		while (!(TWCR & (1 << TWINT)));					// �������� ���������� ��������
	}
		
	inline char I2CMRread(void)
	{
		TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWEA);
		while (!(TWCR & (1 << TWINT)));					// �������� ��������� ���� TWIN
		return TWDR;									// ������ ������� ������
	}
	
	inline char I2CMRreadLast(void)
	{
		TWCR = (1 << TWINT)|(1 << TWEN);
		while (!(TWCR & (1 << TWINT)));					//�������� ��������� ���� TWIN
		return TWDR;									//������ ������� ������
	}
		
	inline void I2CMRstop(void)
	{
		TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWSTO);	// Transmit STOP condition
	}	
}

#endif /* DS1307_H_ */

