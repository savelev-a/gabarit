/*
 * main.c
 *
 *  Created on: 9 февр. 2017 г.
 *      Author: sysadmin
 */

#include <avr/io.h>
#include <util/delay.h>

#define GB_PORT PB0
#define BUTTON_PORT PB1
#define ENGINE_PORT PB2
#define LED_PORT PB5

#define SET_GB_ON  PORTB |= (1<<GB_PORT)
#define SET_GB_OFF PORTB &= ~(1<<GB_PORT);

#define IS_BUTTON_PRESSED !(PINB & (1<<BUTTON_PORT))
#define IS_ENGINE_ON !(PINB & (1<<ENGINE_PORT))

int main()
{
	uint8_t gb_on = 0;					//регистр включения габариток
	uint8_t old_button_state = 0;		//прошлое состояние кнопки
	uint8_t old_engine_state = 0;		//прошлое состояние двигателя
	uint8_t counter = 0;				//счетчик длительности нажатия
	uint8_t gb_lock = 0;				//регистр запрета выключения

	DDRB |= (1<<GB_PORT);
	DDRB &= ~(1<<BUTTON_PORT);
	DDRB &= ~(1<<ENGINE_PORT);

	PORTB |= (1<<BUTTON_PORT);
	PORTB |= (1<<ENGINE_PORT);

	_delay_ms(50);

	while(1)
	{
		if(IS_ENGINE_ON) //нормальный режим, двигатель включен
		{
			if(old_engine_state == 0)
			{
				gb_lock = 0;
				counter = 0;
			}

			if(IS_BUTTON_PRESSED) 	//кнопка нажата
			{
				if(old_button_state == 0)
					gb_on = 1 - gb_on;

				counter++;
				if(counter > 20) gb_lock = 1;

				old_button_state = 1;

			}
			else				//кнопка отжата
			{
				old_button_state = 0;
				counter = 0;
			}

			old_engine_state = 1;

			if(gb_on)
				SET_GB_ON;
			else
				SET_GB_OFF;

			if(gb_lock)
				PORTB |= (1<<LED_PORT);
			else
				PORTB &= ~(1<<LED_PORT);

			_delay_ms(50);
		}
		else			//двигатель выключен
		{
			if(!gb_lock)  //если выключение не заблокированно
			{
				if(old_engine_state == 1)
					_delay_ms(5000);

				SET_GB_OFF;
				gb_on = 0;
			}

			old_engine_state = 0;

		}
	}

	return 0;
}
