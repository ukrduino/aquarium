// DS3231_Serial_Hard
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// A quick demo of how to use my DS3231-library to
// retrieve time- and date-data for you to manipulate.
//
// To use the hardware I2C (TWI) interface of the Arduino you must connect
// the pins as follows:
//
// Arduino Uno/2009:
// ----------------------
// DS3231:  SDA pin   -> Arduino Analog 4 or the dedicated SDA pin
//          SCL pin   -> Arduino Analog 5 or the dedicated SCL pin



#include <DS3231.h>

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

// Init a Time-data structures
Time  currentTime; //time from RTC
Time t;
#define blueTimeSunriseHour 6 //Hour for sunrise (0->255) start for blue LEDs
#define mainTimeSunriseHour 7 //Hour for sunrise (0->255) start for white blue cian LEDs
#define mainTimeWeekEndSunriseHour 8 //Hour for sunrise (0->255) start for white blue cian LEDs for weekends
#define mainTimeDayHour 10 // Hour for day(255) start for white blue cian LEDs
#define mainTimeWeekEndDayHour 11 // Hour for day(255) start for white blue cian LEDs
#define mainTimeSunsetHour 13 // Hour for day(255->0) start for white blue cian LEDs
#define mainTimeWeekEndSunsetHour 14 // Hour for day(255->0) start for white blue cian LEDs
#define mainTimeNightHour 16 //Hour for night(0) start for white LEDs
#define mainTimeWeekEndNightHour 17 //Hour for night(0) start for white LEDs
#define cianTimeNightHour 23 //Hour for night(0) start for cian LEDs
#define blueTimeNightHour 21 //Hour for night(0) start for blue LEDs

#define mainLightFirstPin 3 // pin for three white led bars SHOULD be PWM
#define mainLightSecondPin 5 // pin for two white led bars + cooler SHOULD be PWM
#define cianLightPin 6 // pin for cian led bar + cooler SHOULD be PWM
#define blueLightPin 9 // pin for blue led bar + cooler SHOULD be PWM

int blueSunriseFadeStepInSeconds;
int mainSunriseFadeStepInSeconds;
int mainSunriseWeekEndFadeStepInSeconds;
int mainSunsetFadeStepInSeconds;
int mainSunsetWeekEndFadeStepInSeconds;
int cianSunsetFadeStepInSeconds = 141;
int blueSunsetFadeStepInSeconds;



void setup()
{
  // Setup Serial connection
  Serial.begin(115200);

  // t =  00:00:00 on January 1th 2014
  t.hour = 0;
  t.min = 0;
  t.sec = 0;
  t.year = 2014;
  t.mon = 1;
  t.date = 1;

  blueSunriseFadeStepInSeconds = (mainTimeDayHour - blueTimeSunriseHour) * 60 * 60 / 255; // Duration of light up step in Seconds (0->1->2...) for cian LEDs
  mainSunriseFadeStepInSeconds = (mainTimeDayHour - mainTimeSunriseHour) * 60 * 60 / 255; // Duration of light up step in Seconds (0->1->2...) for white blue cian LEDs
  mainSunriseWeekEndFadeStepInSeconds = (mainTimeWeekEndDayHour - mainTimeWeekEndSunriseHour) * 60 * 60 / 255; // Duration of light up step in Seconds (0->1->2...) for white blue cian LEDs
  mainSunsetFadeStepInSeconds = (mainTimeNightHour - mainTimeSunsetHour) * 60 * 60 / 255; // Duration of light down step in Seconds (255->254->...) for white LEDs
  mainSunsetWeekEndFadeStepInSeconds = (mainTimeWeekEndNightHour - mainTimeWeekEndSunsetHour) * 60 * 60 / 255; // Duration of light down step in Seconds (255->254->...) for white LEDs
//  cianSunsetFadeStepInSeconds = (cianTimeNightHour - mainTimeSunsetHour) * 60 * 60 / 255; // Duration of light down step in Seconds (255->254->...) for cian LEDs
  blueSunsetFadeStepInSeconds = (blueTimeNightHour - mainTimeSunsetHour) * 60 * 60 / 255; // Duration of light down step in Seconds (255->254->...) for blue LEDs

  Serial.print("blueSunriseFadeStepInSeconds:");
  Serial.println(blueSunriseFadeStepInSeconds);
  Serial.print("mainSunriseFadeStepInSeconds:");
  Serial.println(mainSunriseFadeStepInSeconds);
  Serial.print("mainSunriseWeekEndFadeStepInSeconds:");
  Serial.println(mainSunriseWeekEndFadeStepInSeconds);
  Serial.print("mainSunsetFadeStepInSeconds:");
  Serial.println(mainSunsetFadeStepInSeconds);
  Serial.print("mainSunsetWeekEndFadeStepInSeconds:");
  Serial.println(mainSunsetWeekEndFadeStepInSeconds);
  Serial.print("cianSunsetFadeStepInSeconds:");
  Serial.println(cianSunsetFadeStepInSeconds);
  Serial.print("blueSunsetFadeStepInSeconds:");
  Serial.println(blueSunsetFadeStepInSeconds);

  pinMode(mainLightFirstPin, OUTPUT);
  pinMode(mainLightSecondPin, OUTPUT);
  pinMode(cianLightPin, OUTPUT);
  pinMode(blueLightPin, OUTPUT);
  analogWrite(mainLightFirstPin, 0);
  analogWrite(mainLightSecondPin, 0);


  // Initialize the rtc object
  rtc.begin();

  // The following lines can be uncommented to set the date and time
//  rtc.setDOW(MONDAY);     // Set Day-of-Week to MONDAY
//  rtc.setTime(21, 58, 0);     // Set the time to 21:58:00 (24hr format)
//  rtc.setDate(7, 3, 2016);   // Set the date to 07 March 2016
}

