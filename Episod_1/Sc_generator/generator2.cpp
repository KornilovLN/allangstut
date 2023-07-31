/*---------------------------------------------------------------------------------
 * Author: Starmark LN
 * e-mail: starmark.ln@ekatra.io
 * e-mail: starmark.ln@gmail.com
 * date:   09/6/2023  15:30:00
 *
 --------------------------------------------------------------------------------*/

#include "generator2.h"

//=================================================================================
// Генератор-2
// Генератор особого сигнала (Лазер) для тестирования программы приемной стороны
// Парсинга данных смешанного формата
// Также - генерация переменной в таком же смешанном формате
// и отправка всего этого в виде цепочки байт с заголовочным байтом 0x78
//=================================================================================

//--- Примеры реальных посылок устройства "Лазер" в последовательный порт 
uint8_t arr1[] = {0x7e, 0x45, 0x0, 0x29, 0x69, 0x99, 0x14, 0x8f, 0x0, 0x8, 0x50, 0xa0, 0x61, 0x2, 0xec};
uint8_t arr2[] = {0x7e, 0x45, 0x83, 0xc1, 0x28, 0x65, 0x9e, 0x3, 0x1, 0x4, 0x79, 0x22, 0x64, 0x37, 0x70};
uint8_t arr3[] = {0x7e, 0x45, 0x5, 0x80, 0x62, 0x34, 0x2, 0x9e, 0x1, 0x6, 0x14, 0x12, 0x62, 0x89, 0x96 };
uint8_t arr4[] = {0x7e, 0x63, 0x53, 0x29, 0x6a, 0x36, 0x0, 0x8, 0x80, 0x80, 0x1, 0x2, 0x08  };
uint8_t arr5[] = {0x7e, 0x63, 0x24, 0x80, 0x61, 0x45, 0x1, 0x5, 0x50, 0x0, 0x0, 0x72, 0xf3  };
uint8_t arr6[] = {0x7e, 0x70, 0x4, 0x10, 0x3, 0x34, 0x5, 0x46, 0x0, 0x84  };

uint8_t arr11[] = {0x7e, 0x18, 0x4, 0x5, 0x1e, 0xc, 0x14, 0xdd }; 
uint8_t arr12[] = {0x7e, 0x18, 0x3, 0x6, 0xe, 0x82, 0x6,  0x35 }; 
uint8_t arr13[] = {0x7e, 0x18, 0x5, 0x6, 0xe, 0x81, 0x98, 0xc8 }; 
uint8_t arr14[] = {0x7e, 0x2a, 0x1, 0x3, 0x2, 0x5,  0x1e, 0x81, 0x98, 0xea }; 
uint8_t arr15[] = {0x7e, 0x7a, 0x4, 0x10, 0x3, 0x34, 0x5, 0x46, 0x0, 0x8E };

uint8_t arr7[] = {0x7e, 0xf1, 0x0, 0x6f };
uint8_t arr7A[]= {0x7e, 0xf1, 0xaa, 0xf, 0x28  };
uint8_t arr8[] = {0x7e, 0xf2, 0x0, 0x70 }; 

extern int ledPin; 


Generator_2::Generator_2(void)
{
}

void Generator_2::Init(void)
{
  Serial.begin(BAUDRATE2);
  
  //--- рабочие переменные
  uint8_t num = 0;
  uint8_t* arr = arr2;    //--- Работаем пока только с этим вариантом последовательности
  int sz = sizeof(arr2);
}

// Вычисление значений для функции генератора:
// float target = gen(999.9, 3.0, 7.0);
float Generator_2::Func(float nominal, float basefreq, float diffreq)
{
  float res = 1000.0;
  static float tm = 0.0;
  static float dtm = 0.1;
  
  if(nominal <= MAXNOMINAL)
  {
    res = nominal * sin(PI * basefreq * tm/180.0);   
  }
  tm += dtm;

  return res; 
}

