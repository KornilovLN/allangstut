/*---------------------------------------------------------------------------------
 * Author: Starmark LN
 * e-mail: starmark.ln@ekatra.io
 * e-mail: starmark.ln@gmail.com
 * date:   09/6/2023  15:30:00
 *
 --------------------------------------------------------------------------------*/

#include "generator1.h"

//=================================================================================
// Генератор-1
// Генератор произвольного сигнала для тестирования программы приемной стороны
// получили флоат значение, округлили к целому, записали в 16-ти битное число
// и 2 байта записали в младшую часть массива из 4-х байт
// Затем отправили 4 байта в uart
// приемная сторона использует 2 нулевых байта для синхронизации
//=================================================================================

Generator_1::Generator_1(void)
{
}

void Generator_1::Init(void)
{
  Serial.begin(BAUDRATE1);
  
  Vr.cnt = 0.0;
  Vr.dcnt = 0.1;  
  Vr.vali16.data = 0;  
  Vr.bt[0] = 0;
  Vr.bt[1] = 0;
  Vr.bt[2] = 0;
  Vr.bt[3] = 0;  
}

void Generator_1::Calc_GenInt16(void)
{
  float res = 32000.0*sin(3.141593*Vr.cnt*53 * sin(3.141593*Vr.cnt*3/180.0 + 1.45) /180.0 + 1.45) +  100*sin(3.141593*Vr.cnt*11/180.0 + 1.45);
  Vr.vali16.data = (int16_t)round(res);

  Vr.bt[2] = Vr.vali16.bt[0]; //--- младший
  Vr.bt[3] = Vr.vali16.bt[1]; //--- старший
}

void Generator_1::Out_GenInt16(void)
{
  //=== Отправили в порт
  Serial.write(Vr.bt,4);      
  Vr.cnt += Vr.dcnt;   
}

void Generator_1::Work(int16_t pause)
{
  while(1) 
  {
      Calc_GenInt16();
      Out_GenInt16();  
      delay(pause);
  }
} 

/*
void SerialSetup1(void)
{
    Serial.begin(BAUDRATE1);    
}

void Init_GenInt16(genint16 *vars)
{
  vars->cnt = 0.0;
  vars->dcnt = 0.1;
  
  vars->vali16.data = 0;
  
  vars->bt[0] = 0;
  vars->bt[1] = 0;
  vars->bt[2] = 0;
  vars->bt[3] = 0;
}

void Calc_GenInt16(genint16 *vars)
{
  float res = 32000.0*sin(3.141593*vars->cnt*53 * sin(3.141593*vars->cnt*3/180.0 + 1.45) /180.0 + 1.45) +  100*sin(3.141593*vars->cnt*11/180.0 + 1.45);
  vars->vali16.data = (int16_t)round(res);

  vars->bt[2] = vars->vali16.bt[0]; //--- младший
  vars->bt[3] = vars->vali16.bt[1]; //--- старший
}

void Out_GenInt16(genint16 *vars)
{
  //=== Отправили в порт
  Serial.write(vars->bt,4);      
  vars->cnt += vars->dcnt;   
}

void Work_Gen1(int16_t pause)
{
  genint16 workvars; 

  Init_GenInt16(&workvars);
   
  while(1) 
  {
      Calc_GenInt16(&workvars);
      Out_GenInt16(&workvars);  
      delay(pause);
  }
} 
*/   
