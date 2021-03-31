#include <Arduino.h>
#include <wire.h>
#include <LiquidCrystal.h>
#include <arduinoFFT.h>
#include "stimmen.h"
#include <math.h>

#define CPU_CLOCK 84E6

// Pin Definitionen
#define AnalogIn 2
//LCD Pins
#define V0_Pin 3     //Pin für Kontrast
#define RS_Pin 50    // Register Select
#define E_Pin 48     //Enable
#define Data_Pin4 46 //DataPin 4bit Mode
#define Data_Pin5 44 //DataPin 4bit Mode
#define Data_Pin6 42 //DataPin 4bit Mode
#define Data_Pin7 40 //DataPin 4bit Mode

//Deklaration aller nötigen Variablen
const uint16_t samples = 512;
const double samplingFrequency = 8000;
uint32_t TIMER_TICKS;
Ton_t Ton;
double BaseFreq;
const float Toleranz = 0.01;
float delta_f;
uint16_t i = 0, *pt_i;
double vReal[samples], *pt_vReal;
double vImag[samples], *pt_vImag;

//Erstellen der Objekte
arduinoFFT FFT = arduinoFFT();
LiquidCrystal lcd(RS_Pin, E_Pin, Data_Pin4, Data_Pin5, Data_Pin6, Data_Pin7);

//Funktion zum Einrichten des Timers
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
//Eintragen der Werte in Arrays zum Samplen
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
  //Einrichten der Pins
  analogReadResolution(12);
  pinMode(2, INPUT);
  pinMode(V0_Pin, OUTPUT);
  analogWrite(V0_Pin, 100);
  //Zuweisen der Pointer zum Samplen
  pt_i = &i;
  pt_vReal = &vReal[0];
  pt_vImag = &vImag[0];
  //Timer initialisieren
  TIMER_TICKS = CPU_CLOCK / (samplingFrequency * 2);
  setupTimer(TC1, 1, TC4_IRQn, TIMER_TICKS);
  lcd.begin(16, 2);
}

void loop()
{
  /*SAMPLING*/
  //Sampling mit Timer-Interrupt
  i = 0;
  TC_Start(TC1, 1);
  while (i < samples)
  {
    delay(1);
  }
  //Sampling Ende

  //Berechnen der FFT
  FFT = arduinoFFT(vReal, vImag, samples, samplingFrequency);
  FFT.DCRemoval();
  FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(FFT_FORWARD);
  FFT.ComplexToMagnitude();   //Betragsbildung
  BaseFreq = FFT.MajorPeak(); //MaxFindung
  Ton = Ton_gespielt(BaseFreq);

  //Speichern der Abweichung in Variable
  delta_f = Abweichung(BaseFreq, Ton, Toleranz);

  //lcd Ausgabe
  lcd.setCursor(0, 0);
  lcd.print("Freq:");
  lcd.print(BaseFreq);
  lcd.setCursor(0, 1);

  if (delta_f > 0)
  { //Ton zu hoch
    lcd.print(" ");
    lcd.print(Ton.ton_name);
    lcd.setCursor(3, 1);
    lcd.print("<");
  }
  else if (delta_f < 0)
  { //Ton zu niedrig
    lcd.print(">");
    lcd.print(Ton.ton_name);
    lcd.setCursor(3, 1);
    lcd.print(" ");
  }
  else if (delta_f = 0)
  { //Ton richtig, bzw in Toleranz
    lcd.print(" ");
    lcd.print(Ton.ton_name);
    lcd.setCursor(3, 1);
    lcd.print(" ");
  }
  delay(80);
}
