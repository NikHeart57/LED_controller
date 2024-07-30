#define F_CPU 14745600UL

#include <stdlib.h>						// �������� ������� - itoa() atoi()
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ST7789.h"
#include "DS1307.h"

// ������ ��� itoa
char itoa_temp[9];

// �����
bool update_sync_DS1307		= true;

bool update_all				= true;

bool update_word			= true;

bool update_second			= true;
bool update_minute			= true;
bool update_hour			= true;
bool update_clock			= false;	// ��� ���������� ����� ��� ���������� � �������� ������ �� ����� ��� ������������ ����� ����������� ��� ���������

bool update_hour_sunrise	= true;
bool update_minute_sunrise	= true;
bool update_hour_day		= true;
bool update_minute_day		= true;
bool update_hour_sunset		= true;
bool update_minute_sunset	= true;
bool update_hour_night		= true;
bool update_minute_night	= true;
bool update_schedule		= false;	// ���������� ��� ����������
	

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



// ���������� �������
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

float time_Dec = (float)time[0] + (float)time[1] / 60.0 + (float)time[2] / 3600.0;
	
char time_schedule[4][2] =
{
	{10, 00},
	{12, 00},
	{18, 00},
	{20, 00}		
};

float time_schedule_Dec[4]=															
{
	(float)time_schedule[0][0] + (float)time_schedule[0][1] / 60.0,
	(float)time_schedule[1][0] + (float)time_schedule[1][1] / 60.0,
	(float)time_schedule[2][0] + (float)time_schedule[2][1] / 60.0,
	(float)time_schedule[3][0] + (float)time_schedule[3][1] / 60.0,
};	

float time_schedule_Dec_old[4]=
{

};
	

// ��������� �������
inline void reverse(char* str, int len);
inline int intToStr(int x, char str[], int d);
char* ftoa(float n, char* res, int afterpoint);

void Setup_TIM0(void);
void Setup_TIM1(void);
void Setup_INT(void);
void Setup_GPIO(void);

void Buttons_Handler(void);


