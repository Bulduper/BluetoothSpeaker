#include <arduinoFFT.h>

#define SAMPLES 32
#define DC_OFFSET 518

class DancingRGBs
{
int r_pin, g_pin, b_pin;
double vReal[SAMPLES];
double vImag[SAMPLES];
arduinoFFT FFT;

public:
DancingRGBs(int rPin, int gPin, int bPin);
void getSpectrum();
void getSamples(double* Re, double* Im, int samp);
void displayAllInConsole();
void dance();
};


   