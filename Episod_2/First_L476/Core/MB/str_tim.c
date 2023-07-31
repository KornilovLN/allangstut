/**
  ******************************************************************************
  * @file           : str_tim.h
  * @brief          : Модуль таймера RTU (не используется в MODBUS ASCII)
  ******************************************************************************
  * @attention
  *
  * Модуль таймера RTU (не используется в MODBUS ASCII)
  *
  * Применяется для измерения интервалов тишины на линии Modbus RTU
  * Интервал  короткой задержки (полтора символа)
  * и интервал прекращения передачи цепочки (три с половиной символа)
  *
  * Author: ln.starmark@gmail.com
  * Date:  15.02.2020
 */

#include "str_tim.h"
#include "str_mbregisters.h"

// timeout MB  BAUD:  9600 	14400 	19200 	28800 	38400 	57600 	76800 	115200
int16_t zazor35[8] = {70, 	47, 	35, 	23, 	23, 	23, 	23, 	23};
int16_t zazor15[8] = {30, 	20, 	15, 	10, 	10, 	10, 	10, 	10};

int16_t zazcnt35 = 19;
int16_t zazcnt15 = 8;

bool en35 = false;
bool en15 = false;

bool flag_zazor35 = false;
bool flag_zazor15 = false;

// This callback is automatically called by the HAL on the UEV event
// тикаем и приближаемся к концу зазора 35 или 15
#ifndef STR_TESTONLY
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		if(en35)
		{
			zazcnt35--;					// считаем тики 35
			if(zazcnt35 == 0)
			{
				flag_zazor35 = true;	// Сигнал превышения зазора 35
				en35 = false;			// остановить счет тиков 35
			}
		}

		if(en15)
		{
			zazcnt15--;					// считаем тики 15
			if(zazcnt15 == 0)
			{
				flag_zazor15 = true;  	// Сигнал превышения зазора 15
				en15 = false;			// остановить счет тиков 15
			}
		}
	}
}
#endif

void Start_Tim_Base_IT(void)
{
	HAL_TIM_Base_Start_IT(&htim2);
}

// Установить счетчик колич тиков для 35 зазора
// сбросить сигнал выхода за пределы зазора
// установить разрешение счета тиков
void Start35(int8_t baud)
{
	zazcnt35 = zazor35[baud];
	flag_zazor35 = false;
	en35 = true;
}

void Stop35(void)
{
	flag_zazor35 = false;
	en35 = false;
}

// Установить счетчик колич тиков для 15 зазора
// сбросить сигнал выхода за пределы зазора
// установить разрешение счета тиков
void Start15(int8_t baud)
{
	zazcnt15 = zazor15[baud];
	flag_zazor15 = false;
	en15 = true;
}

void Stop15(void)
{
	flag_zazor15 = false;
	en15 = false;
}

bool isFlag15(void)
{
		return flag_zazor15;
}

bool isFlag35(void)
{
		return flag_zazor35;
}

//--------------------------------------------------------------
#ifdef STR_TESTONLY
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{// This callback is automatically called by the HAL on the UEV event

	static int32_t cnt_timtick=0;
	static int32_t cnt_tim_divider=MAX_CNT_TIM_DIVIDER;

	if(htim->Instance == TIM2)
	{
		// тикаем и приближаемся к секунде
		cnt_tim_divider--;
		if(cnt_tim_divider == 0)
		{
			cnt_timtick++;
			cnt_tim_divider = MAX_CNT_TIM_DIVIDER;
		}

		// тут получаем примерно секунду
		if(cnt_timtick > MAX_CNT_TIM)
		{
			cnt_timtick = 0;
			flag_cnt_tim_tick = true;
		}
	}
}
#endif
