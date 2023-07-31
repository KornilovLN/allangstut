/*---------------------------------------------------------------------------------
 * Author: Starmark LN
 * e-mail: starmark.ln@ekatra.io
 * e-mail: starmark.ln@gmail.com
 * date:   09/6/2023  15:30:00
 *
 --------------------------------------------------------------------------------*/

#ifndef __GENERATOR_2__
#define __GENERATOR_2__

//=================================================================================
// Генератор-2
// Генератор особого сигнала (Лазер) для тестирования программы приемной стороны
// Парсинга данных смешанного формата
// Также - генерация переменной в таком же смешанном формате
// и отправка всего этого в виде цепочки байт с заголовочным байтом 0x78
//
//  1-е число индикации в 3-х байтах поцифренно /*/*
//  FFP - Формат числа с фиксированной точкой и ед. изм.
//  Тот самый особый формат
//
//  0100 dddd    0110 dddd    dddd dddd
//  -    dig1         dig2    dig3 dig4
//
//   unit:       format: 
//   dBm         ddd.d
// 
//  Сгенерипрвать значение в пределах:
//  {-999.9  ...  +999.9}
//
//  сгенерировать float |val|=nominal  
//  и преобразовать к формату FFP:
//=================================================================================

#include <Arduino.h>
#include "math.h"

#define BAUDRATE2  19200    //--- Генератор-2

#define UNIT 0x60           //--- Это тип ед. изм.
#define FORM 0x40           //--- Это формат последовательности {0x4x, 0x6x, 0x7x, oxFx}
#define MAXNOMINAL 999.9    //--- В особом формате числа не превышают этого значения


class Generator_2
{
  public:

  Generator_2(void);

  void Init(void);
  void Work(int16_t pause);

  
  private:

  //--- рабочие переменные
  uint8_t num;
  uint8_t* arr;
  int sz;

  float Func(float nominal, float basefreq, float diffreq);
  void Val_to_FFP(uint8_t *bytes);
  void generator(uint8_t *arr_targ, uint8_t sz);
  
  void Calc_GenInt16(void);
  void Out_GenInt16(void);
  
};

/*
#define UNIT 0x60           //--- Это тип ед. изм.
#define FORM 0x40           //--- Это формат последовательности {0x4x, 0x6x, 0x7x, oxFx}
#define MAXNOMINAL 999.9    //--- В особом формате числа не превышают этого значения

extern int ledPin; 

void SerialSetup2(void);
void Work_Gen(int16_t pause);
*/

#endif  //__GENERATOR_2__
