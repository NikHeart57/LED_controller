#define F_CPU 14745600UL

#include <stdlib.h>					// itoa() atoi()
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "DS1307.h"
#include "ST7789.h"

// Флаги
unsigned char flagsync = 1;
unsigned char flagsetting = 11;
unsigned char flagclockupdate = 1;
unsigned char flagwordupdate = 1;
unsigned char flagsettingwordupdate = 0;
unsigned char flagsettingwordupdate2 = 0;
unsigned char flagsceduleupdate = 1;

// Переменные экрана
char xpos;
char ypos;
char buffer[9];
char red = 60;
char green = 55;
char blue = 45;

// Переменные времени
char time[7] = {0, 0, 8, 4, 23, 5, 24};												// sec, min, hour, day, date, month, year
char timesetting[7];
float timeDec = (float)time[0] + (float)time[1] / 60.0 + (float)time[2] / 3600.0;	// Реальное время в десятичном виде


char timeschedule[4][2] =						// Расписание
{
	{8, 00},
	{10, 00},
	{18, 00},
	{20, 00},
};


float timescheduleDec[4]=
{
	(float)timeschedule[0][0] + (float)timeschedule[0][1] / 60.0,
	(float)timeschedule[1][0] + (float)timeschedule[1][1] / 60.0,
	(float)timeschedule[2][0] + (float)timeschedule[2][1] / 60.0,
	(float)timeschedule[3][0] + (float)timeschedule[3][1] / 60.0,
};


char wordschedule[4][8] = {"Рассвет", "День   ", "Закат  ", "Ночь   "};



// Функции
void setup(void);
void printNumber(char number, char* buffer, char* xpos, char* ypos, char red, char green, char blue, char size);
inline void reverse(char* str, int len);
inline int intToStr(int x, char str[], int d);
char* ftoa(float n, char* res, int afterpoint);