// применение функции преобразователя к формату FFP:
// uint8_t* valFFP = Val_to_FFP(float val);
void Generator_2::Val_to_FFP(uint8_t *bytes)
{
  uint8_t arr_dt[3] = {0,}; 
  float val = 0.0;
  uint8_t bt = 0;

  float value = Func(999.9, 3.0, 7.0);
  
  if(!( (value >= -999.9) && (value <= 999.9) ))
  { 
    val = 0.0; 
  }
  else
  { 
    val = abs(value);
  }

  val *= 10;
    
  if(value < 0.0)
  {
    arr_dt[0] = 0x80 + FORM;
  }
  else
  { 
    arr_dt[0] = FORM; //--- Мы выбрали вариант 0x40
  }

  bt = (uint8_t)(trunc(val/1000.0));
  arr_dt[0] |= bt;
  val -= (float)bt*1000.0;
    
  arr_dt[1] = UNIT; // 0x60
  
  bt = (uint8_t)(trunc(val/100.0));
  arr_dt[1] |= bt;
  val -= (float)bt*100.0;
    
  bt = (uint8_t)(trunc(val/10.0));
  arr_dt[2] |= bt<<4;
  val -= (float)bt*10.0;
    
  bt = (uint8_t)(trunc(val));
  arr_dt[2] |= bt;
  
  bytes[0] = arr_dt[0]; 
  bytes[1] = arr_dt[1];
  bytes[2] = arr_dt[2];  
}

void Generator_2::generator(uint8_t *arr_targ, uint8_t sz)
{
  uint8_t bytes[3] = {0,};
  uint16_t crc = 0;
   
  Val_to_FFP(bytes);  
  
  arr_targ[3] = bytes[0];
  arr_targ[4] = bytes[1];  
  arr_targ[5] = bytes[2];
  
  for(int i = 0; i < sz-1; i++)
  {
    crc += arr_targ[i];
  }
  arr_targ[sz-1] = (uint8_t)(crc & 0x00FF);  
}

void Generator_2::Work(int16_t pause)
{
    while (true)
    {
      switch(num)
      {
        case 0://--- последовательность arr2 идет в порт
        
               arr = arr2;  
               sz = sizeof(arr2); 
               
               for(int i = 0; i < sz ; i++) 
               {
                generator(arr, sz); 
                Serial.write(arr[i]);               
               }
               
               num = 1;
               break;
               
        case 1://--- последовательность arr6 идет в порт
        
               arr = arr6;  
               sz = sizeof(arr6);  
               delay(250);
               
               for(int i = 0; i < sz ; i++) 
               { 
                Serial.write(arr[i]); 
               }
                           
               delay(850);
               num = 0;
               break; 
      }

      digitalWrite(ledPin, HIGH); 
      delay(pause);                  
      digitalWrite(ledPin, LOW);    
    
    }  
}

