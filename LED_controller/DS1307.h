/*
 * DS1307.h
 *
 * Created: 15.05.2024 23:07:30
 *  Author: KolyaPC
 */ 


/* 
 * TODO I2CMRinit(void) = I2CMTinit(void) как приравнять
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
	//							Функции общения с DS1307
	//==============================================================================
	// Адрес часов слейв на запись W	0b11010000
	// Адрес часов слейв на чтение R	0b11010001
		
	void WriteTime(char time[]);
	void ReadTime(char time[]);
		
				
	//==============================================================================
	//	INLINE	Функции конвертации из десятичного числа в двоично-десятичное
	//==============================================================================
		
	inline char BinToDec(unsigned char c);				// Перевод двоично-десятичного числа в десятичное
	inline char DecToBin(unsigned char c);				// Перевод десятичного числа в двоично-десятичное
				
		
	//==============================================================================
	//	INLINE					Функции протокола I2C
	//==============================================================================
		
	//  Функции мастера трансмиттера  //
	inline void I2CMTinit(void);								// Задает скорость соединения
	inline void I2CMTstart(void);								// Send START condition
	inline void I2CMTsend(char data);							// Send START condition
	inline void I2CMTstop(void);
		
	//  Функции мастера рессивера  //
	inline void I2CMRinit(void);
	inline void I2CMRstart(void);
	inline void I2CMRsend(char data);
	inline char I2CMRread(void);
	inline char I2CMRreadLast(void);
	inline void I2CMRstop(void);
		
		
		
	//==============================================================================
	//	INLINE	Функции конвертации из десятичного числа в двоично-десятичное
	//==============================================================================
		
	inline char BinToDec(char c)					// Перевод двоично-десятичного числа в десятичное
	{
		unsigned char ch = ((c >> 4) * 10 + (0b00001111 & c));
		return ch;
	}

	inline char DecToBin(char c)					// Перевод десятичного числа в двоично-десятичное
	{
		unsigned char ch = ((c / 10) << 4)|(c % 10);
		return ch;
	}	
		
		
	//==============================================================================
	//	INLINE					Функции протокола I2C
	//==============================================================================
		
	//  Функции мастера трансмиттера  //
		
	inline void I2CMTinit(void)
	{
		TWBR = 40;										// Задает скорость соединения. Чем больше тем медленнее; 255 -> 28кбит/с; 66 -> 100кбит/с; 11 -> 388кбит/с
		TWSR |= (0 << TWPS1)|(0 << TWPS0);				// Аналогично (00 - 11)
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
		while (!(TWCR & (1 << TWINT)));					// ожидание завершения операции
	}

	inline void I2CMTstop(void)
	{
		TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWSTO);	// Transmit STOP condition
	}
		
		
	//  Функции мастера рессивера  //
		
	inline void I2CMRinit(void)
	{
		TWBR = 66;										// Задает скорость соединения. Чем больше тем медленнее; 255 -> 28кбит/с; 66 -> 100кбит/с
		TWSR |= (0 << TWPS1)|(0 << TWPS0);				// Аналогично (00 - 11)
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
		while (!(TWCR & (1 << TWINT)));					// Ожидание завершения операции
	}
		
	inline char I2CMRread(void)
	{
		TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWEA);
		while (!(TWCR & (1 << TWINT)));					// Ожидание установки бита TWIN
		return TWDR;									// Читаем регистр данных
	}
	
	inline char I2CMRreadLast(void)
	{
		TWCR = (1 << TWINT)|(1 << TWEN);
		while (!(TWCR & (1 << TWINT)));					//ожидание установки бита TWIN
		return TWDR;									//читаем регистр данных
	}
		
	inline void I2CMRstop(void)
	{
		TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWSTO);	// Transmit STOP condition
	}	
}

#endif /* DS1307_H_ */