int main(void)
{
	_delay_ms(100);
	setup();
	ST7789::fillScreen(0, 0, 0);
	
	
	DS1307::readTime(time);
	
	
	
	/*
	*	Слова в расписании оформить отдельными функциями чтобы они н робновлялись каждый раз вместе с основной прораммой
	*	вообще один раз их печатать и все
	*/
	
	while (1)
	{
		/*	
		xpos = 210;
		ypos = 210;
		ST7789::printString("  ", &xpos, &ypos, 10, 60, 0, 2);
		
		xpos = 210;
		ypos = 210;
		ST7789::printString(itoa(flagsetting, buffer, 10), &xpos, &ypos, 10, 60, 0, 2);
		
		char i;
		
		xpos = 170;
		ypos = 210;
		ST7789::printString("   ", &xpos, &ypos, 40, 40, 0, 2);
		
		xpos = 170;
		ypos = 210;
		ST7789::printString(itoa(i++, buffer, 10), &xpos, &ypos, 40, 40, 0, 2);
		
		red = 20 + rand() % 40;
		green = 20 + rand() % 40;
		blue = 20 + rand() % 40;
		*/
		
		//==============================================================================
		//									Блок времени
		//==============================================================================
		
		if (flagclockupdate)
		{
			flagclockupdate--;
			
			xpos = 0;
			ypos = 0;
			
			if (flagsetting != 2 && flagsetting != 1 && flagsetting != 0)					// Нормальное отображение
			{
				printNumber(time[2], buffer, &xpos, &ypos, red, green, blue, 5);
				ST7789::printString(":", &xpos, &ypos, red, green, blue, 5);
				printNumber(time[1], buffer, &xpos, &ypos, red, green, blue, 5);
				ST7789::printString(":", &xpos, &ypos, red, green, blue, 5);
				printNumber(time[0], buffer, &xpos, &ypos, red, green, blue, 5);
			}
			else 																		// "Настроечное" отображение
			{
				if (flagsetting == 2)
				{
					xpos = 0;
					ypos = 0;
					printNumber(timesetting[2], buffer, &xpos, &ypos, red, 0, 0, 5);
				}
				else
				{
					xpos = 0;
					ypos = 0;
					printNumber(timesetting[2], buffer, &xpos, &ypos, red, green, blue, 5);
				}
				
				if (flagsetting == 1)
				{
					xpos = 90;
					ypos = 0;
					printNumber(timesetting[1], buffer, &xpos, &ypos, red, 0, 0, 5);
				}
				else
				{
					xpos = 90;
					ypos = 0;
					printNumber(timesetting[1], buffer, &xpos, &ypos, red, green, blue, 5);
				}
				

				if (flagsetting == 0)
				{
					xpos = 180;
					ypos = 0;
					printNumber(timesetting[0], buffer, &xpos, &ypos, red, 0, 0, 5);
				}
				else
				{
					xpos = 180;
					ypos = 0;
					printNumber(timesetting[0], buffer, &xpos, &ypos, red, green, blue, 5);
				}
			}
		}
		
		
		//==============================================================================
		//						   Блок время суток / настройка
		//==============================================================================
		
		if(flagwordupdate)
		{
			flagwordupdate--;
			
			xpos = 0;
			ypos = 45;
			
			if (flagsetting == 11)
			{
				flagsettingwordupdate = 1;
				
				if (flagsettingwordupdate2)
				{
					flagsettingwordupdate2--;
					ST7789::drawSquare(0, 40, 0, 0, 0, 80);
					ST7789::drawSquare(80, 40, 0, 0, 0, 80);
					ST7789::drawSquare(160, 40, 0, 0, 0, 80);
					
				}
				
				// Расчеты времён
				timeDec = (float)time[2] + (float)time[1] / 60.0 + (float)time[0] / 3600.0;
				
				for(char i = 0; i < 4; i++)
				{
					timescheduleDec[i] = (float)timeschedule[i][0] + (float)timeschedule[i][1] / 60.0;
				}
				
				// Определение слова и скважности
				if (timeDec < timescheduleDec[0])													// Время меньше чем время рассвета = ночь - 3
				{
					ST7789::printString(wordschedule[3], &xpos, &ypos, red, green, blue, 5);
					OCR0 = 0;
				}
				else if (timeDec < timescheduleDec[1])												// Время меньше чем время начала дня = рассвет - 0
				{
					ST7789::printString(wordschedule[0], &xpos, &ypos, red, green, blue, 5);
					OCR0 = (char)(((timeDec - timescheduleDec[0])/(timescheduleDec[1] - timescheduleDec[0])) * 255.0);
				}
				else if (timeDec < timescheduleDec[2])												// Время меньше чем время конца дня = день - 1
				{
					ST7789::printString(wordschedule[1], &xpos, &ypos, red, green, blue, 5);
					OCR0 = 255;
				}
				else if (timeDec < timescheduleDec[3])												// Время меньше чем время конца заката = закат - 2
				{
					ST7789::printString(wordschedule[2], &xpos, &ypos, red, green, blue, 5);
					OCR0 = (char)((1.0 - ((timeDec - timescheduleDec[2])/(timescheduleDec[3] - timescheduleDec[2]))) * 255.0);
				}
				else																				// В остальных случаях ночь - 3
				{
					ST7789::printString(wordschedule[3], &xpos, &ypos, red, green, blue, 5);
					OCR0 = 0;
				}
				
				xpos = 0;
				ypos = 90;
				
				ST7789::printString("осв.", &xpos, &ypos, red, green, blue, 3);						// Печать осв.
				
				if ((float)OCR0/2.55 < 10)															// Печать процента
				{
					ST7789::printString("0", &xpos, &ypos, red, green, blue, 3);
				}
				
				ST7789::printString(ftoa(((float)OCR0/2.55), buffer, 2), &xpos, &ypos, red, green, blue, 3);
				ST7789::printString("%  ", &xpos, &ypos, red, green, blue, 3);
			}
			else if(flagsettingwordupdate)
			{
				flagsettingwordupdate--;
				flagsettingwordupdate2 = 1;
				ST7789::drawSquare(0, 40, 0, 0, 0, 80);
				ST7789::drawSquare(80, 40, 0, 0, 0, 80);
				ST7789::drawSquare(160, 40, 0, 0, 0, 80);
				
				xpos = 9;
				ypos = 70;
				ST7789::printString("Настройка", &xpos, &ypos, red, green, blue, 4);
				
				/*
				if(flagsetting == 0 || flagsetting == 1 || flagsetting == 2)
				{
					xpos = 12;
					ypos = 50;
					ST7789::printString("Настройка", &xpos, &ypos, red, green, blue, 4);
					
					xpos = 36;
					ypos = 83;
					ST7789::printString("времени", &xpos, &ypos, red, green, blue, 4);
				}
				
				if(flagsetting == 3 || flagsetting == 4 || flagsetting == 5 || flagsetting == 6 || flagsetting == 7 || flagsetting == 8 || flagsetting == 9 || flagsetting == 10)
				{
					xpos = 12;
					ypos = 50;
					ST7789::printString("Настройка", &xpos, &ypos, red, green, blue, 4);
					
					xpos = 36;
					ypos = 83;
					ST7789::printString("расписания", &xpos, &ypos, red, green, blue, 4);
				}
				*/
			}
		}
		
		
		//==============================================================================
		//								Блок расписания
		//==============================================================================
		
		if (flagsceduleupdate)
		{
			flagsceduleupdate--;
			
			xpos = 0;
			ypos = 135;
			
			if(flagsetting == 11)
			{
				
				printNumber(timeschedule[0][0], buffer, &xpos, &ypos, red, green, blue, 3);
				ST7789::printString(":", &xpos, &ypos, red, green, blue, 3);
				printNumber(timeschedule[0][1], buffer, &xpos, &ypos, red, green, blue, 3);
				ST7789::printString(" Рассвет", &xpos, &ypos, red, green, blue, 3);
			
				xpos = 0;
				ypos = 160;
				printNumber(timeschedule[1][0], buffer, &xpos, &ypos, red, green, blue, 3);
				ST7789::printString(":", &xpos, &ypos, red, green, blue, 3);
				printNumber(timeschedule[1][1], buffer, &xpos, &ypos, red, green, blue, 3);
				ST7789::printString(" День", &xpos, &ypos, red, green, blue, 3);
			
				xpos = 0;
				ypos = 185;
				printNumber(timeschedule[2][0], buffer, &xpos, &ypos, red, green, blue, 3);
				ST7789::printString(":", &xpos, &ypos, red, green, blue, 3);
				printNumber(timeschedule[2][1], buffer, &xpos, &ypos, red, green, blue, 3);
				ST7789::printString(" Закат", &xpos, &ypos, red, green, blue, 3);
			
				xpos = 0;
				ypos = 210;
				printNumber(timeschedule[3][0], buffer, &xpos, &ypos, red, green, blue, 3);
				ST7789::printString(":", &xpos, &ypos, red, green, blue, 3);
				printNumber(timeschedule[3][1], buffer, &xpos, &ypos, red, green, blue, 3);
				ST7789::printString(" Ночь", &xpos, &ypos, red, green, blue, 3);
			}
			else
			{
				
				if (flagsetting == 3)
				{
					xpos = 0;
					ypos = 135;
					printNumber(timeschedule[0][0], buffer, &xpos, &ypos, red, 0, 0, 3);
				}
				else
				{
					xpos = 0;
					ypos = 135;
					printNumber(timeschedule[0][0], buffer, &xpos, &ypos, red, green, blue, 3);
				}
				
				
				if (flagsetting == 4)
				{
					xpos = 54;
					ypos = 135;
					printNumber(timeschedule[0][1], buffer, &xpos, &ypos, red, 0, 0, 3);
				}
				else
				{
					xpos = 54;
					ypos = 135;
					printNumber(timeschedule[0][1], buffer, &xpos, &ypos, red, green, blue, 3);
				}
			
				if (flagsetting == 5)
				{
					xpos = 0;
					ypos = 160;
					printNumber(timeschedule[1][0], buffer, &xpos, &ypos, red, 0, 0, 3);
				}
				else
				{
					xpos = 0;
					ypos = 160;
					printNumber(timeschedule[1][0], buffer, &xpos, &ypos, red, green, blue, 3);
				}
			
				if (flagsetting == 6)
				{
					xpos = 54;
					ypos = 160;
					printNumber(timeschedule[1][1], buffer, &xpos, &ypos, red, 0, 0, 3);
				}
				else
				{
					xpos = 54;
					ypos = 160;
					printNumber(timeschedule[1][1], buffer, &xpos, &ypos, red, green, blue, 3);
				}
			
				if (flagsetting == 7)
				{
					xpos = 0;
					ypos = 185;
					printNumber(timeschedule[2][0], buffer, &xpos, &ypos, red, 0, 0, 3);
				}
				else
				{
					xpos = 0;
					ypos = 185;
					printNumber(timeschedule[2][0], buffer, &xpos, &ypos, red, green, blue, 3);
				}
			
				if (flagsetting == 8)
				{
					xpos = 54;
					ypos = 185;
					printNumber(timeschedule[2][1], buffer, &xpos, &ypos, red, 0, 0, 3);
				}
				else
				{
					xpos = 54;
					ypos = 185;
					printNumber(timeschedule[2][1], buffer, &xpos, &ypos, red, green, blue, 3);
				}
			
				if (flagsetting == 9)
				{
					xpos = 0;
					ypos = 210;
					printNumber(timeschedule[3][0], buffer, &xpos, &ypos, red, 0, 0, 3);
				}
				else
				{
					xpos = 0;
					ypos = 210;
					printNumber(timeschedule[3][0], buffer, &xpos, &ypos, red, green, blue, 3);
				}
			
				if (flagsetting == 10)
				{
					xpos = 54;
					ypos = 210;
					printNumber(timeschedule[3][1], buffer, &xpos, &ypos, red, 0, 0, 3);
				}
				else
				{
					xpos = 54;
					ypos = 210;
					printNumber(timeschedule[3][1], buffer, &xpos, &ypos, red, green, blue, 3);
				}
			}
		}
	}
}





