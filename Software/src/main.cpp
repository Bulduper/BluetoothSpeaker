#include <Arduino.h>
#include <DancingRGBs.h>
#include <OneButton.h>

#define WLED1 8
#define WLED2 9
#define WLED3 10
#define WLED4 11
#define WLED5 12
#define WLED6 13
#define GLED 7

#define STRIPLED_R 6
#define STRIPLED_G 5
#define STRIPLED_B 3

#define BTN A3
#define PAUSE 4
#define MUTE A2
#define BT_INFO 2             //there is a problem, it does not work due to strange voltage lvls on CSR LED

#define BAT_VOLTAGE A1

#define AUDIOIN A0

#define VCC 5.000f
#define LONG_PRESS_MS 500
#define BAT_VOL_OFFSET 0.163   //this value must be added to the battery voltage, it was obtained experimentally  

float bat_voltage;
float batteryThresholds[6]={3.4f, 3.6f, 3.7f, 3.75f, 3.85f, 3.95f};
int currentBatteryLvl;

bool batteryOK = true;
bool btnIsPressed;

unsigned long lastInterruptPressed, lastInterruptReleased;
unsigned long lastLoop_30s = -30000;

DancingRGBs dancingRGBs(STRIPLED_R,STRIPLED_G, STRIPLED_B);
OneButton button = OneButton(BTN);

void initialSequence();
void errorBlink();
void Loop_30s();
void measureVoltage();
void updateBatteryLvl();
void displayBatteryLvl();
void pauseResume();
void longPressResult();
void whiteLeds(byte Byte);

void setup() {

  Serial.begin(9600);
  pinMode(BTN,INPUT_PULLUP);
  pinMode(BT_INFO,INPUT);
  pinMode(PAUSE,OUTPUT);
  pinMode(MUTE,OUTPUT);
  pinMode(GLED,OUTPUT);
  pinMode(STRIPLED_R,OUTPUT);
  pinMode(STRIPLED_G,OUTPUT);
  pinMode(STRIPLED_B,OUTPUT);

  digitalWrite(PAUSE,LOW);
  digitalWrite(MUTE, HIGH);
  digitalWrite(GLED,HIGH);

  button.attachClick(pauseResume);
  button.attachLongPressStop(longPressResult);
  button.setPressTicks(LONG_PRESS_MS);
  delay(500);
  initialSequence();
}

void loop() {
  Loop_30s();
  button.tick();
  if(batteryOK)dancingRGBs.loop();
  else
  {
    digitalWrite(MUTE,LOW);
    errorBlink();
  }

  //Serial.println(analogRead(AUDIOIN));
}

void initialSequence()
{
  whiteLeds(0);
  delay(200);
  whiteLeds(0b001100);
  delay(200);
  whiteLeds(0b010010);
  delay(200);
  whiteLeds(0b100001);
  delay(200);
  whiteLeds(0b110011);
  delay(200);
  whiteLeds(0b111111);
  delay(200);
}

void errorBlink()
{
  whiteLeds(0);
  digitalWrite(GLED,LOW);
  delay(300);
  whiteLeds(0b111111);
  digitalWrite(GLED,HIGH);
  delay(300);
}


void Loop_30s()
{
  if(millis()-lastLoop_30s>30000)
  {
    measureVoltage();
    updateBatteryLvl();
    displayBatteryLvl();
    /*Serial.print(bat_voltage);
    Serial.print("\n");*/
    lastLoop_30s = millis();
  }

}

void measureVoltage()
{
  float bat_avg;
  float analogToVolts = 1023.0F/VCC;
  bat_avg=0;



  ADMUX = 0b01000000 | 1;       // use pin A1 and Vcc voltage reference
  ADCSRA = 0b11010101;      // set ADC

  while(!(ADCSRA & 0x10));        // wait for ADC to complete current conversion ie ADIF bit set
  ADCSRA = 0b11010101 ;               // clear ADIF bit so that ADC can do next operation (0xf5)
  for(int i = 0; i<10; i++)
  {
    
    while(!(ADCSRA & 0x10));        // wait for ADC to complete current conversion ie ADIF bit set
    bat_avg+=ADC;
    //Serial.println(ADC);
    ADCSRA = 0b11010101 ;               // clear ADIF bit so that ADC can do next operation (0xf5)

  }
  while(!(ADCSRA & 0x10));        // wait for ADC to complete current conversion ie ADIF bit set
  ADMUX = 0b00000000;       // use pin A0 (update for audio input readings) and external voltage reference - A PART OF THE SOLUTION BELOW
  

  //////  THIS IS A SOLUTION TO THE PEAKS READ BY ADC ON A0 AFTER ADMUX UPDATE
  //////  1000 READINGS TAKE APPROX. 28ms AND I DON'T LIKE IT, BUT IT WAS THE ONLY APPROACH THAT WORKED FOR ME :/ 
  for(int i= 0; i<1000; i++)
  {
  ADCSRA = 0b11010101;      // set ADC
  while(!(ADCSRA & 0x10));        // wait for ADC to complete current conversion ie ADIF bit set
  }

  bat_avg/=10.f;


  float bat1_v = bat_avg/analogToVolts;

  bat_voltage = bat1_v+BAT_VOL_OFFSET;
}

void updateBatteryLvl()
{
  const int deadBand = 0.02f;
  int i=0;

  if(bat_voltage<=3.3f)
  {
    batteryOK = false;

    return;
  }
  batteryOK = true;
  digitalWrite(MUTE,HIGH);

  while(bat_voltage>batteryThresholds[i])
  {
    i++;
    if(i>=6)break;
  }
  
  //histeresis
  if(currentBatteryLvl<i)
  {
    if(bat_voltage>batteryThresholds[i-1]+deadBand)currentBatteryLvl = i;
  }
  else if(currentBatteryLvl>i)
  {
    if(bat_voltage < batteryThresholds[i] - deadBand)currentBatteryLvl = i;
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

void pauseResume()
{
  digitalWrite(PAUSE,HIGH);
  delay(100);
  digitalWrite(PAUSE,LOW);
}

void longPressResult()
{
  dancingRGBs.nextMode();
}

