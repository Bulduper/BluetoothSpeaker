#include <Arduino.h>

#define WLED1 8
#define WLED2 11
#define WLED3 10
#define WLED4 12
#define WLED5 13
#define WLED6 4

#define STRIPLED_R 5
#define STRIPLED_G 6
#define STRIPLED_B 3

#define BTN 2
#define PAUSE 9
#define MUTE 7

#define VDIV_EN A4
#define BAT1_VOLTAGE A1
#define BAT12_VOLTAGE A2
#define BAT123_VOLTAGE A3

#define AUDIOIN A0

#define VCC 5.063f

float bat1_voltage, bat2_voltage, bat3_voltage;
float bat_voltage_avg;
float batteryThresholds[6]={3.4f, 3.6f, 3.7f, 3.75f, 3.85f, 3.95f};
int currentBatteryLvl;

bool isPaused;
bool isMuted;

void buttonInterrupt();
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
  // put your main code here, to run repeatedly:
  measureVoltage();
  updateBatteryLvl();
  displayBatteryLvl();
  Serial.print(bat1_voltage);
  Serial.print("\t");
  Serial.print(bat2_voltage);
  Serial.print("\t");
  Serial.print(bat3_voltage);
  Serial.print("\n");
  delay(5000);
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
  delay(3);

  analogReference(DEFAULT);

  for(int i = 0; i<10; i++)
  {
    bat1_avg+=analogRead(BAT1_VOLTAGE);
    bat12_avg+=analogRead(BAT12_VOLTAGE);
    bat123_avg+=analogRead(BAT123_VOLTAGE);
    delay(1);
  }
  bat1_avg/=10.f;
  bat12_avg/=10.f;
  bat123_avg/=10.f;

  float bat1_v = bat1_avg/analogToVolts;
  float bat12_v = 2*bat12_avg/analogToVolts;
  float bat123_v = 3*bat123_avg/analogToVolts;

  digitalWrite(VDIV_EN,LOW);

  bat1_voltage = bat1_v;
  bat2_voltage = bat12_v - bat1_v;
  bat3_voltage = bat123_v - bat12_v;
  bat_voltage_avg = (bat1_voltage+bat2_voltage+bat3_voltage)/3.0f;
}

void updateBatteryLvl()
{
  const int deadBand = 0.02f;
  int i=0;
  while(bat_voltage_avg>batteryThresholds[i])
  {
    i++;
    if(i>=6)break;
  }
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
  Serial.println(currentBatteryLvl);

}

void whiteLeds(byte Byte)
{
  int ledNo = 0;
  Serial.println(Byte);
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
  isPaused = !isPaused;
  digitalWrite(PAUSE,isPaused);
  Serial.println("INT");
}