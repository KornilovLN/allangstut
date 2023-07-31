/*---------------------------------------------------------------------------------
 * Author: Starmark LN
 * e-mail: starmark.ln@ekatra.io
 * e-mail: starmark.ln@gmail.com
 * date:   26/6/2023  14:30:00
 *
 --------------------------------------------------------------------------------*/

#include "generator3.h"

//=================================================================================
// Генератор-3
// Генератор строки для тестирования программы приемной стороны    
//=================================================================================

Generator_3::Generator_3(void)
{
}

void Generator_3::Init(void)
{
  Serial.begin(BAUDRATE3);
  
  mCounter = 0;
  mDivider = 7.2;

  mAms = 100.0; 
  mFrs = 17.0; 
  mPhs = 0.0;
  
  mAmc = 200.0; 
  mFrc = 7.0; 
  mPhc = 1.0;    
}

float Generator_3::Sn(float am, float fr, float ph, float t){
  return am*sin(PI180*fr*t + ph);  
}

float Generator_3::Cs(float am, float fr, float ph, float t){
  return am*cos(PI180*fr*t + ph); 
}

float Generator_3::Tn(float sn, float cs){
  return sn+cs;
}

float Generator_3::Tm(){
  
  if(mCounter == MAX_COUNTER)
    mCounter = 0;   

  uint16_t cnt = mCounter;
  ++mCounter;
  
  return (cnt)/mDivider;
}

float Generator_3::Rnd(int A){
  int cel = random(0, A);
  int drob = random(0, 100);
  return (float)cel + (float)drob / 100.0;
}

void Generator_3::Acc()
{
  mAcc.x = 100.0 + Rnd(3);
  mAcc.y = 200.0 + Rnd(5);
  mAcc.z = 300.0 + Rnd(10);
}

void Generator_3::Gir()
{
  mGir.x = 100.0 + Rnd(3);
  mGir.y = 200.0 + Rnd(5);
  mGir.z = 300.0 + Rnd(10);
}

void Generator_3::Mag()
{
  mMag.x = 300.0 + Rnd(3);
  mMag.y = 700.0 + Rnd(5);
  mMag.z = 500.0 + Rnd(10);
}

void Generator_3::Calc(void)
{
	t = Tm(); 
  sn = Sn(mAms, mFrs, mPhs, t);
  cs = Cs(mAmc, mFrc, mPhc, t);
  tn = Tn(sn, cs);  


  Acc(); 
  Gir();
  Mag();  
  
}

void Generator_3::Out0()
{
  Serial.print(sn);
  Serial.print(" ");
  Serial.print(cs);
  Serial.print(" ");
  Serial.print(tn);
  Serial.println();
}

void Generator_3::Out(void)
{
  Out0();
  /*
  Serial.print(sn);
  Serial.print(" ");
  Serial.print(cs);
  Serial.print(" ");
  Serial.println(tn);
  */
//  Serial.print(" ");

/*  
  Serial.print(mAcc.x);
  Serial.print(" ");
  Serial.print(mAcc.y);
  Serial.print(" ");
  Serial.print(mAcc.z);  
  Serial.print(" ");
  
  Serial.print(mGir.x);
  Serial.print(" ");
  Serial.print(mGir.y);
  Serial.print(" ");
  Serial.print(mGir.z); 
  Serial.print(" ");
  
  Serial.print(mMag.x);
  Serial.print(" ");
  Serial.print(mMag.y);
  Serial.print(" ");
  Serial.println(mMag.z);     
*/  
}

void Generator_3::Work(int16_t pause)
{
  while(1) 
  {
      Calc();
      Out();  
      delay(pause);
  }
} 
