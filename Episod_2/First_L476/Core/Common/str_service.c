/**
 * Модуль работы с дополнительными портами ввода-вывода
 *
 * Порт светодиода - простое использование On-Off
 * Функции включения, выключения, переключения
 * Другие мигалки - применялись для отладки
 *
 *
 *
 * Author: ln.starmark@gmail.com
 * Date:  15.02.2020
 */

#include "Leds.h"
#include "str_service.h"
#include "str_systick.h"

volatile uint32_t tck = 0;
volatile uint32_t now = 0;
volatile uint32_t period = 500;
volatile uint32_t per = 500;

void LedToggleUart(void)
{
	HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
}

void LedBlink(bool* flag)
{
	if(*flag)
	{
		LedToggle(1);
		*flag = false;
	}

}

void Init_LedBlinkTime(uint32_t per)
{
	period = per;
	tck = HAL_GetTick();

}

void LedBlinkTime(void)
{
	now = HAL_GetTick();
	if((now - tck) > period)
	{
		tck = HAL_GetTick();
		LedToggle(1);
	}

}

int16_t Generator_Saw(int16_t ampl, int16_t phase, int16_t step)
{
	static uint8_t dir = 0;
	static int16_t res = 0;
	static int16_t old = 0;

	static int16_t t = 0;


	switch(dir)
	{
	case 0:
		t = phase;
		dir = 1;
		break;

	case 1:

		t += step;
		if(t >= ampl)
		{
			res = old;
			dir = 2;
		}else{
			res = ampl*t;
			old = res;
		}
		break;

	case 2:
		t -= step;
		if(t <= -ampl)
		{
			res = old;
			dir = 1;
		}else{
			res = ampl*t;
			old = res;
		}
		break;
	}

	return res;
}