ISR(TIMER1_COMPA_vect)
{
	cli();	
	time[0]++;						// Инкремент секунды
	
	if (time[0] >= 60)
	{
		time[1]++;					// Инкремент минуты
		time[0] = 0;
	}
	
	if (time[1] >= 60)
	{
		time[2]++;
		time[1] = 0;				// Инкремент часа
	}
	
	if (time[2] >= 24)				// Сутки
	{
		time[0] = 0;
		time[1] = 0;
		time[2] = 0;
	}
	
	
	// Синхронизация с DS1307 в полночь
	if (time[0] == 0 && time[1] == 0 && time[2] == 0 && flagsync == 1)
	{
		DS1307::readTime(time);
		flagsync = 0;
	}
	
	if (time[0] == 0 && time[1] == 1 && time[2] == 0)
	{
		flagsync = 1;
	}
	
	flagclockupdate = 1;
	flagwordupdate = 1;
	
	sei();
}

ISR(INT0_vect)
{
	cli();
	
	_delay_ms(20);
	if (PIND & (1 << PIND2))		// Если высокий уровень
	{
		return;
	}
	
	if (flagsetting == 0)
	{
		timesetting[0]++;
		
		if (timesetting[0] >= 60)
		{
			timesetting[0] = 0;
		}
		
		flagclockupdate = 1;
	}

	if (flagsetting == 1)
	{
		timesetting[1]++;
		
		if (timesetting[1] >= 60)
		{
			timesetting[1] = 0;
		}
		
		flagclockupdate = 1;
	}

	if (flagsetting == 2)
	{
		timesetting[2]++;
		
		if (timesetting[2] >= 24)
		{
			timesetting[2] = 0;
		}
		
		flagclockupdate = 1;
	}

	if (flagsetting == 3)
	{
		
		if ((timeschedule[0][0] + 1) * 60 + timeschedule[0][1] <= timeschedule[1][0] * 60 + timeschedule[1][1])
		{
			timeschedule[0][0]++;
		}
		
		if (timeschedule[0][0] >= 24)
		{
			timeschedule[0][0] = 0;
		}
		
		flagsceduleupdate = 1;
	}

	if (flagsetting == 4)
	{
		
		if (timeschedule[0][0] * 60 + timeschedule[0][1] + 2 <= timeschedule[1][0] * 60 + timeschedule[1][1])
		{
			timeschedule[0][1]++;
		}
		
		if (timeschedule[0][1] >= 60)
		{
			timeschedule[0][1] = 0;
		}
		
		flagsceduleupdate = 1;
	}

	if (flagsetting == 5)
	{
		if ((timeschedule[1][0] + 1) * 60 + timeschedule[1][1] <= timeschedule[2][0] * 60 + timeschedule[2][1])
		{
			timeschedule[1][0]++;
		}
		
		
		if (timeschedule[1][0] >= 24)
		{
			timeschedule[1][0] = 0;
		}
		
		flagsceduleupdate = 1;
	}

	if (flagsetting == 6)
	{
		
		if (timeschedule[1][0] * 60 + timeschedule[1][1] + 2 <= timeschedule[2][0] * 60 + timeschedule[2][1])
		{
			timeschedule[1][1]++;
		}
		
		
		if (timeschedule[1][1] >= 60)
		{
			timeschedule[1][1] = 0;
		}
		
		flagsceduleupdate = 1;
	}

	if (flagsetting == 7)
	{
		timeschedule[2][0]++;
		
		if (timeschedule[2][0] >= 24)
		{
			timeschedule[2][0] = 0;
		}
		
		flagsceduleupdate = 1;
	}

	if (flagsetting == 8)
	{
		timeschedule[2][1]++;
		
		if (timeschedule[2][1] >= 60)
		{
			timeschedule[2][1] = 0;
		}
		
		flagsceduleupdate = 1;
	}

	if (flagsetting == 9)
	{
		timeschedule[3][0]++;
		
		if (timeschedule[3][0] >= 24)
		{
			timeschedule[3][0] = 0;
		}
		
		flagsceduleupdate = 1;
	}

	if (flagsetting == 10)
	{
		timeschedule[3][1]++;
		
		if (timeschedule[3][1] >= 60)
		{
			timeschedule[3][1] = 0;
		}
		
		flagsceduleupdate = 1;
	}

	if (flagsetting == 11)
	{
		return;
	}
	
	PORTA = 1;
	_delay_ms(30);
	PORTA = 0;
	
	sei();
}

