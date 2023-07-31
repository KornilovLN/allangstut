#ifndef __MATHCOMMON_H
#define __MATHCOMMON_H

/**
  ******************************************************************************
  * @file           : str_mathcommon.h
  * @brief          : Модуль часто используемых математических функций (пополняемый)
  ******************************************************************************
  * @attention
  *
  * Функции: CalcCRC - расчет 16-битной CRC
  *          CalcLRC - расчет LRC
  *          Некоторые часто используемые функции, назначение ясно из названия
  *
  * Author: ln.starmark@gmail.com
  * Date:   03.04.2020
  */

#include "str_helper.h"

#define HEXFORMAT	1
#define DECFORMAT	0

#define DECODEFORMAT	1
#define NODECODEFORMAT	0

#define FORVARDNULL		1
#define NOFORVARDNULL	0

uint16_t CalcCRC(uint8_t *buff, uint8_t number_byte);
uint8_t CalcLRC(uint8_t* dt, int16_t len);

uint16_t GetFloatHi(float value);
uint16_t GetFloatLo(float value);

float RoundMy(float val, int digit);

void Val32ToARRdigit(uint32_t val, uint8_t* arr);
void HEX32ToARRdigit(uint32_t val, uint8_t* arr);

void Waite(uint16_t cntdelay);

uint64_t Uint64Revers(uint64_t val);
uint64_t Uint64ReversByBytes(uint64_t val);
#endif
