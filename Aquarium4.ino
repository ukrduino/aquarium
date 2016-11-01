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
Time t; // Time variable for operations
#define NUMBER_OF_WHITE_LED_BARS 2 //How many white led bars(connected to relay) to use with PWN white led bat

#define LED_RELAY_1 2 //Relay on the relay shield with white LED bar
#define LED_RELAY_2 3 //Relay on the relay shield with white LED bar
#define LED_RELAY_3 4 //Relay on the relay shield with white LED bar 
#define LED_RELAY_4 5 //Relay on the relay shield with white LED bar

#define WHITE_LED_PWM 6 //L298N PWM output #1 for white LED bar
#define FAN_PWM 9 //L298N PWM output #2 for Fan
#define BLUE_LED_PWM 10 //L298N PWM output #3 for blue LED bar
#define CIAN_LED_PWM 11 //L298N PWM output #4 for white LED bar

// --- sunrise start ---
int blueCianSunriseHour; //Hour for sunrise (0->255) start for blue and cian LEDs
int whiteSunriseHour; //Hour for sunrise (0->255) start for white LEDs
// --- day ---
#define whiteDayHour 12 // Hour for day(255) start for white blue cian LEDs
// --- sunset start ---
#define whiteSunsetHour 15 // Hour for day(255->0) start for white LEDs
#define blueSunsetHour 15 // Hour for day(255->0) start for blue LEDs
#define cianSunsetHour 15 // Hour for day(255->0) start for cian LEDs
// ---- Night ----
#define whiteNightHour 21 //Hour for night(0) start for white LEDs
#define blueNightHour 21 //Hour for night(0) start for blue LEDs
#define cianNightHour 21 //Hour for night(0) start for cian LEDs

int blueCianSunriseStepDurationInSeconds;
int whiteSunriseDurationInSeconds;
int oneMoreWhiteLedBarOnPeriodInSeconds;
float whiteSunriseStepDurationInSeconds;
int whiteSunsetDurationInSeconds;
int oneMoreWhiteLedBarOffPeriodInSeconds;
float whiteSunsetStepDurationInSeconds;
int cianSunsetStepDurationInSeconds;
int blueSunsetStepDurationInSeconds;

int sunrise;
int day;
int sunset;
int night;
int sunriseFadeIn;
int sunsetFadeOut;



void setup()
{
    // Setup Serial connection
  Serial.begin(115200);
  
  // Initialize the rtc object
  rtc.begin();
  currentTime = rtc.getTime();

  //Set sunrise time
  blueCianSunriseHour = 7; //Hour for sunrise (0->255) start for blue and cian LEDs - on monday - friday
  whiteSunriseHour = 7; //Hour for sunrise (0->255) start for white LEDs - on monday - friday
  Serial.print("Day of week: ");
  Serial.println(currentTime.dow);
  if (currentTime.dow == 6 || currentTime.dow == 7) {
    blueCianSunriseHour += 1; //Hour for sunrise (0->255) start for blue and cian LEDs - on saturday - sunday
    whiteSunriseHour = + 1; //Hour for sunrise (0->255) start for white LEDs - on saturday - sunday
  }

  //  // The following lines can be uncommented to set the date and time
//      rtc.setDOW(TUESDAY);     // Set Day-of-Week to MONDAY
//      rtc.setTime(10, 03, 00);     // Set the time to 21:58:00 (24hr format)
//      rtc.setDate(1, 11, 2016);   // Set the date to 07 March 2016




  // t =  00:00:00 on January 1th 2016 - For timespan culculations
  t.hour = 0;
  t.min = 0;
  t.sec = 0;
  t.year = 2016;
  t.mon = 1;
  t.date = 1;

  blueCianSunriseStepDurationInSeconds = (whiteDayHour - blueCianSunriseHour) * 60 * 60 / 255; // Duration of light up step in Seconds (0->1->2->...) for cian and blue LEDs
  whiteSunriseDurationInSeconds = (whiteDayHour - whiteSunriseHour) * 60 * 60;
  oneMoreWhiteLedBarOnPeriodInSeconds = whiteSunriseDurationInSeconds / (NUMBER_OF_WHITE_LED_BARS + 1); // So after ... seconds we enable one more led bar connected to relay shield
  whiteSunriseStepDurationInSeconds = oneMoreWhiteLedBarOnPeriodInSeconds / 255.00; // Duration of light up step in Seconds (0->1->2->...) for white LEDs bar
  whiteSunsetDurationInSeconds = (whiteNightHour - whiteSunsetHour) * 60 * 60;
  oneMoreWhiteLedBarOffPeriodInSeconds = whiteSunsetDurationInSeconds / (NUMBER_OF_WHITE_LED_BARS + 1); // So after ... seconds we disable one more led bar connected to relay shield
  whiteSunsetStepDurationInSeconds = oneMoreWhiteLedBarOffPeriodInSeconds / 255.00; // Duration of light down step in Seconds (255->254->...) for white LEDs
  cianSunsetStepDurationInSeconds = (cianNightHour - cianSunsetHour) * 60 * 60 / 255; // Duration of light down step in Seconds (255->254->...) for cian LEDs
  blueSunsetStepDurationInSeconds = (blueNightHour - blueSunsetHour) * 60 * 60 / 255; // Duration of light down step in Seconds (255->254->...) for blue LEDs

  Serial.print("blueCianSunriseStepDurationInSeconds:");
  Serial.println(blueCianSunriseStepDurationInSeconds);
  Serial.print("cianSunsetStepDurationInSeconds:");
  Serial.println(cianSunsetStepDurationInSeconds);
  Serial.print("blueSunsetStepDurationInSeconds:");
  Serial.println(blueSunsetStepDurationInSeconds);


  Serial.print("whiteSunriseDurationInSeconds:");
  Serial.println(whiteSunriseDurationInSeconds);
  Serial.print("oneMoreWhiteLedBarOnPeriodInSeconds:");
  Serial.println(oneMoreWhiteLedBarOnPeriodInSeconds);
  Serial.print("whiteSunriseStepDurationInSeconds:");
  Serial.println(whiteSunriseStepDurationInSeconds);

  Serial.print("whiteSunsetDurationInSeconds:");
  Serial.println(whiteSunsetDurationInSeconds);
  Serial.print("oneMoreWhiteLedBarOffPeriodInSeconds:");
  Serial.println(oneMoreWhiteLedBarOffPeriodInSeconds);
  Serial.print("whiteSunsetStepDurationInSeconds:");
  Serial.println(whiteSunsetStepDurationInSeconds);

  pinMode(LED_RELAY_1, OUTPUT);
  pinMode(LED_RELAY_2, OUTPUT);
  pinMode(LED_RELAY_3, OUTPUT);
  pinMode(LED_RELAY_4, OUTPUT);
  pinMode(WHITE_LED_PWM, OUTPUT);
  pinMode(FAN_PWM, OUTPUT);
  pinMode(BLUE_LED_PWM, OUTPUT);
  pinMode(CIAN_LED_PWM, OUTPUT);
  analogWrite(WHITE_LED_PWM, 0);
  digitalWrite(LED_RELAY_1, HIGH);
  digitalWrite(LED_RELAY_2, HIGH);
  digitalWrite(LED_RELAY_3, HIGH);
  digitalWrite(LED_RELAY_4, HIGH);


}

