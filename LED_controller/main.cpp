#define F_CPU 14745600UL

#include <stdlib.h>					// itoa() atoi()
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "DS1307.h"
#include "ST7789.h"

// Переменные экрана
char xpos;
char ypos;
char buffer[9];
char red = 60;
char green = 55;
char blue = 45;

// Переменные времени
char time[7] = {40, 59, 19, 2, 21, 5, 24};											// sec, min, hour, day, date, month, year
float timeDec = (float)time[0] + (float)time[1] / 60.0 + (float)time[2] / 3600.0;	// Реальное время в десятичном виде
char timecomp = time[0];															// Секунда сравнения

char timeschedule[4][2] =						// Расписание
{
	{5, 00},
	{10, 00},
	{17, 00},
	{22, 00},
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
void printClock(void);
void printSchedule(void);
inline void reverse(char* str, int len);
inline int intToStr(int x, char str[], int d);
char* ftoa(float n, char* res, int afterpoint);

int main(void)
{
	_delay_ms(100);
	setup();
	ST7789::fillScreen(0, 0, 0);
	
	//DS1307::writeTime(time);
	_delay_ms(50);
	
	DS1307::readTime(time);
		
	while (1) 
    {
		while (time[0] == timecomp){}
		timecomp = time[0];	
		
		xpos = 0;
		ypos = 0;
		printClock();
		
		
		
		xpos = 0;
		ypos = 40;
		
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
		ypos = 85;
		
		ST7789::printString(itoa(OCR0, buffer, 10), &xpos, &ypos, red, green, blue, 3);
		ST7789::printString(" ", &xpos, &ypos, red, green, blue, 3);
		
		if ((float)OCR0/2.55 < 10)
		{
			ST7789::printString("0", &xpos, &ypos, red, green, blue, 3);
		}
		
		ST7789::printString(ftoa(((float)OCR0/2.55), buffer, 2), &xpos, &ypos, red, green, blue, 3);
		ST7789::printString("%  ", &xpos, &ypos, red, green, blue, 3);
		
		xpos = 0;
		ypos = 130;
		printSchedule();
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
	if (time[0] == 0 && time[1] == 0 && time[2] == 0)
	{
		DS1307::readTime(time);
	}
	
	
	sei();
}

void setup(void)
{
	sei();
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

void printClock(void)
{
	if (time[2] < 10)
	{
		ST7789::printString("0", &xpos, &ypos, red, green, blue, 5);
	}
	ST7789::printString(itoa(time[2], buffer, 10), &xpos, &ypos, red, green, blue, 5);
	ST7789::printString(":", &xpos, &ypos, red, green, blue, 5);
	
	if (time[1] < 10)
	{
		ST7789::printString("0", &xpos, &ypos, red, green, blue, 5);
	}
	ST7789::printString(itoa(time[1], buffer, 10), &xpos, &ypos, red, green, blue, 5);
	ST7789::printString(":", &xpos, &ypos, red, green, blue, 5);
	
	if (time[0] < 10)
	{
		ST7789::printString("0", &xpos, &ypos, red, green, blue, 5);
	}
	ST7789::printString(itoa(time[0], buffer, 10), &xpos, &ypos, red, green, blue, 5);
	
}

void printSchedule(void)
{
	for(char i = 0; i < 4; i++)
	{
		// Первое число
		if (timeschedule[i][0] < 10)
		{
			ST7789::printString("0", &xpos, &ypos, red, green, blue, 3);
		}
		ST7789::printString(itoa(timeschedule[i][0], buffer, 10), &xpos, &ypos, red, green, blue, 3);
		ST7789::printString(":", &xpos, &ypos, red, green, blue, 3);
		
		
		// Второе число
		if (timeschedule[i][1] < 10)
		{
			ST7789::printString("0", &xpos, &ypos, red, green, blue, 3);
		}
		ST7789::printString(itoa(timeschedule[i][1], buffer, 10), &xpos, &ypos, red, green, blue, 3);
		ST7789::printString(" ", &xpos, &ypos, red, green, blue, 3);
		
		// Слово
		ST7789::printString(wordschedule[i], &xpos, &ypos, red, green, blue, 3);
			
		xpos = 0;
		ypos += 26;
	}
	ypos -= 26;
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