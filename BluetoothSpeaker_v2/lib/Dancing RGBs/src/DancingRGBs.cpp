#include "DancingRGBs.h"

DancingRGBs::DancingRGBs(int rPin, int gPin, int bPin)
: r_pin(rPin), g_pin(gPin), b_pin(bPin)
{
    FFT = arduinoFFT();
}


void DancingRGBs::getSpectrum()
{
    getSamples(vReal, vImag, SAMPLES);
    
    // ++ FFT
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
    // -- FFT

}

void DancingRGBs::getSamples(double* Re, double* Im, int samp)
{
    ADCSRA = 0b11100101;      // set ADC to free running mode and set pre-scalar to 32 (0xe5)
    ADMUX = 0b00000000;       // use pin A0 and external voltage reference

    // ++ Sampling
   for(int i=0; i<samp; i++)
    {
      while(!(ADCSRA & 0x10));        // wait for ADC to complete current conversion ie ADIF bit set
      ADCSRA = 0b11110101 ;               // clear ADIF bit so that ADC can do next operation (0xf5)
      Serial.print(ADC);
      Serial.print("\t");
      int value = ADC - DC_OFFSET;                 // Read from ADC and subtract DC offset caused value
    Serial.println(value);
      Re[i]= value/8;                      // Copy to bins after compressing
      Im[i] = 0;                         
    }
    // -- Sampling
}

void DancingRGBs::displayAllInConsole()
{
    for(int i=0; i< SAMPLES; i++)
    {
        Serial.print(vReal[i]);
        Serial.print("\t");
    }
    Serial.println();
}

void DancingRGBs::dance()
{
    if(vReal[5]>10)digitalWrite(r_pin,HIGH);
    else digitalWrite(r_pin,LOW);
}