//=================================================================================
// Генератор-2
// Генератор особого сигнала (Лазер) для тестирования программы приемной стороны
// Парсинга данных смешанного формата
// Также - генерация переменной в таком же смешанном формате
// и отправка всего этого в виде цепочки байт с заголовочным байтом 0x78
//=================================================================================
/*
//--- Примеры реальных посылок устройства "Лазер" в последовательный порт 
uint8_t arr1[] = {0x7e, 0x45, 0x0, 0x29, 0x69, 0x99, 0x14, 0x8f, 0x0, 0x8, 0x50, 0xa0, 0x61, 0x2, 0xec};
uint8_t arr2[] = {0x7e, 0x45, 0x83, 0xc1, 0x28, 0x65, 0x9e, 0x3, 0x1, 0x4, 0x79, 0x22, 0x64, 0x37, 0x70};
uint8_t arr3[] = {0x7e, 0x45, 0x5, 0x80, 0x62, 0x34, 0x2, 0x9e, 0x1, 0x6, 0x14, 0x12, 0x62, 0x89, 0x96 };
uint8_t arr4[] = {0x7e, 0x63, 0x53, 0x29, 0x6a, 0x36, 0x0, 0x8, 0x80, 0x80, 0x1, 0x2, 0x08  };
uint8_t arr5[] = {0x7e, 0x63, 0x24, 0x80, 0x61, 0x45, 0x1, 0x5, 0x50, 0x0, 0x0, 0x72, 0xf3  };
uint8_t arr6[] = {0x7e, 0x70, 0x4, 0x10, 0x3, 0x34, 0x5, 0x46, 0x0, 0x84  };

uint8_t arr11[] = {0x7e, 0x18, 0x4, 0x5, 0x1e, 0xc, 0x14, 0xdd }; 
uint8_t arr12[] = {0x7e, 0x18, 0x3, 0x6, 0xe, 0x82, 0x6,  0x35 }; 
uint8_t arr13[] = {0x7e, 0x18, 0x5, 0x6, 0xe, 0x81, 0x98, 0xc8 }; 
uint8_t arr14[] = {0x7e, 0x2a, 0x1, 0x3, 0x2, 0x5,  0x1e, 0x81, 0x98, 0xea }; 
uint8_t arr15[] = {0x7e, 0x7a, 0x4, 0x10, 0x3, 0x34, 0x5, 0x46, 0x0, 0x8E };

uint8_t arr7[] = {0x7e, 0xf1, 0x0, 0x6f };
uint8_t arr7A[]= {0x7e, 0xf1, 0xaa, 0xf, 0x28  };
uint8_t arr8[] = {0x7e, 0xf2, 0x0, 0x70 }; 

//--- рабочие переменные
uint8_t num;
uint8_t* arr = arr2; 
int sz = sizeof(ar2);

void SerialSetup2(void)
{
    Serial.begin(BAUDRATE2);    
}


// Вычисление значений для функции генератора:
// float target = gen(999.9, 3.0, 7.0);
float gen(float nominal, float basefreq, float diffreq)
{
  float res = 1000.0;
  static float tm = 0.0;
  static float dtm = 0.1;
  
  if(nominal <= MAXNOMINAL)
  {
    res = nominal * sin(PI * basefreq * tm/180.0);   
  }
  tm += dtm;

  return res; 
}


// применение функции преобразователя к формату FFP:
// uint8_t* valFFP = Val_to_FFP(float val);
void Val_to_FFP(uint8_t *bytes)
{
  uint8_t arr_dt[3] = {0,}; 
  float val = 0.0;
  uint8_t bt = 0;

  float value = gen(999.9, 3.0, 7.0);
  
  if(!( (value >= -999.9) && (value <= 999.9) ))
  { 
    val = 0.0; 
  }
  else
  { 
    val = abs(value);
  }

  val *= 10;
    
  if(value < 0.0)
  {
    arr_dt[0] = 0x80 + FORM;
  }
  else
  { 
    arr_dt[0] = FORM; //--- Мы выбрали вариант 0x40
  }

  bt = (uint8_t)(trunc(val/1000.0));
  arr_dt[0] |= bt;
  val -= (float)bt*1000.0;
    
  arr_dt[1] = UNIT; // 0x60
  
  bt = (uint8_t)(trunc(val/100.0));
  arr_dt[1] |= bt;
  val -= (float)bt*100.0;
    
  bt = (uint8_t)(trunc(val/10.0));
  arr_dt[2] |= bt<<4;
  val -= (float)bt*10.0;
    
  bt = (uint8_t)(trunc(val));
  arr_dt[2] |= bt;
  
  bytes[0] = arr_dt[0]; 
  bytes[1] = arr_dt[1];
  bytes[2] = arr_dt[2];  
}


void generator(uint8_t *arr_targ, uint8_t sz)
{
  uint8_t bytes[3] = {0,};
  uint16_t crc = 0;
   
  Val_to_FFP(bytes);  
  
  arr_targ[3] = bytes[0];
  arr_targ[4] = bytes[1];  
  arr_targ[5] = bytes[2];
  
  for(int i = 0; i < sz-1; i++)
  {
    crc += arr_targ[i];
  }
  arr_targ[sz-1] = (uint8_t)(crc & 0x00FF);  
}

  
void Work_Gen(int16_t pause)
{
    while (true)
    {
      switch(num)
      {
        case 0://--- последовательность arr2 идет в порт
        
               arr = arr2;  sz = sizeof(arr2); 
               for(int i = 0; i < sz ; i++) 
               {
                generator(arr, sz); 
                Serial.write(arr[i]);               
               }
               
               num = 1;
               break;
               
        case 1://--- последовательность arr6 идет в порт
        
               arr = arr6;  sz = sizeof(arr6);  
               delay(250);
               for(int i = 0; i < sz ; i++) 
               { 
                Serial.write(arr[i]); 
               }
                           
               delay(850);
               num = 0;
               break; 
      }

      digitalWrite(ledPin, HIGH); 
      delay(pause);                  
      digitalWrite(ledPin, LOW);    
    
    }  
}
*/
