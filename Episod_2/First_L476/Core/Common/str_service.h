#ifndef __STR_SERVICE_H
#define __STR_SERVICE_H

#include "str_helper.h"

/**
 * Модуль работы с дополнительными портами ввода-вывода
 *
 * Порт светодиода - простое использование On-Off
 * Функции включения, выключения, переключения
 * Другие мигалки - применялись для отладки
 *
 * Функция чтения портов установки адреса устройства на линии RS-485
 * использовалась в первой версии ПО
 * ID0 - порт нулевого бита адреса ID
 * ID1 - порт 1-го  бита адреса ID
 *
 *
 * Author: LN Starmark
 * Date:  15.02.2020
 */

/// Используются в main.c
extern bool flag_cnt_sys_tick;

///--- функции работы со светодиодом и битами установки ID
void LedBlink(bool* flag);

void Init_LedBlinkTime(uint32_t per);
void LedBlinkTime(void);

void LedToggleUart(void);

int16_t Generator_Saw(int16_t ampl, int16_t phase, int16_t step);

#endif // __STR_SERVICE_H
