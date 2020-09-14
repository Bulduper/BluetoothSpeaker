#include <Arduino.h>
#include <DancingRGBs.h>

#define WLED1 8
#define WLED2 11
#define WLED3 10
#define WLED4 12
#define WLED5 7
#define WLED6 4

#define STRIPLED_R 3
#define STRIPLED_G 6
#define STRIPLED_B 5

#define BTN 2
#define PAUSE 9
#define MUTE 13

#define VDIV_EN A4
#define BAT1_VOLTAGE A1
#define BAT12_VOLTAGE A2
#define BAT123_VOLTAGE A3

#define AUDIOIN A0

#define VCC 5.063f
#define LONG_PRESS_MS 2000

float bat1_voltage, bat2_voltage, bat3_voltage;
float bat_voltage_avg;
float batteryThresholds[6]={3.4f, 3.6f, 3.7f, 3.75f, 3.85f, 3.95f};
int currentBatteryLvl;

bool isPaused;
bool isMuted;

unsigned long lastInterruptPressed, lastInterruptReleased;
unsigned long lastLoop_10s;

DancingRGBs dancingRGBs(STRIPLED_R,STRIPLED_G, STRIPLED_B);

void buttonInterrupt();
void Loop_10s();
void measureVoltage();
void updateBatteryLvl();
void displayBatteryLvl();
void whiteLeds(byte Byte);

void setup() {
  noInterrupts();
  Serial.begin(9600);
  pinMode(BTN,INPUT_PULLUP);
  pinMode(PAUSE,OUTPUT);
  pinMode(MUTE,OUTPUT);
  pinMode(VDIV_EN,OUTPUT);
  pinMode(STRIPLED_R,OUTPUT);
  pinMode(STRIPLED_G,OUTPUT);
  pinMode(STRIPLED_B,OUTPUT);

  digitalWrite(PAUSE,LOW);
  digitalWrite(MUTE, HIGH);
  digitalWrite(WLED1, HIGH);
  digitalWrite(WLED2, HIGH);
  digitalWrite(WLED3, HIGH);
  digitalWrite(WLED4, HIGH);
  digitalWrite(WLED5, HIGH);
  digitalWrite(WLED6, HIGH);

  attachInterrupt(digitalPinToInterrupt(BTN),buttonInterrupt,CHANGE);
  delay(1000);
  interrupts();
}

void loop() {
  //Loop_10s();
  //dancingRGBs.getSpectrum();
  dancingRGBs.readAudio();
  dancingRGBs.dance();
  //dancingRGBs.displayAllInConsole();
  //dancingRGBs.displayIndexInConsole(1);


  //delay(1000);
}

void Loop_10s()
{
  if(millis()-lastLoop_10s>10000)
  {
    measureVoltage();
    updateBatteryLvl();
    displayBatteryLvl();
    /*Serial.print(bat1_voltage);
    Serial.print("\t");
    Serial.print(bat2_voltage);
    Serial.print("\t");
    Serial.print(bat3_voltage);
    Serial.print("\n");
    */lastLoop_10s = millis();
  }

}

void measureVoltage()
{
  float bat1_avg, bat12_avg, bat123_avg;
  float analogToVolts = 1023.0F/VCC;
  bat1_avg=0;
  bat12_avg=0;
  bat123_avg=0;

  //enable the voltage divider
  digitalWrite(VDIV_EN,HIGH);
  //delay(1);

  ADCSRA = 0b11100101;      // set ADC to free running mode and set pre-scalar to 32 (0xe5)
  ADMUX = 0b01000000 | 1;       // use pin A1 and Vcc voltage reference

  for(int i = 0; i<10; i++)
  {
    while(!(ADCSRA & 0x10));        // wait for ADC to complete current conversion ie ADIF bit set
    ADCSRA = 0b11110101 ;               // clear ADIF bit so that ADC can do next operation (0xf5)
    bat1_avg+=ADC;
    
    ADMUX |= 2;
    while(!(ADCSRA & 0x10));        // wait for ADC to complete current conversion ie ADIF bit set
    ADCSRA = 0b11110101 ;               // clear ADIF bit so that ADC can do next operation (0xf5)
    bat12_avg+=ADC;
    
    ADMUX |= 3;
    while(!(ADCSRA & 0x10));        // wait for ADC to complete current conversion ie ADIF bit set
    ADCSRA = 0b11110101 ;               // clear ADIF bit so that ADC can do next operation (0xf5)
    bat123_avg+=ADC;
  }
  bat1_avg/=10.f;
  bat12_avg/=10.f;
  bat123_avg/=10.f;

  float bat1_v = bat1_avg/analogToVolts;
  float bat12_v = 2*bat12_avg/analogToVolts;
  float bat123_v = 3*bat123_avg/analogToVolts;

  //digitalWrite(VDIV_EN,LOW);

  bat1_voltage = bat1_v;
  bat2_voltage = bat12_v - bat1_v;
  bat3_voltage = bat123_v - bat12_v;
  bat_voltage_avg = (bat1_voltage+bat2_voltage+bat3_voltage)/3.0f;
}

void updateBatteryLvl()
{
  const int deadBand = 0.03f;
  int i=0;
  while(bat_voltage_avg>batteryThresholds[i])
  {
    i++;
    if(i>=6)break;
  }
  
  //histeresis
  if(currentBatteryLvl<i)
  {
    if(bat_voltage_avg>batteryThresholds[i-1]+deadBand)currentBatteryLvl = i;
  }
  else if(currentBatteryLvl>i)
  {
    if(bat_voltage_avg < batteryThresholds[i] - deadBand)currentBatteryLvl = i;
  }

}

void displayBatteryLvl()
{
  whiteLeds((1<<currentBatteryLvl)-1);
}

void whiteLeds(byte Byte)
{
  int ledNo = 0;
  while(ledNo<6)
  {
    switch (ledNo)
    {
    case 0:
      digitalWrite(WLED1,Byte%2);
      break;
    case 1:
      digitalWrite(WLED2,Byte%2);
      break;
    case 2:
      digitalWrite(WLED3,Byte%2);
      break;
    case 3:
      digitalWrite(WLED4,Byte%2);
      break;
    case 4:
      digitalWrite(WLED5,Byte%2);
      break;
    case 5:
      digitalWrite(WLED6,Byte%2);
      break;
    default:
      break;
    }
    Byte/=2;
    ledNo++;
  }
}

void buttonInterrupt()
{
  //debouncing
  unsigned long now = micros(); 
  if(now-lastInterruptReleased > 400000L && now-lastInterruptPressed>400000L)
  {
    if(digitalRead(BTN)==HIGH)
    {
      lastInterruptPressed = now;
    }

    else if(digitalRead(BTN)==LOW)
    {
      lastInterruptReleased = now;

      //short press
      if(lastInterruptReleased - lastInterruptPressed < LONG_PRESS_MS)
      {
        isPaused = !isPaused;
      }
      //long press
      else
      {
        //rgb on/off
      }
      
    }
  }
}