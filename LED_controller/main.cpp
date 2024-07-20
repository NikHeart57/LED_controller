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
bool update_sync_DS1307		= true;

bool update_second			= true;
bool update_minute			= true;
bool update_hour			= true;
bool update_clock			= false;	// Эта переменная нужна для обновления с красного шрифта на белый при прерключении между параметрами при настройке

bool update_hour_sunrise	= true;
bool update_minute_sunrise	= true;
bool update_hour_day		= true;
bool update_minute_day		= true;
bool update_hour_sunset		= true;
bool update_minute_sunset	= true;
bool update_hour_night		= true;
bool update_minute_night	= true;
bool update_schedule		= false;	// Аналогично для расписания
	


enum button_enum
{
	none					= 0,
	increase				= 1,
	decrease				= 2,
	clock					= 3,
	schedule				= 4
};

enum button_enum button = none;

enum mode_enum
{
	normal_work				= 0,
	setup_second			= 1,
	setup_minute			= 2,
	setup_hour				= 3,
	setup_hour_sunrise		= 4,
	setup_minute_sunrise	= 5,
	setup_hour_day			= 6,
	setup_minute_day		= 7,
	setup_hour_sunset		= 8,
	setup_minute_sunset		= 9,
	setup_hour_night		= 10,
	setup_minute_night		= 11
};

enum mode_enum mode = normal_work;



// Переменные времени
enum time_enum
{
	second			= 0,
	minute			= 1,
	hour			= 2,
	day_of_week		= 3,
	day				= 4,
	month			= 5,
	year			= 6
};	

char time[year + 1] = {0, 0, 0, 1, 1, 1, 24};	
char time_temp[hour + 1] = {time[second], time[minute], time[hour]};	
	
char time_schedule[4][2] =
{
	{10, 00},
	{12, 00},
	{18, 00},
	{20, 00}		
};
	


// Прототипы функций
void Setup_TIM0(void);
void Setup_TIM1(void);
void Setup_INT(void);
void Setup_GPIO(void);

void Buttons_Handler(void);