void loop() {
  // Get data from the DS3231
  currentTime = rtc.getTime();
  Serial.print(rtc.getTimeStr());
  Serial.print(" ");
  Serial.print(rtc.getDateStr());
  Serial.print(" ");

  setLightLevelOnPin(currentTime, BLUE_LED_PWM);
  setLightLevelOnPin(currentTime, CIAN_LED_PWM);
  setLightLevelOnWhitePin(currentTime);
  Serial.println("---------------delay 5 seconds--------------------- ");
  delay(5000);              // wait for 5 seconds
}

void setLightLevelOnPin(Time currentTime, int pin) {
  if (pin == BLUE_LED_PWM) {
    sunrise = blueCianSunriseHour;
    day = whiteDayHour;
    sunset = blueSunsetHour;
    night = blueNightHour;
    sunriseFadeIn = blueCianSunriseStepDurationInSeconds;
    sunsetFadeOut = blueSunsetStepDurationInSeconds;
  }
  if (pin == CIAN_LED_PWM) {
    sunrise = blueCianSunriseHour;
    day = whiteDayHour;
    sunset = cianSunsetHour;
    night = cianNightHour;
    sunriseFadeIn = blueCianSunriseStepDurationInSeconds;
    sunsetFadeOut = cianSunsetStepDurationInSeconds;
  }

  currentTime.year = 2016;
  currentTime.mon = 1;
  currentTime.date = 1;
  //--- sunrire block---
  if (currentTime.hour >= sunrise && currentTime.hour < day) {
    t.hour = sunrise;
    long time1 = rtc.getUnixTime(t);
    long time2 = rtc.getUnixTime(currentTime);
    Serial.print("---Sunrise time for pin: ");
    Serial.print(pin);
    int lightLevel = (time2 - time1) / sunriseFadeIn;
    Serial.print(", light level: ");
    Serial.println(lightLevel);
    analogWrite(pin, lightLevel);
  };
  if (currentTime.hour >= day && currentTime.hour < sunset) {
    t.hour = 0;
    Serial.print("---Day time for pin: ");
    Serial.print(pin);
    Serial.println(", light level: 255");
    analogWrite(pin, 255);
  };
  if (currentTime.hour >= sunset && currentTime.hour < night) {
    t.hour = sunset;
    long time1 = rtc.getUnixTime(t);
    long time2 = rtc.getUnixTime(currentTime);
    Serial.print("---Sunset time for pin: ");
    Serial.print(pin);
    int lightLevel = 255 - ((time2 - time1) / sunriseFadeIn);
    Serial.print(", light level: ");
    Serial.println(lightLevel);
    analogWrite(pin, lightLevel);
  }
  if (currentTime.hour >= night) {
    t.hour = 0;
    Serial.print("---Night time on pin: ");
    Serial.print(pin);
    Serial.println(", light level: 0");
    analogWrite(pin, 0);
  };
};


