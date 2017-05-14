#include <DS3231.h>

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

// Init a Time-data structures
Time  currentTime; //time from RTC
Time t; // Time variable for operations
#define NUMBER_OF_WHITE_LED_BARS_ON_RELAY 4 //How many white led bars(connected to relay) to use with PWN white led bat
#define NUMBER_OF_WHITE_LED_BARS 5
#define LED_RELAY_1 2 //Relay on the relay shield with white LED bar
#define LED_RELAY_2 3 //Relay on the relay shield with white LED bar
#define LED_RELAY_3 4 //Relay on the relay shield with white LED bar 
#define LED_RELAY_4 5 //Relay on the relay shield with white LED bar

#define WHITE_LED_PWM 6 //L298N PWM output #1 for white LED bar
#define FAN_PWM 9 //L298N PWM output #2 for Fan 
#define FAN_PWM_LIMIT 150 //Fan speed limit (0-255)
#define BLUE_LED_PWM 10 //L298N PWM output #3 for blue LED bar
#define CIAN_LED_PWM 11 //L298N PWM output #4 for white LED bar

// --- sunrise start ---
#define firstSunriseHour 7 //Hour for sunrise (0->255)
// --- day ---
#define firstDayHour 9 // Hour for day(255) start
// --- sunset start ---
#define firstSunsetHour 18 // Hour for sunset(255->0) start
// --- day pause start ---
#define dayPauseHour 20 // Hour for daypause(0) start
// --- second sunrise start --
#define secondSunriseHour 19 //Hour for sunrise (0->255) start
// --- second day ---
#define secondDayHour 20 // Hour for day(255) start
// --- second sunset start ---
#define secondSunsetHour 20 // Hour for sunset(255->0) start
// ---- Night ----
#define nightHour 21 //Hour for night(0)



int firstSunriseDurationInSeconds;
int firstSunsetDurationInSeconds;
int firstSunriseStepDurationInSeconds;
int firstSunsetStepDurationInSeconds;

int secondSunriseDurationInSeconds;
int secondSunsetDurationInSeconds;
int secondSunriseStepDurationInSeconds;
int secondSunsetStepDurationInSeconds;

int firstOneMoreWhiteLedBarOnPeriodInSeconds;
int firstOneMoreWhiteLedBarOffPeriodInSeconds;

int secondOneMoreWhiteLedBarOnPeriodInSeconds;
int secondOneMoreWhiteLedBarOffPeriodInSeconds;

bool secondBlockEnabled = false;
bool secondDayEnabled = false;


void setup()
{
  // Setup Serial connection
  Serial.begin(115200);

  // Initialize the rtc object
  rtc.begin();
  currentTime = rtc.getTime();

  //  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(SUNDAY);     // Set Day-of-Week to MONDAY
  //rtc.setTime(21, 05, 00);     // Set the time to 21:58:00 (24hr format)
  //rtc.setDate(26, 03, 2017);   // Set the date to 07 March 2016




  // t =  00:00:00 on January 1th 2017 - For timespan culculations
  t.hour = 0;
  t.min = 0;
  t.sec = 0;
  t.year = 2017;
  t.mon = 1;
  t.date = 1;

  firstSunriseDurationInSeconds = (firstDayHour - firstSunriseHour) * 60 * 60;
  firstSunsetDurationInSeconds = (dayPauseHour - firstSunsetHour) * 60 * 60;
  secondSunriseDurationInSeconds = (secondDayHour - secondSunriseHour) * 60 * 60;
  secondSunsetDurationInSeconds = (nightHour - secondSunsetHour) * 60 * 60;

  firstSunriseStepDurationInSeconds = firstSunriseDurationInSeconds / 255;
  firstSunsetStepDurationInSeconds = firstSunsetDurationInSeconds / 255;
  secondSunriseStepDurationInSeconds = secondSunriseDurationInSeconds / 255;
  secondSunsetStepDurationInSeconds = secondSunsetDurationInSeconds / 255;

  firstOneMoreWhiteLedBarOnPeriodInSeconds = firstSunriseDurationInSeconds / (NUMBER_OF_WHITE_LED_BARS); // So after ... seconds we enable one more led bar connected to relay shield
  firstOneMoreWhiteLedBarOffPeriodInSeconds = firstSunsetDurationInSeconds / (NUMBER_OF_WHITE_LED_BARS);
  secondOneMoreWhiteLedBarOnPeriodInSeconds = secondSunriseDurationInSeconds / (NUMBER_OF_WHITE_LED_BARS);
  secondOneMoreWhiteLedBarOffPeriodInSeconds = secondSunsetDurationInSeconds / (NUMBER_OF_WHITE_LED_BARS);


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

  setLightLevel(currentTime);
  Serial.println("---------------delay 5 seconds--------------------- ");
  delay(5000);              // wait for 5 seconds
}