ISR(INT1_vect)
{
	cli();
	
	_delay_ms(20);
	if (PIND & (1 << PIND3))		// Если высокий уровень
	{
		return;
	}
	
	if (flagsetting == 0)
	{
		timesetting[0]--;
		
		if (timesetting[0] >= 60)
		{
			timesetting[0] = 59;
		}
		
		flagclockupdate = 1;
	}
	
	if (flagsetting == 1)
	{
		timesetting[1]--;
		
		if (timesetting[1] >= 60)
		{
			timesetting[1] = 59;
		}
		
		flagclockupdate = 1;
	}
	
	if (flagsetting == 2)
	{
		timesetting[2]--;
		
		if (timesetting[2] >= 24)
		{
			timesetting[2] = 23;
		}
		
		flagclockupdate = 1;
	}
	
	if (flagsetting == 3)
	{
		timeschedule[0][0]--;
		
		if (timeschedule[0][0] >= 24)
		{
			timeschedule[0][0] = 0;
		}
		
		flagsceduleupdate = 1;
	}
	
	if (flagsetting == 4)
	{
		timeschedule[0][1]--;
		
		if (timeschedule[0][1] >= 24)
		{
			timeschedule[0][1] = 0;
		}
		
		flagsceduleupdate = 1;
	}
	
	if (flagsetting == 5)
	{
		timeschedule[1][0]--;
		
		if (timeschedule[1][0] >= 24)
		{
			timeschedule[1][0] = 0;
		}
		
		flagsceduleupdate = 1;
	}
	
	if (flagsetting == 6)
	{
		timeschedule[1][1]--;
		
		if (timeschedule[1][1] >= 24)
		{
			timeschedule[1][1] = 0;
		}
		
		flagsceduleupdate = 1;
	}
	
	if (flagsetting == 7)
	{
		timeschedule[2][0]--;
		
		if (timeschedule[2][0] >= 24)
		{
			timeschedule[2][0] = 0;
		}
		
		flagsceduleupdate = 1;
	}
	
	if (flagsetting == 8)
	{
		timeschedule[2][1]--;
		
		if (timeschedule[2][1] >= 24)
		{
			timeschedule[2][1] = 0;
		}
		
		flagsceduleupdate = 1;
	}
	
	if (flagsetting == 9)
	{
		timeschedule[3][0]--;
		
		if (timeschedule[3][0] >= 24)
		{
			timeschedule[3][0] = 0;
		}
		
		flagsceduleupdate = 1;
	}
	
	if (flagsetting == 10)
	{
		timeschedule[3][1]--;
		
		if (timeschedule[3][1] >= 24)
		{
			timeschedule[3][1] = 0;
		}
		
		flagsceduleupdate = 1;
	}
	
	if (flagsetting == 11)
	{
		return;
	}
	
	PORTA = 1;
	_delay_ms(30);
	PORTA = 0;
	
	sei();
}

