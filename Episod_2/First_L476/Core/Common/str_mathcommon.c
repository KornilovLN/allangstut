/**
  ******************************************************************************
  * @file           : str_mathcommon.c
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

#include "str_mathcommon.h"

/// Расчет CRC 16 bits используется в алгоритме реализации протокола Modbus RTU
/// При чтении пакета данных CRC находится в конце пакета (Старший : Младший) байты
/// Приформировании пакета для отправки CRC также записывают в конец пакета (Старший : Младший) байты
uint16_t CalcCRC(uint8_t *buff, uint8_t number_byte)
{
	uint16_t crc;
    unsigned char bit_counter;

    crc = 0xFFFF;
    while ( number_byte>0 )
    {
    	crc ^= *buff;
		  buff++;
      bit_counter=0;

      while ( bit_counter < 8 )
      {
        	if ( crc & 0x0001 )
          {
            	crc >>= 1;
              crc ^= 0xA001;
          }
          else
          {
              crc >>=1;
          }
          bit_counter++;
		  }
	number_byte--;
	}
	return (crc >> 8) | (crc << 8);
}

//// Расчет LRC используется в алгоритме реализации протокола Modbus ASCII
/// На вход указатель на целевой пакет байтов
/// на выходе контрольная сумма LRC (отрицательная)
/// Она будет поставлена в конец пакета
uint8_t CalcLRC(uint8_t* dt, int16_t len)
{
	uint8_t res = 0;
	int16_t sum = 0;

	for(int16_t i = 0; i < len; i++)
	{
		sum += dt[i];
	}

	res = sum & 0x00FF;
  res ^= 0xFF;
  res += 1;

	return res;
}

/// Преобразование формата float в 2-х словную последовательность
/// для старшего слова
uint16_t GetFloatHi(float value)
{
	LG_RG_BT_FL lrbf;
	lrbf.fl = value;
	return lrbf.rg[0];
}

/// Преобразование формата float в 2-х словную последовательность
/// для младшего слова
uint16_t GetFloatLo(float value)
{
	LG_RG_BT_FL lrbf;
	lrbf.fl = value;
	return lrbf.rg[1];
}

/// Округление float до указанного числа цифр
float RoundMy(float val, int digit)
{
	int mult =1;
	float divider  =1.0;

	switch(digit)
	{
		case 0: divider = 1.0;
						mult =1;
		        break;
		case 1: divider = 10.0;
						mult =10;
		        break;
		case 2: divider = 100.0;
						mult =100;
		        break;
		case 3: divider = 1000.0;
						mult =1000;
		        break;
		case 4: divider = 10000.0;
						mult =10000;
		        break;
	}

		if(val >= 0.0)
		{
			val = roundf(val*mult)/divider;
		}
		else
		{
			val = roundf(-val*mult)/divider;
		}

		return val;
}

/// Преобразование целого числа десятичного к массиву цифр для
/// дальнейшего использования при выводе на индикатор
void Val32ToARRdigit(uint32_t val, uint8_t* arr)
{
	uint32_t ttt = 0;

	arr[0] = val / 10000000;
	ttt = val % 10000000;

	arr[1] = ttt / 1000000;
	ttt = ttt % 1000000;

	arr[2] = ttt / 100000;
	ttt = ttt % 100000;

	arr[3] = ttt / 10000;
	ttt = ttt % 10000;

	arr[4] = ttt / 1000;
	ttt = ttt % 1000;

	arr[5] = ttt / 100;
	ttt = ttt % 100;

	arr[6] = ttt / 10;
	ttt = ttt % 10;

	arr[7] = ttt;
}

/// Преобразование целого шестнадцатиричного числа к массиву цифр для
/// дальнейшего использования при выводе на индикатор
void  HEX32ToARRdigit(uint32_t val, uint8_t* arr)
{
	uint32_t ttt = 0;

	arr[0] = val / 0x10000000;
	ttt = val % 0x10000000;

	arr[1] = ttt / 0x1000000;
	ttt = ttt % 0x1000000;

	arr[2] = ttt / 0x100000;
	ttt = ttt % 0x100000;

	arr[3] = ttt / 0x10000;
	ttt = ttt % 0x10000;

	arr[4] = ttt / 0x1000;
	ttt = ttt % 0x1000;

	arr[5] = ttt / 0x100;
	ttt = ttt % 0x100;

	arr[6] = ttt / 0x10;
	ttt = ttt % 0x10;

	arr[7] = ttt;
}

/// Функции реверса чисел
uint64_t Uint64Revers(uint64_t val)
{
	uint64_t rev = 0;
	uint64_t mask = 0x0000000000000001;

	while(mask)
	{
    rev<<=1;
    if(val&mask)
        rev|=0x01;
    mask<<=1;
	}

	return rev;
}

uint64_t Uint64ReversByBytes(uint64_t val)
{
	uint64_t rev = 0;

	for(uint8_t i = 0; i < 8; i++)
	{
		rev |= val & 0xFF;
		rev <<= 8;
    val >>= 8;
	}

	return rev;
}

/// Примитивная функция задержки - применяется на этапах отладки ПО
void Waite(uint16_t cntdelay)
{
	volatile uint8_t ttt = 0;
	volatile uint16_t i = 0;
	for(i=0; i<cntdelay; i++)
		ttt += 1;
}