int main(void)
{
	_delay_ms(200);
		
	Setup_INT();
	Setup_GPIO();
	Setup_TIM0();
	Setup_TIM1();
	
	DS1307_ReadTime(time);	
	
	ST7789_InitSPI();
	ST7789_InitST7789();
	ST7789_FillScreen(BLACK);
	ST7789_SetXYpos(0, 0);

	colour Text_Colour = WHITE;
	colour Text_Colour_RED = RED;

	while (1)
	{	
		
		Text_Colour.red = rand() % 30;
		Text_Colour.green = rand() % 60;
		Text_Colour.blue = rand() % 30;
		
		
		Buttons_Handler();
		
		
		/* 
				**�����**
		if(mode != setup_second && update_second)			// ������� �� ��� ����� ������ � ������� ������
		{
			update_second = false;
			** ��� ����� **
		}
		else if(mode == setup_second && update_second)		// ����� �� ��� ���� ������ � ��������� ������
		{
			update_second = false;
			** ��� ������� **		
		}
		*/
		
		//==============================================================================
		//								������ �������
		//==============================================================================

		if(mode != normal_work && update_second)
		{
			
		}
		else if(mode == normal_work && update_second)
		{			
			// ������� �����
			time_Dec = (float)time[hour] + (float)time[minute] / 60.0 + (float)time[second] / 3600.0;
			
			for(int i = 0; i < 4; i++)
			{
				time_schedule_Dec[i] = (float)time_schedule[i][0] + (float)time_schedule[i][1] / 60.0;
			}
			
			update_word = true;
		}

		//==============================================================================
		//								����� ���������
		//==============================================================================
			
		if(mode != normal_work && update_word)
		{
			update_word = false;
			
		}
		else if(mode == normal_work && update_word)
		{
			update_word = false;
				
			// ����������� ������� �����, ������������� ����� � ����������
			// ����� ������ ��� ����� �������� => ����
			
			ST7789_SetXYpos(0, 40);
			
			if (time_Dec < time_schedule_Dec[0])
			{
				ST7789_PrintString((char*)"����   ", Text_Colour, 5);
				OCR0 = 0;
			}
				
			// ����� ������ ��� ����� ������ ��� => �������
			else if (time_Dec < time_schedule_Dec[1])
			{
				ST7789_PrintString((char*)"�������", Text_Colour, 5);
				OCR0 = (char)(((time_Dec - time_schedule_Dec[0])/(time_schedule_Dec[1] - time_schedule_Dec[0])) * 255.0);
			}
				
			// ����� ������ ��� ����� ����� ��� => ����
			else if (time_Dec < time_schedule_Dec[2])
			{
				ST7789_PrintString((char*)"����   ", Text_Colour, 5);
				OCR0 = 255;
			}
				
			// ����� ������ ��� ����� ����� ������ => �����
			else if (time_Dec < time_schedule_Dec[3])
			{
				ST7789_PrintString((char*)"�����  ", Text_Colour, 5);
				OCR0 = (char)((1.0 - ((time_Dec - time_schedule_Dec[2])/(time_schedule_Dec[3] - time_schedule_Dec[2]))) * 255.0);
			}
				
			// � ��������� ������� ����
			else
			{
				ST7789_PrintString((char*)"����   ", Text_Colour, 5);
				OCR0 = 0;
			}	
			
			
			
			// ������ ����������

			ST7789_SetXYpos(0, 90);
			ST7789_PrintString((char*)"���.", Text_Colour, 3);
			
			if ((float)OCR0/2.55 < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour, 3);
			}
			ST7789_PrintString(ftoa(((float)OCR0/2.55), itoa_temp, 2), Text_Colour, 3);
			
			ST7789_PrintString((char*)"%  ", Text_Colour, 3);	
		}
			
		
		//==============================================================================
		//							����� �������� ������� (0)
		//==============================================================================

		
		
		
		if(mode != normal_work && update_all)
		{
			
			update_all = false;					
		}
		else if(mode == normal_work && update_all)
		{
			
			update_all = false;	
			
			ST7789_SetXYpos(60, 0);
			ST7789_PrintString((char*)":", Text_Colour, 5);
			ST7789_SetXYpos(150, 0);
			ST7789_PrintString((char*)":", Text_Colour, 5);
			
			
			for(int i = 0; i < 3; i++)
			{
				ST7789_SetXYpos(90 * i, 0);
				if (time[2 - i] < 10)
				{
					ST7789_PrintString((char*)"0", Text_Colour, 5);
				}
				ST7789_PrintString((char*)itoa(time[2 - i], itoa_temp, 10), Text_Colour, 5);
			}
		
			for(int i = 0; i < 4; i++)
			{
				ST7789_SetXYpos(36, 136 + 26 * i);
				ST7789_PrintString((char*)":", Text_Colour, 3);
			}		
			
			for(int i = 0; i < 4; i++)
			{
				for(int j = 0; j < 2; j++)
				{
					ST7789_SetXYpos(54 * j, 136 + 26 * i);
									
					if (time_schedule[i][j] < 10)
					{
						ST7789_PrintString((char*)"0", Text_Colour, 3);
					}
					ST7789_PrintString((char*)itoa(time_schedule[i][j], itoa_temp, 10), Text_Colour, 3);
				}
				
				switch (i)
				{
					case 0:
						ST7789_PrintString((char*)" �������", Text_Colour, 3);
						break;
						
					case 1:
						ST7789_PrintString((char*)" ����   ", Text_Colour, 3);
						break;
						
					case 2:
						ST7789_PrintString((char*)" �����  ", Text_Colour, 3);
						break;	
						
					case 3:
						ST7789_PrintString((char*)" ����   ", Text_Colour, 3);
						break;
						
					default:
						break;
				}
			}
		}
		
		

		//==============================================================================
		//							����� ��������� ������ (1)
		//==============================================================================
		
		/* 
				**�����**
		if(mode != setup_second && update_second)			// ������� �� ��� ����� ������ � ������� ������
		{
			update_second = false;
			** ��� ����� **
		}
		else if(mode == setup_second && update_second)		// ����� �� ��� ���� ������ � ��������� ������
		{
			update_second = false;
			** ��� ������� **		
		}
		*/
		
		if (mode != setup_second && update_second)
		{
			update_second = false;
			
			ST7789_SetXYpos(180, 0);
			if (time[second] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour, 5);
			}			
			ST7789_PrintString((char*)itoa(time[second], itoa_temp, 10), Text_Colour, 5);
		}
		else if (mode == setup_second && update_second)
		{
			update_second = false;
			
			ST7789_SetXYpos(180, 0);
			if (time_temp[second] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour_RED, 5);
			}
			ST7789_PrintString((char*)itoa(time_temp[second], itoa_temp, 10), Text_Colour_RED, 5);
			
			if (update_clock)
			{
				update_clock = false;
				/*	����� ��� �������� ������� ���������� ��������. ���� ��� ����������, ���� � ����� �� ������
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
		//							����� ��������� ����� (2)
		//==============================================================================
		
		if (mode != setup_minute && update_minute)
		{
			update_minute = false;
			
			ST7789_SetXYpos(90, 0);
			if (time[minute] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour, 5);
			}
			ST7789_PrintString((char*)itoa(time[minute], itoa_temp, 10), Text_Colour, 5);
			//ST7789_PrintString((char*)":", WHITE1, 5);		
		}
		else if (mode == setup_minute && update_minute)
		{
			update_minute = false;
			
			ST7789_SetXYpos(90, 0);
			if (time_temp[minute] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour_RED, 5);
			}
			ST7789_PrintString((char*)itoa(time_temp[minute], itoa_temp, 10), Text_Colour_RED, 5);
			
			if (update_clock)
			{
				update_clock = false;
				/*	����� ��� �������� ������� ���������� ��������. ���� ��� ����������, ���� � ����� �� ������
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
					ST7789_PrintString((char*)"0", Text_Colour, 5);
				}
				ST7789_PrintString((char*)itoa(time_temp[second], itoa_temp, 10), Text_Colour, 5);
			}
		}	
		
		
		//==============================================================================
		//							����� ��������� ����� (3)
		//==============================================================================
		
		if (mode != setup_hour && update_hour)
		{
			update_hour = false;
			
			ST7789_SetXYpos(0, 0);
			if (time[hour] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour, 5);
			}
			ST7789_PrintString((char*)itoa(time[hour], itoa_temp, 10), Text_Colour, 5);
			//ST7789_PrintString((char*)":", WHITE1, 5);	
		}
		else if (mode == setup_hour && update_hour)
		{

			update_hour = false;
			
			ST7789_SetXYpos(0, 0);
			if (time_temp[hour] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour_RED, 5);
			}
			ST7789_PrintString((char*)itoa(time_temp[hour], itoa_temp, 10), Text_Colour_RED, 5);
				
			if (update_clock)
			{
				update_clock = false;
					
				ST7789_SetXYpos(90, 0);
				if (time_temp[minute] < 10)
				{
					ST7789_PrintString((char*)"0", Text_Colour, 5);
				}
				ST7789_PrintString((char*)itoa(time_temp[minute], itoa_temp, 10), Text_Colour, 5);
				/*	����� ��� �������� ������� ���������� ��������. ���� ��� ����������, ���� � ����� �� ������
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
		//						����� ��������� ������� (4)
		//==============================================================================
		
		if (mode != setup_hour_sunrise && update_hour_sunrise)
		{
			update_hour_sunrise = false;
			
			ST7789_SetXYpos(0, 136);
			if (time_schedule[0][0] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[0][0], itoa_temp, 10), Text_Colour, 3);
		}
		else if (mode == setup_hour_sunrise && update_hour_sunrise)
		{
			update_hour_sunrise = false;
				
			ST7789_SetXYpos(0, 136);
			if (time_schedule[0][0] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour_RED, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[0][0], itoa_temp, 10), Text_Colour_RED, 3);
			
			/* ���������� ����� - �� �����	
			if (update_schedule)
			{
				update_schedule = false;
				
				ST7789_SetXYpos(0, 0);
				if (time[hour] < 10)
				{
					ST7789_PrintString((char*)"0", WHITE1, 5);
				}
				ST7789_PrintString((char*)itoa(time[hour], itoa_temp, 10), WHITE1, 5);
			}*/
		}

		
		//==============================================================================
		//						����� ��������� ������� (5)
		//==============================================================================
		
		if (mode != setup_minute_sunrise && update_minute_sunrise)
		{
			update_minute_sunrise = false;
			
			ST7789_SetXYpos(54, 136);
			if (time_schedule[0][1] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[0][1], itoa_temp, 10), Text_Colour, 3);
		}
		else if (mode == setup_minute_sunrise && update_minute_sunrise)
		{
			update_minute_sunrise = false;
			
			ST7789_SetXYpos(54, 136);
			if (time_schedule[0][1] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour_RED, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[0][1], itoa_temp, 10), Text_Colour_RED, 3);
			
			if (update_schedule)
			{
				update_schedule = false;
				
				ST7789_SetXYpos(0, 136);
				if (time_schedule[0][0] < 10)
				{
					ST7789_PrintString((char*)"0", Text_Colour, 3);
				}
				ST7789_PrintString((char*)itoa(time_schedule[0][0], itoa_temp, 10), Text_Colour, 3);
			}
		}


		//==============================================================================
		//						����� ��������� ��� (6)
		//==============================================================================
		
		if (mode != setup_hour_day && update_hour_day)
		{
			update_hour_day = false;
			
			ST7789_SetXYpos(0, 162);
			if (time_schedule[1][0] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[1][0], itoa_temp, 10), Text_Colour, 3);
		}
		else if (mode == setup_hour_day && update_hour_day)
		{
			update_hour_day = false;
			
			ST7789_SetXYpos(0, 162);
			if (time_schedule[1][0] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour_RED, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[1][0], itoa_temp, 10), Text_Colour_RED, 3);
			
			if (update_schedule)
			{
				update_schedule = false;
				
				ST7789_SetXYpos(54, 136);
				if (time_schedule[0][1] < 10)
				{
					ST7789_PrintString((char*)"0", Text_Colour, 3);
				}
				ST7789_PrintString((char*)itoa(time_schedule[0][1], itoa_temp, 10), Text_Colour, 3);
			}
		}
		
		
		//==============================================================================
		//						����� ��������� ��� (7)
		//==============================================================================
		
		if (mode != setup_minute_day && update_minute_day)
		{
			update_minute_day = false;
			
			ST7789_SetXYpos(54, 162);
			if (time_schedule[1][1] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[1][1], itoa_temp, 10), Text_Colour, 3);
		}
		else if (mode == setup_minute_day && update_minute_day)
		{
			update_minute_day = false;
			
			ST7789_SetXYpos(54, 162);
			if (time_schedule[1][1] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour_RED, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[1][1], itoa_temp, 10), Text_Colour_RED, 3);
			
			if (update_schedule)
			{
				update_schedule = false;
				
				ST7789_SetXYpos(0, 162);
				if (time_schedule[1][0] < 10)
				{
					ST7789_PrintString((char*)"0", Text_Colour, 3);
				}
				ST7789_PrintString((char*)itoa(time_schedule[1][0], itoa_temp, 10), Text_Colour, 3);
			}
		}
		
		
		//==============================================================================
		//						����� ��������� ����� (8)
		//==============================================================================
		
		if (mode != setup_hour_sunset && update_hour_sunset)
		{
			update_hour_sunset = false;
			
			ST7789_SetXYpos(0, 188);
			if (time_schedule[2][0] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[2][0], itoa_temp, 10), Text_Colour, 3);
		}
		else if (mode == setup_hour_sunset && update_hour_sunset)
		{
			update_hour_sunset = false;
			
			ST7789_SetXYpos(0, 188);
			if (time_schedule[2][0] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour_RED, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[2][0], itoa_temp, 10), Text_Colour_RED, 3);
			
			if (update_schedule)
			{
				update_schedule = false;
				
				ST7789_SetXYpos(54, 162);
				if (time_schedule[1][1] < 10)
				{
					ST7789_PrintString((char*)"0", Text_Colour, 3);
				}
				ST7789_PrintString((char*)itoa(time_schedule[1][1], itoa_temp, 10), Text_Colour, 3);
			}
		}
		
		
		//==============================================================================
		//						����� ��������� ����� (9)
		//==============================================================================
		
		if (mode != setup_minute_sunset && update_minute_sunset)
		{
			update_minute_sunset = false;
			
			ST7789_SetXYpos(54, 188);
			if (time_schedule[2][1] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[2][1], itoa_temp, 10), Text_Colour, 3);
		}
		else if (mode == setup_minute_sunset && update_minute_sunset)
		{
			update_minute_sunset = false;
			
			ST7789_SetXYpos(54, 188);
			if (time_schedule[2][1] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour_RED, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[2][1], itoa_temp, 10), Text_Colour_RED, 3);
			
			if (update_schedule)
			{
				update_schedule = false;
				
				ST7789_SetXYpos(0, 188);
				if (time_schedule[2][0] < 10)
				{
					ST7789_PrintString((char*)"0", Text_Colour, 3);
				}
				ST7789_PrintString((char*)itoa(time_schedule[2][0], itoa_temp, 10), Text_Colour, 3);
			}
		}
		
		
		//==============================================================================
		//						����� ��������� ���� (10)
		//==============================================================================
		
		if (mode != setup_hour_night && update_hour_night)
		{
			update_hour_night = false;
			
			ST7789_SetXYpos(0, 214);
			if (time_schedule[3][0] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[3][0], itoa_temp, 10), Text_Colour, 3);
		}
		else if (mode == setup_hour_night && update_hour_night)
		{
			update_hour_night = false;
			
			ST7789_SetXYpos(0, 214);
			if (time_schedule[3][0] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour_RED, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[3][0], itoa_temp, 10), Text_Colour_RED, 3);
			
			if (update_schedule)
			{
				update_schedule = false;
				
				ST7789_SetXYpos(54, 188);
				if (time_schedule[2][1] < 10)
				{
					ST7789_PrintString((char*)"0", Text_Colour, 3);
				}
				ST7789_PrintString((char*)itoa(time_schedule[2][1], itoa_temp, 10), Text_Colour, 3);
			}
		}
		
		
		//==============================================================================
		//						����� ��������� ����� (9)
		//==============================================================================
		
		if (mode != setup_minute_night && update_minute_night)
		{
			update_minute_night = false;
			
			ST7789_SetXYpos(54, 214);
			if (time_schedule[3][1] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[3][1], itoa_temp, 10), Text_Colour, 3);
		}
		else if (mode == setup_minute_night && update_minute_night)
		{
			update_minute_night = false;
			
			ST7789_SetXYpos(54, 214);
			if (time_schedule[3][1] < 10)
			{
				ST7789_PrintString((char*)"0", Text_Colour_RED, 3);
			}
			ST7789_PrintString((char*)itoa(time_schedule[3][1], itoa_temp, 10), Text_Colour_RED, 3);
			
			if (update_schedule)
			{
				update_schedule = false;
				
				ST7789_SetXYpos(0, 214);
				if (time_schedule[3][0] < 10)
				{
					ST7789_PrintString((char*)"0", Text_Colour, 3);
				}
				ST7789_PrintString((char*)itoa(time_schedule[3][0], itoa_temp, 10), Text_Colour, 3);
			}
		}
		
		/*
		ST7789_SetXYpos(190, 214);
		ST7789_PrintString((char*)itoa(mode, itoa_temp, 10), rand() % 30, rand() % 60, rand() % 30, 3);
		ST7789_PrintString((char*)" ", 3);
		
		ST7789_SetXYpos(214, 214);
		ST7789_PrintString((char*)itoa(update_sync_DS1307, itoa_temp, 10), rand() % 30, rand() % 60, rand() % 30, 3);
		*/
	}
}