int main(void)
{
	_delay_ms(100);
		
	Setup_INT();
	Setup_GPIO();
	Setup_TIM1();
	
	DS1307_ReadTime(time);	
	
	ST7789_InitSPI();
	ST7789_InitST7789();
	ST7789_FillScreen(BLACK);
	ST7789_SetXYpos(0, 0);

	colour WHITE1;

	while (1)
	{	
		WHITE1.red = rand() % 30;
		WHITE1.green = rand() % 60;
		WHITE1.blue = rand() % 30;
		
		Buttons_Handler();
		
		//==============================================================================
		//							Режим простого времени (0)
		//==============================================================================
		
		/* 
				**Схема**
		if(mode != normal_work && update)
		{
			update = false;
			** Код нормы **
		}
		else if(mode == normal_work && update)
		{
			update = false;	
			** Код НЕнормы **	
		}
		*/
		
		
		//==============================================================================
		//							Режим настройки секунд (1)
		//==============================================================================
		
		/* 
				**Схема**
		if(mode != setup_second && update_second)			// Сначала то что нужно делать в обычном режиме
		{
			update_second = false;
			** Код нормы **
		}
		else if(mode == setup_second && update_second)		// Потом то что надо делать в необычном режиме
		{
			update_second = false;
			** Код НЕнормы **		
		}
		*/
		
		if (mode != setup_second && update_second)
		{
			update_second = false;
			
			ST7789_SetXYpos(180, 0);
			if (time[second] < 10)
			{
				ST7789_PrintString((char*)"0", WHITE1, 5);
			}			
			ST7789_PrintString((char*)itoa(time[second], itoa_temp, 10), WHITE1, 5);
		}
		else if (mode == setup_second && update_second)
		{
			update_second = false;
			
			ST7789_SetXYpos(180, 0);
			if (time_temp[second] < 10)
			{
				ST7789_PrintString((char*)"0", RED, 5);
			}
			ST7789_PrintString((char*)itoa(time_temp[second], itoa_temp, 10), RED, 5);
			
			if (update_clock)
			{
				update_clock = false;
				/*	Убрал для экономии ресурса обновления монитора. Этот код избыточный, хотя и особо не мешает
				ST7789_SetXYpos(0, 0);
				if (time_temp[hour] < 10)
				{
					ST7789_PrintString((char*)"0", WHITE1, 5);
				}
				ST7789_PrintString((char*)itoa(time_temp[hour], itoa_temp, 10), WHITE1, 5);
				
				ST7789_SetXYpos(90, 0);
				if (time_temp[minute] < 10)
				{
					ST7789_PrintString((char*)"0", WHITE1, 5);
				}
				ST7789_PrintString((char*)itoa(time_temp[minute], itoa_temp, 10), WHITE1, 5);
				*/
			}
		}
		
		
		//==============================================================================
		//							Режим настройки минут (2)
		//==============================================================================
		
		
		if (mode != setup_minute && update_minute)
		{
			update_minute = false;
			
			ST7789_SetXYpos(90, 0);
			if (time[minute] < 10)
			{
				ST7789_PrintString((char*)"0", WHITE1, 5);
			}
			ST7789_PrintString((char*)itoa(time[minute], itoa_temp, 10), WHITE1, 5);
			ST7789_PrintString((char*)":", WHITE1, 5);		
		}
		else if (mode == setup_minute && update_minute)
		{
			update_minute = false;
			
			ST7789_SetXYpos(90, 0);
			if (time_temp[minute] < 10)
			{
				ST7789_PrintString((char*)"0", RED, 5);
			}
			ST7789_PrintString((char*)itoa(time_temp[minute], itoa_temp, 10), RED, 5);
			
			if (update_clock)
			{
				update_clock = false;
				/*	Убрал для экономии ресурса обновления монитора. Этот код избыточный, хотя и особо не мешает
				ST7789_SetXYpos(0, 0);
				if (time_temp[hour] < 10)
				{
					ST7789_PrintString((char*)"0", WHITE1, 5);
				}
				ST7789_PrintString((char*)itoa(time_temp[hour], itoa_temp, 10), WHITE1, 5);
				*/
				ST7789_SetXYpos(180, 0);
				if (time_temp[second] < 10)
				{
					ST7789_PrintString((char*)"0", WHITE1, 5);
				}
				ST7789_PrintString((char*)itoa(time_temp[second], itoa_temp, 10), WHITE1, 5);
			}
		}	
		
		
		//==============================================================================
		//							Режим настройки часов (3)
		//==============================================================================
		
		if (mode != setup_hour && update_hour)
		{
			update_hour = false;
			
			ST7789_SetXYpos(0, 0);
			if (time[hour] < 10)
			{
				ST7789_PrintString((char*)"0", WHITE1, 5);
			}
			ST7789_PrintString((char*)itoa(time[hour], itoa_temp, 10), WHITE1, 5);
			ST7789_PrintString((char*)":", WHITE1, 5);	
		}
		else if (mode == setup_hour && update_hour)
		{

			update_hour = false;
			
			ST7789_SetXYpos(0, 0);
			if (time_temp[hour] < 10)
			{
				ST7789_PrintString((char*)"0", RED, 5);
			}
			ST7789_PrintString((char*)itoa(time_temp[hour], itoa_temp, 10), RED, 5);
				
			if (update_clock)
			{
				update_clock = false;
					
				ST7789_SetXYpos(90, 0);
				if (time_temp[minute] < 10)
				{
					ST7789_PrintString((char*)"0", WHITE1, 5);
				}
				ST7789_PrintString((char*)itoa(time_temp[minute], itoa_temp, 10), WHITE1, 5);
				/*	Убрал для экономии ресурса обновления монитора. Этот код избыточный, хотя и особо не мешает
				ST7789_SetXYpos(180, 0);
				if (time_temp[second] < 10)
				{
					ST7789_PrintString((char*)"0", WHITE1, 5);
				}
				ST7789_PrintString((char*)itoa(time_temp[second], itoa_temp, 10), WHITE1, 5);
				*/
			}
		}	
		
		//==============================================================================
		//						Режим настройки расвета (4)
		//==============================================================================
		
		if (mode != setup_hour_sunrise && update_hour_sunrise)
		{
			update_hour_sunrise = false;
			
			ST7789_SetXYpos(0, 136);
			if (time_schedule[0][0] < 10)
			{
				ST7789_PrintString((char*)"0", WHITE1, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[0][0], itoa_temp, 10), WHITE1, 3);
		}
		else if (mode == setup_hour_sunrise && update_hour_sunrise)
		{
			update_hour_sunrise = false;
				
			ST7789_SetXYpos(0, 136);
			if (time_schedule[0][0] < 10)
			{
				ST7789_PrintString((char*)"0", RED, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[0][0], itoa_temp, 10), RED, 3);
				
			if (update_schedule)
			{
				update_schedule = false;
				
				ST7789_SetXYpos(0, 0);
				if (time[hour] < 10)
				{
					ST7789_PrintString((char*)"0", WHITE1, 5);
				}
				ST7789_PrintString((char*)itoa(time[hour], itoa_temp, 10), WHITE1, 5);
			}
		}

		
		//==============================================================================
		//						Режим настройки расвета (5)
		//==============================================================================
		
		if (mode != setup_minute_sunrise && update_minute_sunrise)
		{
			update_minute_sunrise = false;
			
			ST7789_SetXYpos(54, 136);
			if (time_schedule[0][1] < 10)
			{
				ST7789_PrintString((char*)"0", WHITE1, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[0][1], itoa_temp, 10), WHITE1, 3);
		}
		else if (mode == setup_minute_sunrise && update_minute_sunrise)
		{
			update_minute_sunrise = false;
			
			ST7789_SetXYpos(54, 136);
			if (time_schedule[0][1] < 10)
			{
				ST7789_PrintString((char*)"0", RED, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[0][1], itoa_temp, 10), RED, 3);
			
			if (update_schedule)
			{
				update_schedule = false;
				
				ST7789_SetXYpos(0, 136);
				if (time_schedule[0][0] < 10)
				{
					ST7789_PrintString((char*)"0", WHITE1, 3);
				}
				ST7789_PrintString((char*)itoa(time_schedule[0][0], itoa_temp, 10), WHITE1, 3);
			}
		}

		//==============================================================================
		//						Режим настройки дня (6)
		//==============================================================================
		
		if (mode != setup_hour_day && update_hour_day)
		{
			update_hour_day = false;
			
			ST7789_SetXYpos(0, 162);
			if (time_schedule[1][0] < 10)
			{
				ST7789_PrintString((char*)"0", WHITE1, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[1][0], itoa_temp, 10), WHITE1, 3);
		}
		else if (mode == setup_hour_day && update_hour_day)
		{
			update_hour_day = false;
			
			ST7789_SetXYpos(0, 162);
			if (time_schedule[1][0] < 10)
			{
				ST7789_PrintString((char*)"0", RED, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[1][0], itoa_temp, 10), RED, 3);
			
			if (update_schedule)
			{
				update_schedule = false;
				
				ST7789_SetXYpos(54, 136);
				if (time_schedule[0][1] < 10)
				{
					ST7789_PrintString((char*)"0", WHITE1, 3);
				}
				ST7789_PrintString((char*)itoa(time_schedule[0][1], itoa_temp, 10), WHITE1, 3);
			}
		}
		
		//==============================================================================
		//						Режим настройки дня (7)
		//==============================================================================
		
		if (mode != setup_minute_day && update_minute_day)
		{
			update_minute_day = false;
			
			ST7789_SetXYpos(54, 162);
			if (time_schedule[1][1] < 10)
			{
				ST7789_PrintString((char*)"0", WHITE1, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[1][1], itoa_temp, 10), WHITE1, 3);
		}
		else if (mode == setup_minute_day && update_minute_day)
		{
			update_minute_day = false;
			
			ST7789_SetXYpos(54, 162);
			if (time_schedule[1][1] < 10)
			{
				ST7789_PrintString((char*)"0", RED, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[1][1], itoa_temp, 10), RED, 3);
			
			if (update_schedule)
			{
				update_schedule = false;
				
				ST7789_SetXYpos(0, 162);
				if (time_schedule[1][0] < 10)
				{
					ST7789_PrintString((char*)"0", WHITE1, 3);
				}
				ST7789_PrintString((char*)itoa(time_schedule[1][0], itoa_temp, 10), WHITE1, 3);
			}
		}
		
		//==============================================================================
		//						Режим настройки закта (8)
		//==============================================================================
		
		if (mode != setup_hour_sunset && update_hour_sunset)
		{
			update_hour_sunset = false;
			
			ST7789_SetXYpos(0, 188);
			if (time_schedule[2][0] < 10)
			{
				ST7789_PrintString((char*)"0", WHITE1, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[2][0], itoa_temp, 10), WHITE1, 3);
		}
		else if (mode == setup_hour_sunset && update_hour_sunset)
		{
			update_hour_sunset = false;
			
			ST7789_SetXYpos(0, 188);
			if (time_schedule[2][0] < 10)
			{
				ST7789_PrintString((char*)"0", RED, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[2][0], itoa_temp, 10), RED, 3);
			
			if (update_schedule)
			{
				update_schedule = false;
				
				ST7789_SetXYpos(54, 162);
				if (time_schedule[1][1] < 10)
				{
					ST7789_PrintString((char*)"0", WHITE1, 3);
				}
				ST7789_PrintString((char*)itoa(time_schedule[1][1], itoa_temp, 10), WHITE1, 3);
			}
		}
		
		//==============================================================================
		//						Режим настройки закта (9)
		//==============================================================================
		
		if (mode != setup_minute_sunset && update_minute_sunset)
		{
			update_minute_sunset = false;
			
			ST7789_SetXYpos(54, 188);
			if (time_schedule[2][1] < 10)
			{
				ST7789_PrintString((char*)"0", WHITE1, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[2][1], itoa_temp, 10), WHITE1, 3);
		}
		else if (mode == setup_minute_sunset && update_minute_sunset)
		{
			update_minute_sunset = false;
			
			ST7789_SetXYpos(54, 188);
			if (time_schedule[2][1] < 10)
			{
				ST7789_PrintString((char*)"0", RED, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[2][1], itoa_temp, 10), RED, 3);
			
			if (update_schedule)
			{
				update_schedule = false;
				
				ST7789_SetXYpos(0, 188);
				if (time_schedule[2][0] < 10)
				{
					ST7789_PrintString((char*)"0", WHITE1, 3);
				}
				ST7789_PrintString((char*)itoa(time_schedule[2][0], itoa_temp, 10), WHITE1, 3);
			}
		}
		
		//==============================================================================
		//						Режим настройки ночи (10)
		//==============================================================================
		
		if (mode != setup_hour_night && update_hour_night)
		{
			update_hour_night = false;
			
			ST7789_SetXYpos(0, 214);
			if (time_schedule[3][0] < 10)
			{
				ST7789_PrintString((char*)"0", WHITE1, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[3][0], itoa_temp, 10), WHITE1, 3);
		}
		else if (mode == setup_hour_night && update_hour_night)
		{
			update_hour_night = false;
			
			ST7789_SetXYpos(0, 214);
			if (time_schedule[3][0] < 10)
			{
				ST7789_PrintString((char*)"0", RED, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[3][0], itoa_temp, 10), RED, 3);
			
			if (update_schedule)
			{
				update_schedule = false;
				
				ST7789_SetXYpos(54, 188);
				if (time_schedule[2][1] < 10)
				{
					ST7789_PrintString((char*)"0", WHITE1, 3);
				}
				ST7789_PrintString((char*)itoa(time_schedule[2][1], itoa_temp, 10), WHITE1, 3);
			}
		}
		
		//==============================================================================
		//						Режим настройки закта (9)
		//==============================================================================
		
		if (mode != setup_minute_night && update_minute_night)
		{
			update_minute_night = false;
			
			ST7789_SetXYpos(54, 214);
			if (time_schedule[3][1] < 10)
			{
				ST7789_PrintString((char*)"0", WHITE1, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[3][1], itoa_temp, 10), WHITE1, 3);
		}
		else if (mode == setup_minute_night && update_minute_night)
		{
			update_minute_night = false;
			
			ST7789_SetXYpos(54, 214);
			if (time_schedule[3][1] < 10)
			{
				ST7789_PrintString((char*)"0", RED, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[3][1], itoa_temp, 10), RED, 3);
			
			if (update_schedule)
			{
				update_schedule = false;
				
				ST7789_SetXYpos(0, 214);
				if (time_schedule[3][0] < 10)
				{
					ST7789_PrintString((char*)"0", WHITE1, 3);
				}
				ST7789_PrintString((char*)itoa(time_schedule[3][0], itoa_temp, 10), WHITE1, 3);
			}
		}
		
		ST7789_SetXYpos(180, 210);
		ST7789_PrintString((char*)itoa(mode, itoa_temp, 10), rand() % 30, rand() % 60, rand() % 30, 3);
		ST7789_PrintString((char*)" ", 3);
		
		ST7789_SetXYpos(210, 210);
		ST7789_PrintString((char*)itoa(update_sync_DS1307, itoa_temp, 10), rand() % 30, rand() % 60, rand() % 30, 3);
	}
}