ISR(INT2_vect)
{
	cli();
	
	_delay_ms(20);
	if (PINB & (1 << PINB2))		// Если высокий уровень
	{
		return;
	}
	
	
	// Инкремент флага настроек
	flagsetting++;
	if (flagsetting >= 12)
	{
		flagsetting = 0;
	}
	
	
	// Установка флагов отображения. от 0 до 2 флаги обновления часов, от 3 до 11 обновление расписания
	if (flagsetting <= 3)
	{
		flagclockupdate = 1;
	}
	
	if (flagsetting >= 3)
	{
		flagsceduleupdate = 2;
	}
	
	if (flagsetting == 0)
	{
		flagwordupdate = 1;
	}
	
	
	// Особые действия по началу и завершению настроек часов
	if (flagsetting == 0)					// При начале настройки часов загрузить текущее время в буффер
	{
		for(int i = 0; i < 7; i++)
		{
			timesetting[i] = time[i];
		}
	}
	
	if (flagsetting == 3)					// При окончании настройки часов выгрузить время из буффера
	{
		for(int i = 0; i < 7; i++)
		{
			time[i] = timesetting[i];
		}
		DS1307::writeTime(time);
	}

	
	PORTA = 1;
	_delay_ms(30);
	PORTA = 0;
	
	sei();
}