void setLightLevel(Time currentTime) {
  currentTime.year = 2017;
  currentTime.mon = 1;
  currentTime.date = 1;
  //--- sunrise block---
  if (currentTime.hour >= firstSunriseHour && currentTime.hour < firstDayHour) {
    t.hour = firstSunriseHour;
    long time1 = rtc.getUnixTime(t);
    long time2 = rtc.getUnixTime(currentTime);
    int lightLevel = (time2 - time1) / firstSunriseStepDurationInSeconds;
    Serial.print("It is sunrise time now, light level: ");
    Serial.println(lightLevel);
    analogWrite(BLUE_LED_PWM, lightLevel);
    analogWrite(CIAN_LED_PWM, lightLevel);
    long secondsPassedFromSunrise = time2 - time1;
    int numberOfLedBar = (int) secondsPassedFromSunrise / firstOneMoreWhiteLedBarOnPeriodInSeconds;
    whiteLedBarsLightLevel(numberOfLedBar);
    float lightLevelFloat = (secondsPassedFromSunrise % firstOneMoreWhiteLedBarOnPeriodInSeconds) / firstSunriseStepDurationInSeconds;
    lightLevel = (int)lightLevelFloat;
    analogWrite(WHITE_LED_PWM, lightLevel);
  };
  if (currentTime.hour >= firstDayHour && currentTime.hour < firstSunsetHour) {
    t.hour = 0;
    Serial.println("It is day time now, light level: 255");
    analogWrite(BLUE_LED_PWM, 255);
    analogWrite(CIAN_LED_PWM, 255);
    analogWrite(WHITE_LED_PWM, 255);
    whiteLedBarsLightLevel(NUMBER_OF_WHITE_LED_BARS_ON_RELAY);
    // fanSpeed(FAN_PWM_LIMIT);
  };
  if (currentTime.hour >= firstSunsetHour && currentTime.hour < dayPauseHour) {
    t.hour = firstSunsetHour;
    long time1 = rtc.getUnixTime(t);
    long time2 = rtc.getUnixTime(currentTime);
    int lightLevel = 255 - ((time2 - time1) / firstSunsetStepDurationInSeconds);
    Serial.print("It is sunset time now, light level: ");
    Serial.println(lightLevel);
    analogWrite(BLUE_LED_PWM, lightLevel);
    analogWrite(CIAN_LED_PWM, lightLevel);
    long secondsPassedFromSunset = time2 - time1;
    int numberOfLedBarToDisable = (int) secondsPassedFromSunset / firstOneMoreWhiteLedBarOffPeriodInSeconds;
    whiteLedBarsLightLevel(NUMBER_OF_WHITE_LED_BARS_ON_RELAY - numberOfLedBarToDisable);
    lightLevel = (int)(secondsPassedFromSunset % firstOneMoreWhiteLedBarOffPeriodInSeconds) / firstSunsetStepDurationInSeconds;
    Serial.print("lightLevel on WHITE_LED_PWM to deduct from 255: ");
    Serial.println(lightLevel);
    analogWrite(WHITE_LED_PWM, 255 - lightLevel);
  }
  if (currentTime.hour >= dayPauseHour && currentTime.hour < secondSunriseHour) {
    t.hour = 0;
    Serial.println("It is day pause time now, light level: 0");
    analogWrite(BLUE_LED_PWM, 0);
    analogWrite(CIAN_LED_PWM, 0);
    analogWrite(WHITE_LED_PWM, 0);
    whiteLedBarsLightLevel(0);
    // fanSpeed(0);
  };

  //--- second sunrise block---
  if (secondBlockEnabled) {
    if (currentTime.hour >= secondSunriseHour && currentTime.hour < secondDayHour) {
      t.hour = secondSunriseHour;
      long time1 = rtc.getUnixTime(t);
      long time2 = rtc.getUnixTime(currentTime);
      int lightLevel = (time2 - time1) / secondSunriseStepDurationInSeconds;
      Serial.print("It is sunrise time now, light level: ");
      Serial.println(lightLevel);
      analogWrite(BLUE_LED_PWM, lightLevel);
      analogWrite(CIAN_LED_PWM, lightLevel);
      long secondsPassedFromSunrise = time2 - time1;
      int numberOfLedBar = (int) secondsPassedFromSunrise / secondOneMoreWhiteLedBarOnPeriodInSeconds;
      whiteLedBarsLightLevel(numberOfLedBar);
      float lightLevelFloat = (secondsPassedFromSunrise % secondOneMoreWhiteLedBarOnPeriodInSeconds) / secondSunriseStepDurationInSeconds;
      lightLevel = (int)lightLevelFloat;
      analogWrite(WHITE_LED_PWM, lightLevel);
    };
    if (secondDayEnabled) {
      if (currentTime.hour >= secondDayHour && currentTime.hour < secondSunsetHour) {
        t.hour = 0;
        Serial.println("It is day time now, light level: 255");
        analogWrite(BLUE_LED_PWM, 255);
        analogWrite(CIAN_LED_PWM, 255);
        analogWrite(WHITE_LED_PWM, 255);
        whiteLedBarsLightLevel(NUMBER_OF_WHITE_LED_BARS_ON_RELAY);
        //fanSpeed(255);
      };
    }
    if (currentTime.hour >= secondSunsetHour && currentTime.hour < nightHour) {
      t.hour = secondSunsetHour;
      long time1 = rtc.getUnixTime(t);
      long time2 = rtc.getUnixTime(currentTime);
      int lightLevel = 255 - ((time2 - time1) / secondSunsetStepDurationInSeconds);
      Serial.print("It is sunset time now, light level: ");
      Serial.println(lightLevel);
      analogWrite(BLUE_LED_PWM, lightLevel);
      analogWrite(CIAN_LED_PWM, lightLevel);
      long secondsPassedFromSunset = time2 - time1;
      int numberOfLedBarToDisable = (int) secondsPassedFromSunset / secondOneMoreWhiteLedBarOffPeriodInSeconds;
      whiteLedBarsLightLevel(NUMBER_OF_WHITE_LED_BARS_ON_RELAY - numberOfLedBarToDisable);
      lightLevel = (int)(secondsPassedFromSunset % secondOneMoreWhiteLedBarOffPeriodInSeconds) / secondSunsetStepDurationInSeconds;
      Serial.print("lightLevel on WHITE_LED_PWM to deduct from 255: ");
      Serial.println(lightLevel);
      analogWrite(WHITE_LED_PWM, 255 - lightLevel);
      //      fanSpeed(0);
    }
  } else {
    if (currentTime.hour >= dayPauseHour && currentTime.hour < nightHour) {
      noLight();
    }
  }
  // Night befor 24 h || Night after 0 h
  if (currentTime.hour >= nightHour || currentTime.hour < firstSunriseHour) {
    noLight();
  };
};

void noLight() {
  t.hour = 0;
  Serial.println("It is night time now, light level: 0");
  analogWrite(BLUE_LED_PWM, 0);
  analogWrite(CIAN_LED_PWM, 0);
  analogWrite(WHITE_LED_PWM, 0);
  whiteLedBarsLightLevel(0);
  //  fanSpeed(0);
}

void fanSpeed(int lightLevel) {
  if (lightLevel < FAN_PWM_LIMIT) {
    analogWrite(FAN_PWM, lightLevel);
  }
  else {
    analogWrite(FAN_PWM, FAN_PWM_LIMIT);
  }
}

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