void Buttons_Handler(void)
{
	//==============================================================================
	//						Обработка нажатия кнопки инкремента
	//==============================================================================
	if (button == increase)
	{
		// Закрытие заявки на обработку
		button = none;
		
		switch (mode)
		{
			// Инкремент секунд
			case setup_second:
				{
					time_temp[second]++;
					
					if (time_temp[second] >= 60)
					{
						time_temp[second] = 0;
					}
					
					update_second = true;					
				}
				break;
			
			// Инкремент минут
			case setup_minute:
				{
					time_temp[minute]++;
					
					if (time_temp[minute] >= 60)
					{
						time_temp[minute] = 0;
					}
					
					update_minute = true;
				}
				break;
			
			// Инкремент часов
			case setup_hour:
				{
					time_temp[hour]++;
					
					if (time_temp[hour] >= 24)
					{
						time_temp[hour] = 0;
					}
					
					update_hour = true;
				}
				break;
			//**********************************************************	
			// Инкремент часов рассвета
			case setup_hour_sunrise:
			{
				time_schedule[0][0]++;
				
				if (time_schedule[0][0] >= 24)
				{
					time_schedule[0][0] = 0;
				}
				
				update_hour_sunrise = true;
			}
			break;
			
			// Инкремент минут рассвета
			case setup_minute_sunrise:
			{
				time_schedule[0][1]++;
				
				if (time_schedule[0][1] >= 60)
				{
					time_schedule[0][1] = 0;
				}
				
				update_minute_sunrise = true;
			}
			break;	
			
			// Инкремент часов дня
			case setup_hour_day:
			{
				time_schedule[1][0]++;
				
				if (time_schedule[1][0] >= 24)
				{
					time_schedule[1][0] = 0;
				}
				
				update_hour_day = true;
			}
			break;
			
			// Инкремент минут дня
			case setup_minute_day:
			{
				time_schedule[1][1]++;
				
				if (time_schedule[1][1] >= 60)
				{
					time_schedule[1][1] = 0;
				}
				
				update_minute_day = true;
			}
			break;
			
			// Инкремент часов заката
			case setup_hour_sunset:
			{
				time_schedule[2][0]++;
				
				if (time_schedule[2][0] >= 24)
				{
					time_schedule[2][0] = 0;
				}
				
				update_hour_sunset = true;
			}
			break;
			
			// Инкремент минут заката
			case setup_minute_sunset:
			{
				time_schedule[2][1]++;
				
				if (time_schedule[2][1] >= 60)
				{
					time_schedule[2][1] = 0;
				}
				
				update_minute_sunset = true;
			}
			break;
			
			// Инкремент часов ночи
			case setup_hour_night:
			{
				time_schedule[3][0]++;
				
				if (time_schedule[3][0] >= 24)
				{
					time_schedule[3][0] = 0;
				}
				
				update_hour_night = true;
			}
			break;
			
			// Инкремент минут ночи
			case setup_minute_night:
			{
				time_schedule[3][1]++;
				
				if (time_schedule[3][1] >= 60)
				{
					time_schedule[3][1] = 0;
				}
				
				update_minute_night = true;
			}
			break;
				
			default:
			{
				mode = normal_work;
				return;
			}
				break;
		}
	}
	
	//==============================================================================
	//						Обработка нажатия кнопки декремента
	//==============================================================================
	if (button == decrease)
	{
		// Закрытие заявки на обработку
		button = none;
		
		switch (mode)
		{
			// Инкремент секунд
			case setup_second:
				{
					time_temp[second]--;
					
					if (time_temp[second] >= 60)
					{
						time_temp[second] = 59;
					}
					
					update_second = true;
				}
				break;
			
			// Инкремент минут
			case setup_minute:
				{
					time_temp[minute]--;
				
					if (time_temp[minute] >= 60)
					{
						time_temp[minute] = 59;
					}
					
					update_minute = true;
				
				}
				break;
			
			// Инкремент часов
			case setup_hour:
				{
					time_temp[hour]--;
					
					if (time_temp[hour] >= 24)
					{
						time_temp[hour] = 23;
					}
					
					update_hour = true;
				}
				break;
				
			default:
				{
					mode = normal_work;
					return;
				}
				break;
		}
	}
	
	
	//==============================================================================
	//						Обработка нажатия кнопки часов
	//==============================================================================
	if (button == clock)
	{
		// Проверка
		if (mode == setup_hour_sunrise	|| mode == setup_minute_sunrise	||
			mode == setup_hour_day		|| mode == setup_minute_day		||
			mode == setup_hour_sunset	|| mode == setup_minute_sunset	||
			mode == setup_hour_night	|| mode == setup_minute_night	) return;
		
		// Закрытие заявки на обработку
		button = none;
		
		switch (mode)
		{
			// Переключение на настройку секунд
			case normal_work:
				{
					mode = setup_second;
					
					update_second = true;
					update_clock = true;
			
					for (int i = 0; i < 3; i++)
					{
						time_temp[i] = time[i];
					}
				}
				break;
			
			// Переключение на настройку минут
			case setup_second:
				{
					mode = setup_minute;
					
					update_minute = true;
					update_clock = true;
				}
				break;
			
			// Переключение на настройку часов
			case setup_minute:
				{
					mode = setup_hour;
					update_hour = true;
					update_clock = true;
				}
				break;
			
			// Переключение на обычный режим
			case setup_hour:
				{
					mode = normal_work;
					
					update_second = true;
					update_minute = true;
					update_hour = true;
					update_clock = true;
					
					for (int i = 0; i < 3; i++)
					{
						time[i] = time_temp[i];
					}
					
					DS1307_WriteTime(time);
				}
				break;
				
			default:
				{
					mode = normal_work;
					return;
				}
				break;
		}
	}	
	
	//==============================================================================
	//						Обработка нажатия кнопки расписания
	//==============================================================================
	if (button == schedule)
	{	
		// Проверка
		if (mode == setup_second || mode == setup_minute || mode == setup_hour) return;

		// Закрытие заявки на обработку
		button = none;
		
		switch (mode)
		{
			// Переключение на настройку часов-рассвета
			case normal_work:
				{
					mode = setup_hour_sunrise;
					
					update_schedule = true;
					update_hour_sunrise = true;
				}
				break;
				
			// Переключение на настройку минут-рассвета
			case setup_hour_sunrise:
				{
					mode = setup_minute_sunrise;
					
					update_schedule = true;
					update_minute_sunrise = true;
				}
				break;	
				
			// Переключение на настройку часов-дня
			case setup_minute_sunrise:
				{
					mode = setup_hour_day;
					
					update_schedule = true;
					update_hour_day = true;
				}
				break;	
				
			// Переключение на настройку минут-дня
			case setup_hour_day:
				{
					mode = setup_minute_day;
					
					update_schedule = true;
					update_minute_day = true;
				}
				break;	
				
			// Переключение на настройку часов-заката
			case setup_minute_day:
				{
					mode = setup_hour_sunset;
					
					update_schedule = true;
					update_hour_sunset = true;
				}
				break;
			
			// Переключение на настройку минут-заката
			case setup_hour_sunset:
				{
					mode = setup_minute_sunset;
					
					update_schedule = true;
					update_minute_sunset = true;
				}
				break;	
				
			// Переключение на настройку часов-ночи
				case setup_minute_sunset:
				{
					mode = setup_hour_night;
					
					update_schedule = true;
					update_hour_night = true;
				}
				break;
			
			// Переключение на настройку минут-заката
			case setup_hour_night:
				{
					mode = setup_minute_night;
					
					update_schedule = true;
					update_minute_night = true;
				}
				break;
				
			// Переключение на обычный режим										// TODO - добавить функцию записи в EEPROM!
			case setup_minute_night:
				{
					mode = normal_work;
					
					update_schedule			= true;
					update_hour_sunrise		= true;
					update_hour_day			= true;
					update_hour_sunset		= true;
					update_hour_night		= true;
					update_minute_sunrise	= true;
					update_minute_day		= true;
					update_minute_sunset	= true;
					update_minute_night		= true;
					
					//функция_записи_в_EEPROM!(time_schedule);
					
				}
				break;
				
			default:
				{
					mode = normal_work;
					return;
				}
				break;
		}
		
	}
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



ISR(INT0_vect)
{
	cli();
	_delay_ms(50);
	
	char bounce_counter = 0;

	while (!(PINA == 254 || PINA == 253 || PINA == 251 || PINA == 247))		// Опрос порта A до тех пор пока не будет определена кнопка (типа против дребезга и некорректных значений в порте)
	{
		bounce_counter++;
		_delay_ms(5);
		
		if (bounce_counter > 10)											// Если после 20 опросов нет значения кнопки то выход из прерывания
		{
			return;
		}
	}

	switch (PINA)															// Запоминание нажатой кнопки
	{
		case 254:
		button = increase;
			break;
		case 253:
		button = decrease;
			break;
		case 251:
		button = clock;
			break;
		case 247:
		button = schedule;
			break;
		default:
		return;
			break;
	}
	
	sei();
}


ISR(TIMER1_COMPA_vect)
{
	cli();
	
	time[second]++;
	if (!(mode == setup_second || mode == setup_minute || mode == setup_hour)) update_second = true;

	if (time[second] >= 60)
	{
		time[second] = 0;
		time[minute]++;
		if (!(mode == setup_second || mode == setup_minute || mode == setup_hour)) update_minute = true;
		
		if (time[minute] >= 60)
		{
			time[minute] = 0;
			time[hour]++;
			if (!(mode == setup_second || mode == setup_minute || mode == setup_hour)) update_hour = true;

			if (time[hour] >= 24)
			{
				time[second] = 0;
				time[minute] = 0;
				time[hour] = 0;
			}
		}
	}
	
	// Синхронизация
	if (time[second] == 0 && time[minute] == 0 && update_sync_DS1307)
	{
		update_sync_DS1307 = false;
		DS1307_ReadTime(time);
	}
	
	if (time[second] == 0 && time[minute] == 1)
	{
		update_sync_DS1307 = true;
	}
	
	sei();
}