void setup(void)
{
	sei();
	
	////////////// Внешние прерывания //////////////
	GICR |= (1 << INT1)|(1 << INT0)|(1 << INT2);						// General Interrupt Control Register - Установка битов INT1, INT0 или INT2 разрешает прерывания при возникновении события на соответствующем выводе микроконтроллера AVR, а сброс — запрещает.
	MCUCR |= (1 << ISC11)|(0 << ISC10)|(1 << ISC01)|(0 << ISC00);		// 10	- Перывание по спадающему фронту INT0, INT1
	MCUCSR |= (0 << ISC2);												// 0	- Перывание по спадающему фронту INT2
	
	DDRA = 0xff;
	PORTA = 0;
	
	
	//////////// Таймер 1 (16 бит) Часы ////////////
	OCR1A   = 14398;	// Запись значения прерывания CTC производится до инициализации таймера
	TCCR1A |= (0 << COM1A1)|(0 << COM1A0)|(0 << COM1B1)|(0 << COM1B0)|(0 << FOC1A)|(0 << FOC1B)|(0 << WGM11)|(0 << WGM10);	// COM - порты, FOC - ?, WGM - режим CTC, CS - прескелер
	TCCR1B |= (0 <<  ICNC1)|(0 <<  ICES1)|(0 <<  WGM13)|(1 <<  WGM12)|(1 <<  CS12)|(0 <<  CS11)|(1 <<  CS10);				// IC - настройки пина ICP1 (PD6)
	TIMSK  |= (0 << TICIE1)|(1 << OCIE1A)|(0 << OCIE1B)|(0 << TOIE1);														// OCIE1A - Прерывание по совпадению разрешено
	

	//////////// Таймер 0 (8 бит) PWM ////////////
	OCR0  |= 0;			// Скважность - Значение сравнения f = fcpu/(N*256) = 7372800/256 = 28800 || = 14745600/256 = 57600 | 248 -> 97.1% | 249 -> 97.5% | 250 -> 97.9% | 251 -> 98.3% | 252 -> 98.7%  | *по расчету надо 97,5 - 98,6%
	TCCR0 |= (0 << FOC0)|(1  << WGM01)|(1 << WGM00)|(1 << COM01)|(0 << COM00)|(0 << CS02)|(0 << CS01)|(1 << CS00);	// WGM - fast PWM, COM - clear on compare, CS - прескелер, FOC - ?
	
	DDRB |= 0b00001000;		// Вывод ШИМ - PB3(OC0)
	
	ST7789::init();
}


