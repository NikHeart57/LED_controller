#define F_CPU 14745600UL

#include <stdlib.h>						// включает функции - itoa() atoi()
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ST7789.h"

void SetupTimers(void);
void SetupInterrupts(void);

char xpos = 0;
char ypos = 0;

int main(void)
{
	_delay_ms(200);

	ST7789_InitSPI();
	ST7789_InitST7789();
	
	ST7789_FillScreen(BLACK);
	ST7789_PrintString((char*)"BEBRA!", &xpos, &ypos, YELLOW, WHITE, 3);
	ST7789_PrintString((char*)"AmoGus", &xpos, &ypos, GREEN, RED, 3);
	

	while(1)
	{	
	}
}



void SetupTimers(void)
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
	
	cli();
}


void SetupInerrupts(void)
{
	sei();
	
	GICR |= (0 << INT1)|(1 << INT0)|(0 << INT2);						// General Interrupt Control Register - Установка битов INT1, INT0 или INT2 разрешает прерывания при возникновении события на соответствующем выводе микроконтроллера AVR, а сброс — запрещает.
	MCUCR |= (1 << ISC11)|(0 << ISC10)|(1 << ISC01)|(0 << ISC00);		// 10	- Перывание по спадающему фронту INT0, INT1
	MCUCSR |= (0 << ISC2);												// 0	- Перывание по спадающему фронту INT2
	
	cli();
}

