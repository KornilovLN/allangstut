#ifndef __STR_TIM_H
#define __STR_TIM_H

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

#include "str_helper.h"
#include "str_onoffmodule.h"

#define MAX_CNT_TIM   			80				//	TEST
#define MAX_CNT_TIM_DIVIDER 	200				//	TEST

#ifdef STR_TESTONLY
// Используются в main.c и в str_uartdemo.c		//	TEST
extern int32_t test_cnt;
extern bool flag_cnt_tim_tick;
#endif

extern TIM_HandleTypeDef htim2;

/// Используется в stm32l4xx_it.c main.c
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void Start_Tim_Base_IT(void);

/// Установить счетчик колич тиков для 35 зазора
/// сбросить сигнал выхода за пределы зазора
/// установить разрешение счета тиков
void Start35(int8_t baud);
void Stop35(void);

/// Установить счетчик колич тиков для 15 зазора
/// сбросить сигнал выхода за пределы зазора
/// установить разрешение счета тиков
void Start15(int8_t baud);
void Stop15(void);

/// Проверка флага
bool isFlag15(void);
bool isFlag35(void);

///-----------------------------------------------------------
extern DMA_HandleTypeDef hdma_adc1;
//extern ADC_HandleTypeDef hadc1;
//extern DAC_HandleTypeDef hdac1;

//extern OPAMP_HandleTypeDef hopamp1;
//extern OPAMP_HandleTypeDef hopamp2;

extern int16_t ADC_value[8];
extern float PT1000[];

extern int16_t mode;
extern int16_t new_data;

extern int16_t timer6_int_cnt;
extern int16_t timer6_int_cnt_2;
extern int16_t b_cnt;

extern uint32_t time1;
extern uint32_t time2;

extern int16_t ku;
extern int16_t ki;
extern int16_t offset;
extern int16_t t__cels;
extern int16_t t__faring;
extern int16_t t__chip;
extern uint16_t s__simens;
extern int16_t p__bar;

extern int vtmp0;
extern int vtmp1;
extern int vtmp2;
extern int vtmp3;
extern int vtmp4;

extern float f_offset;
extern float u0i_tmp;
extern float u1u_tmp;
extern float u2_tmp;
extern float u3_tmp;
extern float u4_tmp;

extern float u0;
extern float u1;
extern float u2;
extern float u3;
extern float u4;

extern float current;
extern float voltage;

extern float resist;
extern float conduct;

extern float t_cels;
extern float t_faring;
extern float t_chip;

extern float tV_25;            //Вольт
extern float tSlope;

#endif // __STR_TIM14_H