void printNumber(char number, char* buffer, char* xpos, char* ypos, char red, char green, char blue, char size)
{
	if (number < 10)
	{
		ST7789::printString("0", xpos, ypos, red, green, blue, size);
	}
	ST7789::printString(itoa(number, buffer, 10), xpos, ypos, red, green, blue, size);
}


// Reverses a string 'str' of length 'len'
inline void reverse(char* str, int len)
{
	int i = 0, j = len - 1, temp;
	while (i < j) {
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++;
		j--;
	}
}

// Converts a given integer x to string str[].
// d is the number of digits required in the output.
// If d is more than the number of digits in x,
// then 0s are added at the beginning.
inline int intToStr(int x, char str[], int d)
{
	int i = 0;
	while (x) {
		str[i++] = (x % 10) + '0';
		x = x / 10;
	}
	
	// If number of digits required is more, then
	// add 0s at the beginning
	while (i < d)
	str[i++] = '0';
	
	reverse(str, i);
	str[i] = '\0';
	return i;
}

// Converts a floating-point/double number to a string.
char* ftoa(float n, char* res, int afterpoint)
{
	// Extract integer part
	int ipart = (int)n;
	
	// Extract floating part
	float fpart = n - (float)ipart;
	
	// convert integer part to string
	int i = intToStr(ipart, res, 0);
	
	// check for display option after point
	if (afterpoint != 0) {
		res[i] = '.'; // add dot
		
		// Get the value of fraction part upto given no.
		// of points after dot. The third parameter
		// is needed to handle cases like 233.007
		fpart = fpart * pow(10, afterpoint);
		
		intToStr((int)fpart, res + i + 1, afterpoint);
	}
	
	return res;
}