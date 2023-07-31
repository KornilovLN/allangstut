#ifndef __STR_SYSTICK_H
#define __STR_SYSTICK_H

/**
 * Модуль используемый для измерения интервалов времени (пополняемый)
 *
 * Author: ln.starmark@gmail.com
 * Date:  15.02.2020
 */

#define MAX_CNT_SYSTICK 		10
#define MAX_CNT_SYSTICK_DIVIDER 100

/// Использование только в целях отладки
/// Возможно применение в других версиях ПО

/// Используются в main.c и в str_uartdemo.c
extern int32_t test_cnt;

/// Используются в main.c и в str_service.c
extern bool flag_cnt_sys_tick;

/// Используется в stm32l4xx_it.c
void HAL_SYSTICK_Callback(void);

#endif // __STR_SYSTICK_H
