#include <Arduino.h>
#include <wire.h>
#include <LiquidCrystal.h>
#include <arduinoFFT.h>
#include "stimmen.h"
#include <math.h>

arduinoFFT FFT = arduinoFFT();
LiquidCrystal lcd(51, 50, 40, 41, 42, 43, 10, POSITIVE);

// Pin Definitionen LCD
#define AnalogIn 2
#define CPU_CLOCK 84E6

unsigned long microseconds;
uint32_t TIMER_TICKS; // 5250, oder 5249, wenn 5250 == 8kHz ?
double BaseFreq;
uint16_t i = 0, *pt_i;
const uint16_t samples = 512;
const double samplingFrequency = 8000;
const float Toleranz = 0.01;
unsigned int sampling_period_us;
double vReal[samples], *pt_vReal;
double vImag[samples], *pt_vImag;
Ton_t Ton;

uint16_t maxAmplitudes = 0;

void setupTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t ticks_to_count)
{
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk((uint32_t)irq);
  NVIC_EnableIRQ(irq);
  TC_Configure(tc, channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK1);
  uint32_t rc = (uint32_t)ticks_to_count; //compare Value >> ticket with TIMER_CLOCK1 = MCK/2
  TC_SetRA(tc, channel, rc / 2);          // 50% high, 50%low
  TC_SetRC(tc, channel, rc);
  tc->TC_CHANNEL[channel].TC_IER = TC_IER_CPCS;  // RC Compare IRQ
  tc->TC_CHANNEL[channel].TC_IDR = ~TC_IER_CPCS; // disable all other IRQ sources
}

//Interrupt Handler
//Eintragen der Werte in Arrays
void TC4_Handler()
{
  TC_GetStatus(TC1, 1);
  if (*pt_i < samples)
  {
    *(pt_vReal + *pt_i) = analogRead(AnalogIn);
    *(pt_vImag + *pt_i) = 0;
    *pt_i = *pt_i + 1;
  }
  else
    TC_Stop(TC1, 1);
}

void setup()
{
  TIMER_TICKS = CPU_CLOCK/(samplingFrequency*2);
  lcd.begin(16, 2);
  analogReadResolution(12);
  //sampling_period_us = round(1000000 * (1.0 / samplingFrequency));
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Ready");
  pinMode(2, INPUT);
  pt_i = &i;
  pt_vReal = &vReal[0];
  pt_vImag = &vImag[0];
  setupTimer(TC1, 1, TC4_IRQn, TIMER_TICKS);
}

void loop()
{
  /*SAMPLING*/
  /*
   //mit Arduino Core
  microseconds = micros();
  for (int i = 0; i < samples; i++)
  {
    vReal[i] = analogRead(AnalogIn);
    vImag[i] = 0;
    
   while (micros() - microseconds < sampling_period_us)
    {
      //empty loop
    }
    microseconds += sampling_period_us;
  }
    */
  //Sampling mit Timer-Interrupt
  i = 0;
  TC_Start(TC1, 1);
  while (i < samples)
  {
    delay(1); //WARUM??
  }
  //Sampling Ende
  //Berechnen der FFT
  FFT = arduinoFFT(vReal, vImag, samples, samplingFrequency);
  FFT.DCRemoval();
  FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(FFT_FORWARD);
  FFT.ComplexToMagnitude();   //Betragsbildung
  BaseFreq = FFT.MajorPeak(); //MaxFindung
  if (analogRead(2) > maxAmplitudes)
  { //Ausgabe d. Maximalen Input-Pegels
    maxAmplitudes = analogRead(2);
    Serial.print("\t\t\t New max:");
    Serial.print(maxAmplitudes);
    Serial.print("\n");
  }
  Ton = Ton_gespielt(BaseFreq);
  //Ausgabe der berechneten Frequenz
  if(Abweichung(BaseFreq,Ton,Toleranz)>0){
    Serial.println("Stimm runter");
  }else if(Abweichung(BaseFreq,Ton,Toleranz)<0) Serial.println("Stimm hoch");
  Serial.println(BaseFreq);
  Serial.print(" Ton:");
  Serial.println(Ton.ton_name);
  delay(50);
  /*
  //lcd Ausgabe
  lcd.setCursor(0, 0);
  lcd.print("Frequenz:");
  lcd.setCursor(0, 1);
  lcd.print(BaseFreq);
  Serial.print("\n");
  delay(100); */
}
