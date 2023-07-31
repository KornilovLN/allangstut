/**
 * Модуль используемый для измерения интервалов времени (пополняемый)
 *
 * Author: ln.starmark@gmail.com
 * Date:  15.02.2020
 */

#include "str_helper.h"
#include "str_systick.h"

void HAL_SYSTICK_Callback(void)
{
	static int32_t cnt_systick=0;
	static int32_t cnt_systick_divider=MAX_CNT_SYSTICK_DIVIDER;

	// тикаем и приближаемся к секунде
	cnt_systick_divider--;
	if(cnt_systick_divider == 0)
	{
		cnt_systick++;
		cnt_systick_divider = MAX_CNT_SYSTICK_DIVIDER;
	}

	// тут получаем примерно секунду
	if(cnt_systick > MAX_CNT_SYSTICK)
	{
		cnt_systick = 0;
		flag_cnt_sys_tick = true;
	}

}


