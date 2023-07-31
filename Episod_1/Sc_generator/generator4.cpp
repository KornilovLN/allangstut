/*---------------------------------------------------------------------------------
 * Author: Starmark LN
 * e-mail: starmark.ln@ekatra.io
 * e-mail: starmark.ln@gmail.com
 * date:   30/6/2023  14:30:00
 *
 --------------------------------------------------------------------------------*/

#include "generator4.h"

//=================================================================================
// Генератор-4
// Генератор строки для тестирования программы приемной стороны    
//=================================================================================

Generator_4::Generator_4(){}

void Generator_4::Init(void)
{
  Serial.begin(BAUDRATE4);
  
  mCounter = 0;
  mDivider = 7.2;

  mAms = 100.0; 
  mFrs = 17.0; 
  mPhs = 0.0;
  
  mAmc = 200.0; 
  mFrc = 7.0; 
  mPhc = 1.0;   

  mPortval.ports = 0; 
}

void Generator_4::GenPort(void){
  static uint32_t cnt = 0;

  cnt += 1;
  if(cnt == 256){
    cnt = 0;
  }
  
  mPortval.ports = cnt;
}

float Generator_4::Sn(float am, float fr, float ph, float t){
  return am*sin(PI180*fr*t + ph);  
}

float Generator_4::Cs(float am, float fr, float ph, float t){
  return am*cos(PI180*fr*t + ph); 
}

float Generator_4::Tn(float sn, float cs){
  return sn+cs;
}

float Generator_4::Tm(void){
  
  if(mCounter == MAX_COUNTER)
    mCounter = 0;   

  uint16_t cnt = mCounter;
  ++mCounter;
  
  return (cnt)/mDivider;
}

float Generator_4::Rnd(int A){
  int cel = random(0, A);
  int drob = random(0, 100);
  return (float)cel + (float)drob / 100.0;
}

void Generator_4::Acc(void)
{
  mAcc.x = 0.0 + Rnd(7);
  mAcc.y = 100.0 + Rnd(5);
  mAcc.z = 200.0 + Rnd(10);
}

void Generator_4::Gir(void)
{
  mGir.x = 300.0 + Rnd(13);
  mGir.y = 400.0 + Rnd(9);
  mGir.z = 500.0 + Rnd(7);
}

void Generator_4::Mag(void)
{
  mMag.x = 600.0 + Rnd(3);
  mMag.y = 700.0 + Rnd(5);
  mMag.z = 800.0 + Rnd(10);
}

void Generator_4::Bme(void)
{
  mBme.x = 25.0 + Rnd(2);
  mBme.y = 680.0 + Rnd(3);
  mBme.z = 45.0 + Rnd(7);
}

void Generator_4::Alarm(void)
{
  static int cnt = 0;

  if( ((cnt > 50)&&(cnt < 75)) || ((cnt > 150)&&(cnt < 200)))
    mAlarm = 1;
  else
    mAlarm = 0;

  cnt++;
  if(cnt >= 256)
    cnt = 0;  
}

void Generator_4::Calc(void)
{
  GenPort(); 
  
	t = Tm(); 
  sn = Sn(mAms, mFrs, mPhs, t);
  cs = Cs(mAmc, mFrc, mPhc, t);
  tn = Tn(sn, cs);  

  Acc(); 
  Gir();
  Mag();    
  Bme();

  Alarm();
}

void Generator_4::OutPort(void){

  Serial.print(mPortval.ports);
  Serial.print(" ");
}

void Generator_4::Out0(void)
{
  Serial.print(sn);
  Serial.print(" ");
  Serial.print(cs);
  Serial.print(" ");
  Serial.print(tn);
  Serial.print(" ");
}

void Generator_4::Out1(void)
{
  Serial.print(mAcc.x);
  Serial.print(" ");
  Serial.print(mAcc.y);
  Serial.print(" ");
  Serial.print(mAcc.z);  
  Serial.print(" ");
}

void Generator_4::Out2(void)
{
  Serial.print(mGir.x);
  Serial.print(" ");
  Serial.print(mGir.y);
  Serial.print(" ");
  Serial.print(mGir.z); 
  Serial.print(" ");
}

void Generator_4::Out3(void)
{
  Serial.print(mMag.x);
  Serial.print(" ");
  Serial.print(mMag.y);
  Serial.print(" ");
  Serial.print(mMag.z);  
  Serial.print(" ");
}

void Generator_4::Out4(void)
{
  Serial.print(mBme.x);
  Serial.print(" ");
  Serial.print(mBme.y);
  Serial.print(" ");
  Serial.print(mBme.z);
  Serial.print(" ");  
}

void Generator_4::OutAlarm(void)
{
  Serial.println(mAlarm);
  
}

void Generator_4::Out(void)
{
  OutPort();
  Out0();
  Out1();
  Out2();
  Out3();   
  Out4();
  OutAlarm(); 
}

void Generator_4::Work(int16_t pause)
{
  while(1) 
  {
      Calc();
      Out();  
      delay(pause);
  }
} 