void setLightLevelOnWhitePin(Time currentTime) {
  currentTime.year = 2016;
  currentTime.mon = 1;
  currentTime.date = 1;
  sunrise = whiteSunriseHour;
  day = whiteDayHour;
  sunset = whiteSunsetHour;
  night = whiteNightHour;
  //--- sunrise block---
  if (currentTime.hour >= sunrise && currentTime.hour < day) {
    t.hour = sunrise;
    long time1 = rtc.getUnixTime(t);
    long time2 = rtc.getUnixTime(currentTime);
    Serial.println("---Sunrise time on WHITE_LED_PWM: ");
    long secondsPassedFromSunrise = time2 - time1;
    Serial.print("secondsPassedFromSunrise: ");
    Serial.println(secondsPassedFromSunrise);
    int numberOfLedBar = (int) secondsPassedFromSunrise / oneMoreWhiteLedBarOnPeriodInSeconds;
    Serial.print("numberOfLedBar: ");
    Serial.println(numberOfLedBar);
    whiteLedBarsLightLevel(numberOfLedBar);
    float lightLevelFloat = (secondsPassedFromSunrise % oneMoreWhiteLedBarOnPeriodInSeconds) / whiteSunriseStepDurationInSeconds;
    int lightLevel = (int)lightLevelFloat;
    Serial.print("lightLevel on WHITE_LED_PWM: ");
    Serial.println(lightLevel);
    Serial.print("lightLevelFloat on WHITE_LED_PWM: ");
    Serial.println(lightLevelFloat);
    analogWrite(WHITE_LED_PWM, lightLevel);
  };

  //--- sunset block---
  if (currentTime.hour >= sunset && currentTime.hour < night) {
    t.hour = sunset;
    long time1 = rtc.getUnixTime(t);
    long time2 = rtc.getUnixTime(currentTime);
    Serial.println("---Sunset time on WHITE_LED_PWM: ");
    long secondsPassedFromSunset = time2 - time1;
    Serial.print("secondsPassedFromSunset: ");
    Serial.println(secondsPassedFromSunset);
    int numberOfLedBarToDisable = (int) secondsPassedFromSunset / oneMoreWhiteLedBarOffPeriodInSeconds;
    Serial.print("numberOfLedBar to disable: ");
    Serial.println(numberOfLedBarToDisable);
    whiteLedBarsLightLevel(NUMBER_OF_WHITE_LED_BARS - numberOfLedBarToDisable);
    int lightLevel = (int)(secondsPassedFromSunset % oneMoreWhiteLedBarOffPeriodInSeconds) / whiteSunsetStepDurationInSeconds;
    Serial.print("lightLevel on WHITE_LED_PWM to deduct from 255: ");
    Serial.println(lightLevel);
    analogWrite(WHITE_LED_PWM, 255 - lightLevel);
  };

  if (currentTime.hour >= day && currentTime.hour < sunset) {
    t.hour = 0;
    Serial.print("---Day time on WHITE_LED_PWM");
    Serial.println(", light level on pin: 255");
    analogWrite(WHITE_LED_PWM, 255);
    whiteLedBarsLightLevel(NUMBER_OF_WHITE_LED_BARS);
  };
  if (currentTime.hour >= night) {
    t.hour = 0;
    Serial.print("---Night time on WHITE_LED_PWM");
    Serial.println(", light level on pin: 0");
    analogWrite(WHITE_LED_PWM, 0);
    whiteLedBarsLightLevel(0);
  };
};

void whiteLedBarsLightLevel(int ledBarsNumber) {
  switch (ledBarsNumber) {
    case 0:
      digitalWrite(LED_RELAY_1, HIGH);
      digitalWrite(LED_RELAY_2, HIGH);
      digitalWrite(LED_RELAY_3, HIGH);
      digitalWrite(LED_RELAY_4, HIGH);
      break;
    case 1:
      digitalWrite(LED_RELAY_1, LOW);
      digitalWrite(LED_RELAY_2, HIGH);
      digitalWrite(LED_RELAY_3, HIGH);
      digitalWrite(LED_RELAY_4, HIGH);
      break;
    case 2:
      digitalWrite(LED_RELAY_1, LOW);
      digitalWrite(LED_RELAY_2, LOW);
      digitalWrite(LED_RELAY_3, HIGH);
      digitalWrite(LED_RELAY_4, HIGH);
      break;
    case 3:
      digitalWrite(LED_RELAY_1, LOW);
      digitalWrite(LED_RELAY_2, LOW);
      digitalWrite(LED_RELAY_3, LOW);
      digitalWrite(LED_RELAY_4, HIGH);
      break;
    case 4:
      digitalWrite(LED_RELAY_1, LOW);
      digitalWrite(LED_RELAY_2, LOW);
      digitalWrite(LED_RELAY_3, LOW);
      digitalWrite(LED_RELAY_4, LOW);
      break;
    default:
      digitalWrite(LED_RELAY_1, HIGH);
      digitalWrite(LED_RELAY_2, HIGH);
      digitalWrite(LED_RELAY_3, HIGH);
      digitalWrite(LED_RELAY_4, HIGH);
      break;
  }
};