void loop() {
  // Get data from the DS3231
  currentTime = rtc.getTime();
  setLightLevelOnPin(currentTime, mainLightFirstPin);
  setLightLevelOnPin(currentTime, mainLightSecondPin);
  setLightLevelOnPin(currentTime, cianLightPin);
  setLightLevelOnPin(currentTime, blueLightPin);
  delay(1000);              // wait for a second
}

void setLightLevelOnPin(Time currentTime, int pin) {
  int sunrise;
  int day;
  int sunset;
  int night;
  long sunriseFadeIn;
  long sunsetFadeOut;
  Serial.print("Day of week: ");
  Serial.println(currentTime.dow);
  if (pin == mainLightFirstPin || pin == mainLightSecondPin) {
    Serial.print("Main light:");
    Serial.print(pin);
    if (currentTime.dow == 6 || currentTime.dow == 7) {
      Serial.println(" - weekend");
      sunrise = mainTimeWeekEndSunriseHour;
      day = mainTimeWeekEndDayHour;
      sunset = mainTimeWeekEndSunsetHour;
      night = mainTimeWeekEndNightHour;
      sunriseFadeIn = mainSunriseWeekEndFadeStepInSeconds;
      sunsetFadeOut = mainSunsetWeekEndFadeStepInSeconds;
    } else {
      Serial.println(" - working day");
      sunrise = mainTimeSunriseHour;
      day = mainTimeDayHour;
      sunset = mainTimeSunsetHour;
      night = mainTimeNightHour;
      sunriseFadeIn = mainSunriseFadeStepInSeconds;
      sunsetFadeOut = mainSunsetFadeStepInSeconds;
    }
  }
  if (pin == blueLightPin) {
    Serial.print("Blue light:");
    Serial.println(pin);
    sunrise = blueTimeSunriseHour;
    day = mainTimeDayHour;
    sunset = mainTimeSunsetHour;
    night = blueTimeNightHour;
    sunriseFadeIn = blueSunriseFadeStepInSeconds;
    sunsetFadeOut = blueSunsetFadeStepInSeconds;

  }
  if (pin == cianLightPin) {
    Serial.print("Cian light:");
    Serial.println(pin);
    sunrise = mainTimeSunriseHour;
    day = mainTimeDayHour;
    sunset = mainTimeSunsetHour;
    night = cianTimeNightHour;

    sunriseFadeIn = mainSunriseFadeStepInSeconds;
    sunsetFadeOut = cianSunsetFadeStepInSeconds;
  }


  currentTime.year = 2014;
  currentTime.mon = 1;
  currentTime.date = 1;
  if (currentTime.hour >= sunrise && currentTime.hour < day) {
    t.hour = sunrise;
    long time1 = rtc.getUnixTime(t);
    long time2 = rtc.getUnixTime(currentTime);
    Serial.print("------Sunrise time, ");
    int lightLevel = 1;
    for (lightLevel; lightLevel <= 255; lightLevel++) {
      time1 += sunriseFadeIn;
      if (time2 < time1) {
        Serial.print("light level:");
        Serial.println(lightLevel);
        analogWrite(pin, lightLevel);
        break;
      }
    }
  };
  if (currentTime.hour >= day && currentTime.hour < sunset) {
    t.hour = 0;
    Serial.println("------Day time, light level:255");
    analogWrite(pin, 255);
  };
  if (currentTime.hour >= sunset && currentTime.hour < night) {
    t.hour = sunset;
    long time1 = rtc.getUnixTime(t);
    long time2 = rtc.getUnixTime(currentTime);
    Serial.print("------Sunset time, ");
    int lightLevel = 255;
    for (lightLevel; lightLevel > 0; lightLevel--) {
      time1 += sunsetFadeOut;
      if (time2 < time1) {
        Serial.print("light level:");
        Serial.println(lightLevel);
        analogWrite(pin, lightLevel);
        break;
      }
    }
  }
  if (currentTime.hour >= night) {
    t.hour = 0;
    Serial.println("------Night time, light level:0");
    analogWrite(pin, 0);
  };
};



