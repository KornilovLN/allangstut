/*---------------------------------------------------------------------------------
 * Author: Starmark LN
 * e-mail: starmark.ln@ekatra.io
 * e-mail: starmark.ln@gmail.com
 * date:   09/6/2023  15:30:00
 *
 --------------------------------------------------------------------------------*/

#ifndef __GENERATOR_1__
#define __GENERATOR_1__

//=================================================================================
// Генератор-1
// Генератор произвольного сигнала для тестирования программы приемной стороны
// получили флоат значение, округлили к целому, записали в 16-ти битное число
// и 2 байта записали в младшую часть массива из 4-х байт
// Затем отправили 4 байта в uart
// приемная сторона использует 2 нулевых байта для синхронизации
//=================================================================================

#include <Arduino.h>
#include "math.h"

#define BAUDRATE1  19200 //230400   //--- Генератор-1

class Generator_1
{
  public:

  Generator_1(void);

  void Init(void);
  void Work(int16_t pause);

  
  private:
  
  typedef union UNI
  {
      int16_t data;
      uint8_t bt[2];
  }uni;

  typedef struct GENINT16
  {    
    float cnt;
    float dcnt;   
    uni vali16;
    uint8_t bt[4];
  }genint16;

  genint16 Vr;

  void Calc_GenInt16(void);
  void Out_GenInt16(void);
  
};

#endif  //__GENERATOR_1__
