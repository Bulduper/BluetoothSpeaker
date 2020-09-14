#include "DancingRGBs.h"
#include <Arduino.h>

DancingRGBs::DancingRGBs(int rPin, int gPin, int bPin)
: r_pin(rPin), g_pin(gPin), b_pin(bPin)
{
}


void DancingRGBs::readAudio()
{
    ADCSRA = 0b11100101;      // set ADC to free running mode and set pre-scalar to 32 (0xe5)
    ADMUX = 0b00000000;       // use pin A0 and external voltage reference


    while(!(ADCSRA & 0x10));        // wait for ADC to complete current conversion ie ADIF bit set
    ADCSRA = 0b11110101 ;               // clear ADIF bit so that ADC can do next operation (0xf5)
    //Serial.print(ADC);
    //Serial.print("\t");
    int value = ADC - DC_OFFSET;                 // Read from ADC and subtract DC offset caused value
    audioAnalogValue = value;
    addAmpToCache(value);
    //Serial.println(value);

}

void DancingRGBs::addAmpToCache(float value)
{
    static int i;
    amp_cache[i] = value;
    i++;
    if(i>=CACHE_SIZE)i=0;
}

float DancingRGBs::getMaxFromCache()
{
    float max = 0.0f;
    for(int i=0; i<CACHE_SIZE; i++)
    {
        if(abs(amp_cache[i])>max)max = abs(amp_cache[i]);
    }
    return max;
}

float DancingRGBs::getIntensity()
{
    switch(currentMode)
    {
        case NOLEDS:
        {
            return 0.0f;
            break;
        }

        case DANCE:
        {
            float intensity;
            float maxFromCache = getMaxFromCache();
            float relAmplitude = audioAnalogValue/maxFromCache;
            float a, b;
            
            if(maxFromCache<minAudioInput) return calmIntensity;
            a =(maxIntensity-minIntensity)/(maxAmplitude-minAmplitude);
            b = maxIntensity - a*maxAmplitude;
            intensity = a * relAmplitude + b;
            intensity = constrain(intensity,calmIntensity, maxIntensity);
            return intensity;
            break;
        }

        case TRANSITION:
        {
            return transModeIntensity;
            break;
        }
    }
}


void DancingRGBs::dance()
{
    unsigned int now = millis();
    float intensity = getIntensity();

    analogWrite(r_pin,sin_half_sq(now,6000,0)*255*intensity);
    analogWrite(g_pin,sin_half_sq(now,6000,60)*255*intensity);
    analogWrite(b_pin,sin_half_sq(now,6000,120)*255*intensity);

    //Serial.println(getMaxFromCache());
}

double sin_half_sq(long x, unsigned long T, int offset)
{
    double sinus = sin( ( (double)x / (double)T + (double)offset / 360.0) * 2*PI);
    //if(sinus<0.0)sinus = 0.0;
    return sinus*sinus;
}