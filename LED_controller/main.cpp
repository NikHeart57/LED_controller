#define F_CPU 14745600UL

#include <stdlib.h>						// включает функции - itoa() atoi()
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ST7789.h"
#include "DS1307.h"

// Массив для itoa
char itoa_temp[9];

// Флаги
bool second_update = true;
bool minute_update = true;
bool hour_update = true;

// Переменные кнопок
enum button_name
{
	button_PA0,
	button_PA1,
	button_PA2,
	button_PA3,
	button_NULL
};

char button_active = button_NULL;


// Переменные времени
enum time_name
{
	second,
	minute,
	hour,
	day_of_week,
	day,
	month,
	year
};

char time[7] = {0, 15, 21, 6, 13, 7, 24};
char old_time = time[second];


void Setup_TIM0(void);
void Setup_TIM1(void);
void Setup_INT(void);
void Setup_GPIO(void);


int main(void)
{
	_delay_ms(100);
	
	Setup_INT();
	Setup_GPIO();
	Setup_TIM1();
	
	//DS1307_WriteTime(time);
	DS1307_ReadTime(time);	
	
	ST7789_InitSPI();
	ST7789_InitST7789();
	ST7789_FillScreen(BLACK);
	ST7789_SetXYpos(0, 0);

	
	while (1)
	{			
		if (hour_update)
		{
			hour_update = false;
			ST7789_SetXYpos(0, 0);
			if (time[hour] < 10)
			{
				ST7789_PrintString((char*)"0");
			}
			ST7789_PrintString((char*)itoa(time[hour], itoa_temp, 10));
			ST7789_PrintString((char*)":");
		}
		
		if (minute_update)
		{
			minute_update = false;
			ST7789_SetXYpos(36, 0);
			if (time[minute] < 10)
			{
				ST7789_PrintString((char*)"0");
			}
			ST7789_PrintString((char*)itoa(time[minute], itoa_temp, 10));
			ST7789_PrintString((char*)":");
		}
		
		if (second_update)
		{
			second_update = false;
			ST7789_SetXYpos(72, 0);
			if (time[second] < 10)
			{
				ST7789_PrintString((char*)"0");
			}
			ST7789_PrintString((char*)itoa(time[second], itoa_temp, 10));
		}
	}
}

ISR(TIMER1_COMPA_vect)
{
	cli();
	
	time[second]++;
	second_update = true;

	if (time[second] >= 60)
	{
		time[second] = 0;
		time[minute]++;
		minute_update = true;
		
		if (time[minute] >= 60)
		{
			time[minute] = 0;
			time[hour]++;
			hour_update = true;
			
			if (time[hour] >= 24)
			{
				time[second] = 0;
				time[minute] = 0;
				time[hour] = 0;
			}
		}
	}
	
	sei();
}


ISR(INT0_vect)
{
	cli();
	
	char ERR_counter = 0;

	while (!(PINA == 254 || PINA == 253 || PINA == 251 || PINA == 247))		// Опрос порта A до тех пор пока не будет определена кнопка (типа против дребезга и некорректных значений в порте)
	{
		ERR_counter++;
		_delay_ms(1);
		
		if (ERR_counter > 20)												// Если после 20 опросов нет значения кнопки то выход из прерывания																					
		{				
			return;
		}
	}

	switch (PINA)															// Запоминание нажатой кнопки
	{
		case 254:
			button_active = button_PA0;
		break;
		case 253:
			button_active = button_PA1;
		break;
		case 251:
			button_active = button_PA2;
		break;
		case 247:
			button_active = button_PA3;
		break;
		default:
			button_active = button_NULL;
			return;
		break;
	}	

	ST7789_SetXYpos(0, 0);
	ST7789_PrintString((char*) itoa(button_active, itoa_temp, 10));
	
	ST7789_SetXYpos(0, 32);
	ST7789_PrintString((char*)"    ");
	ST7789_SetXYpos(0, 32);
	ST7789_PrintString((char*) itoa((int)ERR_counter, itoa_temp, 10));
	
	sei();
}


void Setup_TIM0(void)
{
	//////////// Таймер 0 (8 бит) PWM ////////////
	OCR0  |= 0;			// Скважность - Значение сравнения f = fcpu/(N*256) = 7372800/256 = 28800 || = 14745600/256 = 57600 | 248 -> 97.1% | 249 -> 97.5% | 250 -> 97.9% | 251 -> 98.3% | 252 -> 98.7%  | *по расчету надо 97,5 - 98,6%
	TCCR0 |= (0 << FOC0)|(1  << WGM01)|(1 << WGM00)|(1 << COM01)|(0 << COM00)|(0 << CS02)|(0 << CS01)|(1 << CS00);	// WGM - fast PWM, COM - clear on compare, CS - прескелер, FOC - ?
	
	DDRB |= 0b00001000;		// Вывод ШИМ - PB3(OC0)
	
	sei();
}


void Setup_TIM1(void)
{
	//////////// Таймер 1 (16 бит) Часы ////////////
	OCR1A   = 14398;	// Запись значения прерывания CTC производится до инициализации таймера
	TCCR1A |= (0 << COM1A1)|(0 << COM1A0)|(0 << COM1B1)|(0 << COM1B0)|(0 << FOC1A)|(0 << FOC1B)|(0 << WGM11)|(0 << WGM10);	// COM - порты, FOC - ?, WGM - режим CTC, CS - прескелер
	TCCR1B |= (0 <<  ICNC1)|(0 <<  ICES1)|(0 <<  WGM13)|(1 <<  WGM12)|(1 <<  CS12)|(0 <<  CS11)|(1 <<  CS10);				// IC - настройки пина ICP1 (PD6)
	TIMSK  |= (0 << TICIE1)|(1 << OCIE1A)|(0 << OCIE1B)|(0 << TOIE1);														// OCIE1A - Прерывание по совпадению разрешено
	
	sei();
}

void Setup_INT(void)
{
	GICR |= (0 << INT1)|(1 << INT0)|(0 << INT2);						// General Interrupt Control Register - Установка битов INT1, INT0 или INT2 разрешает прерывания при возникновении события на соответствующем выводе микроконтроллера AVR, а сброс — запрещает.
	MCUCR |= (1 << ISC11)|(0 << ISC10)|(1 << ISC01)|(0 << ISC00);		// 10	- Перывание по спадающему фронту INT0, INT1
	MCUCSR |= (0 << ISC2);												// 0	- Перывание по спадающему фронту INT2
	
	sei();
}

void Setup_GPIO(void)
{
	PORTA = 0xff;						// Все единицы
	DDRA = 0x00;						// Все вход
}
