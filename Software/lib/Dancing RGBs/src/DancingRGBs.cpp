#include "DancingRGBs.h"
#include <Arduino.h>

DancingRGBs::DancingRGBs(int rPin, int gPin, int bPin)
: r_pin(rPin), g_pin(gPin), b_pin(bPin)
{
    a =(maxIntensity-minIntensity)/(A_upperThreshold-A_lowerThreshold);
    b = maxIntensity - a * A_upperThreshold;
}


void DancingRGBs::readAudio()
{
    ADMUX = 0b00000000;       // use pin A0 and external voltage reference
    ADCSRA = 0b11010101;      // set ADC to free running mode and set pre-scalar to 32 (0xe5)

    while(!(ADCSRA & 0x10));        // wait for ADC to complete current conversion ie ADIF bit set

    //Serial.println(ADC);
    //Serial.print("\t");
    int value = ADC - DC_OFFSET;                 // Read from ADC and subtract DC offset caused value
    audioAnalogValue = value;
    addAmpToCache(value);
}

void DancingRGBs::addAmpToCache(float value)
{
    static int i;
    amp_cache[i] = value;
    i++;
    if(i>=CACHE_SIZE)i=0;
}


float DancingRGBs::getAvgFromCache()
{
    float avg = 0.0f;
    float max = 0.0f;
    for(int i=0; i<CACHE_SIZE; i++)
    {
        avg+=abs(amp_cache[i]);
        if(abs(amp_cache[i])>max)max = abs(amp_cache[i]);
    }
    maxInCache = max;
    return avg/CACHE_SIZE;
}
void DancingRGBs::loop()
{
    readAudio();
    getRGB_pwm();
    showLEDs();
}

void DancingRGBs::getRGB_pwm()
{
    unsigned int now = millis();
    float intensity;
    switch(currentMode)
    {
        case NOLEDS:
        {
            intensity = 0;
            RGB_pwm[0]=intensity;
            RGB_pwm[1]=intensity;
            RGB_pwm[2]=intensity;
            break;
        }

        case DANCE:
        {

            float avgFromCache = getAvgFromCache();
            float relAmplitude = abs(audioAnalogValue)/avgFromCache;

            
            if(maxInCache<minAudioInput || relAmplitude<A_lowerThreshold) intensity=calmIntensity;
            else
            {

                intensity = a * relAmplitude + b;
                intensity = constrain(intensity,calmIntensity, maxIntensity);
            }
            

            RGB_pwm[0]=sin_sq(now,transTime,0)*255*intensity;
            RGB_pwm[1]=sin_sq(now,transTime,60)*255*intensity;
            RGB_pwm[2]=sin_sq(now,transTime,120)*255*intensity;

            break;
        }

        case TRANSITION:
        {
            intensity = transModeIntensity;
            RGB_pwm[0]=sin_sq(now,transTime,0)*255*intensity;
            RGB_pwm[1]=sin_sq(now,transTime,60)*255*intensity;
            RGB_pwm[2]=sin_sq(now,transTime,120)*255*intensity;

            break;
        }

        case TORCH:
        {
            intensity = 255;
            RGB_pwm[0]=intensity;
            RGB_pwm[1]=intensity;
            RGB_pwm[2]=intensity;

            break;
        }
    }

    #ifdef AUDIO_DEBUG
    Serial.print(audioAnalogValue);
    Serial.print("\t");
    Serial.print(getAvgFromCache());
    Serial.print("\t");
    Serial.print(maxInCache);
    Serial.print("\t");
    Serial.print(minAudioInput);
    Serial.print("\t");
    Serial.print(intensity*20);
    Serial.print("\n");
    #endif
}


void DancingRGBs::showLEDs()
{
    analogWrite(r_pin,RGB_pwm[0]);
    analogWrite(g_pin,RGB_pwm[1]);
    analogWrite(b_pin,RGB_pwm[2]);
}

void DancingRGBs::nextMode()
{
    currentMode ++;
    if(currentMode>TORCH)currentMode = 0;
}

double sin_sq(long x, unsigned long T, int offset)
{
    double sinus = sin( ( (double)x / (double)T + (double)offset / 360.0) * 2*PI);
    return sinus*sinus;
}