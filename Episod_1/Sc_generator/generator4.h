/*---------------------------------------------------------------------------------
 * Author: Starmark LN
 * e-mail: starmark.ln@ekatra.io
 * e-mail: starmark.ln@gmail.com
 * date:   30/6/2023  13:30:00
 *
 --------------------------------------------------------------------------------*/


#ifndef __GENERATOR_4__
#define __GENERATOR_4__

//=================================================================================
// Генератор-4
// Генератор строки для тестирования программы приемной стороны
//=================================================================================

#include <Arduino.h>
#include <math.h>


#define BAUDRATE4  19200 //230400   //--- Генератор-4
#define MAX_COUNTER 36000

#define PI180   (PI/180.0)

class Generator_4
{
  public:

  Generator_4();

  void Init(void);
  void Work(int16_t pause);
  
  private:

  typedef struct {
    float x; 
    float y; 
    float z;
  }stFloat_t;

  typedef union {
    uint32_t  ports;
    uint8_t arrbt[4];
  }uniPortVal;


  void GenPort(void);  
  float Sn(float am, float fr, float ph, float t);
  float Cs(float am, float fr, float ph, float t);
  float Tn(float sn, float cs);
  float Tm();

  void Acc(void); 
  void Gir(void);
  void Mag(void);
  void Bme(void);
  void Alarm(void);
  
  float Rnd(int A);
  void OutPort(void); 
  void Out0(void);
  void Out1(void);
  void Out2(void);
  void Out3(void);
  void Out4(void);
  void OutAlarm(void);

  void Calc(void);
  void Out(void);

  uint16_t mCounter;
  float mDivider;

  uniPortVal mPortval;

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
  stFloat_t mBme;  

  uint8_t mAlarm;
  
  String mS = "";	
  
};

#endif  //__GENERATOR_4__
