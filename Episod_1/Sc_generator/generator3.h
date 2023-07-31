/*---------------------------------------------------------------------------------
 * Author: Starmark LN
 * e-mail: starmark.ln@ekatra.io
 * e-mail: starmark.ln@gmail.com
 * date:   26/6/2023  13:30:00
 *
 --------------------------------------------------------------------------------*/

#ifndef __GENERATOR_3__
#define __GENERATOR_3__

//=================================================================================
// Генератор-3
// Генератор строки для тестирования программы приемной стороны
//=================================================================================

#include <Arduino.h>
#include <math.h>


#define BAUDRATE3  19200 //230400   //--- Генератор-3
#define MAX_COUNTER 36000

#define PI180   (PI/180.0)

class Generator_3
{
  public:

  Generator_3(void);

  void Init(void);
  void Work(int16_t pause);

  
  private:

  typedef struct {
    float x; 
    float y; 
    float z;
  }stFloat_t;

  float Sn(float am, float fr, float ph, float t);
  float Cs(float am, float fr, float ph, float t);
  float Tn(float sn, float cs);
  float Tm();

  void Acc(); 
  void Gir();
  void Mag();

  float Rnd(int A);
  void Out0();

  void Calc(void);
  void Out(void);

  uint16_t mCounter;
  float mDivider;

  float t; 
  float sn;
  float cs;
  float tn;    

  float mAms; 
  float mFrs; 
  float mPhs;
  
  float mAmc; 
  float mFrc; 
  float mPhc;

  stFloat_t mAcc;
  stFloat_t mGir;
  stFloat_t mMag;
  
  String mS = "";	
  
};

#endif  //__GENERATOR_3__
