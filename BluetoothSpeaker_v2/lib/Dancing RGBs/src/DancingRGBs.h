#define SAMPLES 16
#define DC_OFFSET 518
#define CACHE_SIZE 100

enum Mode
{
    NOLEDS,
    DANCE,
    TRANSITION
};

class DancingRGBs
{
int r_pin, g_pin, b_pin;

double audioAnalogValue;
double amp_cache[CACHE_SIZE];
Mode currentMode = DANCE;

//TRANSITION mode vars
float transModeIntensity = 0.9f;

//DANCE mode vars
float maxAmplitude = 0.9f;
float minAmplitude = 0.7f;
float maxIntensity = 1.0f;
float minIntensity = 0.6f;
float calmIntensity = 0.0f;
float minAudioInput = 6.0f;

public:
DancingRGBs(int rPin, int gPin, int bPin);
void readAudio();
void addAmpToCache(float value);
float getMaxFromCache();
float getIntensity();
void dance();
};


double sin_half_sq(long x, unsigned long T, int offset);


   