#define DC_OFFSET 508
#define CACHE_SIZE 50

//#define AUDIO_DEBUG 

enum Mode
{
    DANCE,
    NOLEDS,
    TRANSITION,
    TORCH
};

class DancingRGBs
{
int r_pin, g_pin, b_pin;

double audioAnalogValue;
double amp_cache[CACHE_SIZE];
double maxInCache;
int currentMode = DANCE;

//TRANSITION mode vars
float transModeIntensity = 0.9f;
unsigned long transTime = 6000;

//DANCE mode vars

//A_Threshold - relative to averageAmplitude. LEDS are on between the lower and the upper one.
float A_upperThreshold = 2.2f;
float A_lowerThreshold = 1.5f;

//LED brightness -> 1.0 means full brightness
float maxIntensity = 1.0f;
float minIntensity = 0.4f;

//intensity = a * relative_amplitude + b ;; for each amplitude in between the thresholds
float a, b;

//brightness when there is trigger
float calmIntensity = 0.0f;

//if the audio input is below this value, ignore it
float minAudioInput = 5.0f;

//Red, Green, Blue current PWM array
double RGB_pwm[3];

private:
void readAudio();
void addAmpToCache(float value);
float getAvgFromCache();
void getRGB_pwm();
void showLEDs();


public:
DancingRGBs(int rPin, int gPin, int bPin);
void loop();
void nextMode();
};


double sin_sq(long x, unsigned long T, int offset);


   