void Buttons_Handler(void)
{
	//==============================================================================
	//						��������� ������� ������ ����������
	//==============================================================================
	if (button == increase)
	{
		// �������� ������ �� ���������
		button = none;
		
		switch (mode)
		{
			// ��������� ������
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
			
			// ��������� �����
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
			
			// ��������� �����
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
			// ��������� ����� ��������
			case setup_hour_sunrise:
			{
			
				if ((time_schedule[0][0] + 1) * 60 + time_schedule[0][1] <= time_schedule[1][0] * 60 + time_schedule[1][1])
				{
					time_schedule[0][0]++;
				}
				
				if (time_schedule[0][0] >= 24)
				{
					time_schedule[0][0] = 23;
				}
				
				update_hour_sunrise = true;
				}
				break;
			
			// ��������� ����� ��������
			case setup_minute_sunrise:
			{
				if (time_schedule[0][0] * 60 + time_schedule[0][1] + 2 <= time_schedule[1][0] * 60 + time_schedule[1][1])
				{
					time_schedule[0][1]++;
				}
				
				if (time_schedule[0][1] >= 60)
				{
					time_schedule[0][1] = 59;
				}
				
				update_minute_sunrise = true;
				}
				break;	
			
			// ��������� ����� ���
			case setup_hour_day:
			{
				if ((time_schedule[1][0] + 1) * 60 + time_schedule[1][1] <= time_schedule[2][0] * 60 + time_schedule[2][1])
				{
					time_schedule[1][0]++;
				}
				
				if (time_schedule[1][0] >= 24)
				{
					time_schedule[1][0] = 23;
				}
				
				update_hour_day = true;
				}
				break;
			
			// ��������� ����� ���
			case setup_minute_day:
			{
				if (time_schedule[1][0] * 60 + time_schedule[1][1] + 2 <= time_schedule[2][0] * 60 + time_schedule[2][1])
				{
					time_schedule[1][1]++;
				}
				
				if (time_schedule[1][1] >= 60)
				{
					time_schedule[1][1] = 59;
				}
				
				update_minute_day = true;
				}
				break;
			
			// ��������� ����� ������
			case setup_hour_sunset:
			{
				if ((time_schedule[2][0] + 1) * 60 + time_schedule[2][1] <= time_schedule[3][0] * 60 + time_schedule[3][1])
				{
					time_schedule[2][0]++;
				}
				
				if (time_schedule[2][0] >= 24)
				{
					time_schedule[2][0] = 23;
				}
				
				update_hour_sunset = true;
				}
				break;
			
			// ��������� ����� ������
			case setup_minute_sunset:
			{
				if (time_schedule[2][0] * 60 + time_schedule[2][1] + 2 <= time_schedule[3][0] * 60 + time_schedule[3][1])
				{
					time_schedule[2][1]++;
				}
				
				if (time_schedule[2][1] >= 60)
				{
					time_schedule[2][1] = 59;
				}
				
				update_minute_sunset = true;
				}
				break;
			
			// ��������� ����� ����
			case setup_hour_night:
			{
				// ��� �����������
				time_schedule[3][0]++;
				
				if (time_schedule[3][0] >= 24)
				{
					time_schedule[3][0] = 23;
				}
				
				update_hour_night = true;
				}
				break;
			
			// ��������� ����� ����
			case setup_minute_night:
			{
				// ��� �����������
				time_schedule[3][1]++;
				
				if (time_schedule[3][1] >= 60)
				{
					time_schedule[3][1] = 59;
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
	//						��������� ������� ������ ����������
	//==============================================================================
	if (button == decrease)
	{
		// �������� ������ �� ���������
		button = none;
		
		switch (mode)
		{
			// ��������� ������
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
			
			// ��������� �����
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
			
			// ��������� �����
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
			//**********************************************************	
			// ��������� ����� ��������
			case setup_hour_sunrise:
			{
				// ���������� ��� �����������
				time_schedule[0][0]--;
				
				if (time_schedule[0][0] >= 24)
				{
					time_schedule[0][0] = 0;
				}
				
				update_hour_sunrise = true;
				}
				break;
			
			// ��������� ����� ��������
			case setup_minute_sunrise:
			{
				// ���������� ��� �����������
				time_schedule[0][1]--;
				
				if (time_schedule[0][1] >= 60)
				{
					time_schedule[0][1] = 0;
				}
				
				update_minute_sunrise = true;
				}
				break;	
			
			// ��������� ����� ���
			case setup_hour_day:
			{
				if ((time_schedule[1][0] - 1) * 60 + time_schedule[1][1] > time_schedule[0][0] * 60 + time_schedule[0][1])
				{
					time_schedule[1][0]--;
				}
				
				if (time_schedule[1][0] >= 24)
				{
					time_schedule[1][0] = 0;
				}
				
				update_hour_day = true;
				}
				break;
			
			// ��������� ����� ���
			case setup_minute_day:
			{
				if ((time_schedule[1][0]) * 60 + time_schedule[1][1] - 1 > time_schedule[0][0] * 60 + time_schedule[0][1])
				{
					time_schedule[1][1]--;
				}
				
				if (time_schedule[1][1] >= 60)
				{
					time_schedule[1][1] = 0;
				}
				
				update_minute_day = true;
				}
				break;
			
			// ��������� ����� ������
			case setup_hour_sunset:
			{
				if ((time_schedule[2][0] - 1) * 60 + time_schedule[2][1] > time_schedule[1][0] * 60 + time_schedule[1][1])
				{
					time_schedule[2][0]--;
				}
				
				if (time_schedule[2][0] >= 24)
				{
					time_schedule[2][0] = 0;
				}
				
				update_hour_sunset = true;
				}
				break;
			
			// ��������� ����� ������
			case setup_minute_sunset:
			{
				if ((time_schedule[2][0]) * 60 + time_schedule[2][1] - 1 > time_schedule[1][0] * 60 + time_schedule[1][1])
				{
					time_schedule[2][1]--;
				}
				
				if (time_schedule[2][1] >= 60)
				{
					time_schedule[2][1] = 0;
				}
				
				update_minute_sunset = true;
				}
				break;
			
			// ��������� ����� ����
			case setup_hour_night:
			{
				if ((time_schedule[3][0] - 1) * 60 + time_schedule[3][1] > time_schedule[2][0] * 60 + time_schedule[2][1])
				{
					time_schedule[3][0]--;
				}
				
				if (time_schedule[3][0] >= 24)
				{
					time_schedule[3][0] = 0;
				}
				
				update_hour_night = true;
				}
				break;
			
			// ��������� ����� ����
			case setup_minute_night:
			{
				if ((time_schedule[3][0]) * 60 + time_schedule[3][1] - 1 > time_schedule[2][0] * 60 + time_schedule[2][1])
				{
					time_schedule[3][1]--;
				}
				
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
	//						��������� ������� ������ �����
	//==============================================================================
	if (button == clock)
	{
		// ��������
		if (mode == setup_hour_sunrise	|| mode == setup_minute_sunrise	||
			mode == setup_hour_day		|| mode == setup_minute_day		||
			mode == setup_hour_sunset	|| mode == setup_minute_sunset	||
			mode == setup_hour_night	|| mode == setup_minute_night	) return;
		
		// �������� ������ �� ���������
		button = none;
		
		switch (mode)
		{
			// ������������ �� ��������� ������
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
			
			// ������������ �� ��������� �����
			case setup_second:
				{
					mode = setup_minute;
					
					update_minute = true;
					update_clock = true;
				}
				break;
			
			// ������������ �� ��������� �����
			case setup_minute:
				{
					mode = setup_hour;
					update_hour = true;
					update_clock = true;
				}
				break;
			
			// ������������ �� ������� �����
			case setup_hour:
				{
					mode = normal_work;
					
					update_second = true;
					update_minute = true;
					update_hour = true;
					update_clock = true;
					
					update_word = true;
					
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
	//						��������� ������� ������ ����������
	//==============================================================================
	if (button == schedule)
	{	
		// ��������
		if (mode == setup_second || mode == setup_minute || mode == setup_hour) return;

		// �������� ������ �� ���������
		button = none;
		
		switch (mode)
		{
			// ������������ �� ��������� �����-��������
			case normal_work:
				{
					mode = setup_hour_sunrise;
					
					update_schedule = true;
					update_hour_sunrise = true;
				}
				break;
				
			// ������������ �� ��������� �����-��������
			case setup_hour_sunrise:
				{
					mode = setup_minute_sunrise;
					
					update_schedule = true;
					update_minute_sunrise = true;
				}
				break;	
				
			// ������������ �� ��������� �����-���
			case setup_minute_sunrise:
				{
					mode = setup_hour_day;
					
					update_schedule = true;
					update_hour_day = true;
				}
				break;	
				
			// ������������ �� ��������� �����-���
			case setup_hour_day:
				{
					mode = setup_minute_day;
					
					update_schedule = true;
					update_minute_day = true;
				}
				break;	
				
			// ������������ �� ��������� �����-������
			case setup_minute_day:
				{
					mode = setup_hour_sunset;
					
					update_schedule = true;
					update_hour_sunset = true;
				}
				break;
			
			// ������������ �� ��������� �����-������
			case setup_hour_sunset:
				{
					mode = setup_minute_sunset;
					
					update_schedule = true;
					update_minute_sunset = true;
				}
				break;	
				
			// ������������ �� ��������� �����-����
				case setup_minute_sunset:
				{
					mode = setup_hour_night;
					
					update_schedule = true;
					update_hour_night = true;
				}
				break;
			
			// ������������ �� ��������� �����-������
			case setup_hour_night:
				{
					mode = setup_minute_night;
					
					update_schedule = true;
					update_minute_night = true;
				}
				break;
				
			// ������������ �� ������� �����										// TODO - �������� ������� ������ � EEPROM!
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
					
					//�������_������_�_EEPROM!(time_schedule);
					
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
	//////////// ������ 0 (8 ���) PWM ////////////
	OCR0  |= 0;			// ���������� - �������� ��������� f = fcpu/(N*256) = 7372800/256 = 28800 || = 14745600/256 = 57600 | 248 -> 97.1% | 249 -> 97.5% | 250 -> 97.9% | 251 -> 98.3% | 252 -> 98.7%  | *�� ������� ���� 97,5 - 98,6%
	TCCR0 |= (0 << FOC0)|(1  << WGM01)|(1 << WGM00)|(1 << COM01)|(0 << COM00)|(0 << CS02)|(0 << CS01)|(1 << CS00);	// WGM - fast PWM, COM - clear on compare, CS - ���������, FOC - ?
	
	DDRB |= 0b00001000;		// ����� ��� - PB3(OC0)
	
	sei();
}


void Setup_TIM1(void)
{
	//////////// ������ 1 (16 ���) ���� ////////////
	OCR1A   = 14398;	// ������ �������� ���������� CTC ������������ �� ������������� �������
	TCCR1A |= (0 << COM1A1)|(0 << COM1A0)|(0 << COM1B1)|(0 << COM1B0)|(0 << FOC1A)|(0 << FOC1B)|(0 << WGM11)|(0 << WGM10);	// COM - �����, FOC - ?, WGM - ����� CTC, CS - ���������
	TCCR1B |= (0 <<  ICNC1)|(0 <<  ICES1)|(0 <<  WGM13)|(1 <<  WGM12)|(1 <<  CS12)|(0 <<  CS11)|(1 <<  CS10);				// IC - ��������� ���� ICP1 (PD6)
	TIMSK  |= (0 << TICIE1)|(1 << OCIE1A)|(0 << OCIE1B)|(0 << TOIE1);														// OCIE1A - ���������� �� ���������� ���������
	
	sei();
}

void Setup_INT(void)
{
	GICR |= (0 << INT1)|(1 << INT0)|(0 << INT2);						// General Interrupt Control Register - ��������� ����� INT1, INT0 ��� INT2 ��������� ���������� ��� ������������� ������� �� ��������������� ������ ���������������� AVR, � ����� � ���������.
	MCUCR |= (1 << ISC11)|(0 << ISC10)|(1 << ISC01)|(0 << ISC00);		// 10	- ��������� �� ���������� ������ INT0, INT1
	MCUCSR |= (0 << ISC2);												// 0	- ��������� �� ���������� ������ INT2
	
	sei();
}

void Setup_GPIO(void)
{
	PORTA = 0xff;						// ��� �������
	DDRA = 0x00;						// ��� ����
}



ISR(INT0_vect)
{
	cli();
	_delay_ms(50);
	
	char bounce_counter = 0;

	while (!(PINA == 254 || PINA == 253 || PINA == 251 || PINA == 247))		// ����� ����� A �� ��� ��� ���� �� ����� ���������� ������ (���� ������ �������� � ������������ �������� � �����)
	{
		bounce_counter++;
		_delay_ms(5);
		
		if (bounce_counter > 10)											// ���� ����� 20 ������� ��� �������� ������ �� ����� �� ����������
		{
			return;
		}
	}

	switch (PINA)															// ����������� ������� ������
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
	
	// �������������
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