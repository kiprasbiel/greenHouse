/* Real time library */
#include <DS3231.h>
DS3231  rtc(SDA, SCL);
Time t;

/* Disables time checking */
const int testingMode = 1;

const int wateringRangeBot = 70;
const int wateringRangeTop = 80;
const int paklaida = 5;

/* Kiek laiko uztrunka vienas laistymo ciklas (min) */
const int wateringInterval = 1;
/* Kiek laiko laukti po laistymo, kada gali buti vel laistoma */
const int wateringPauseTime = 5;

int isTimeSet = 0;

const int pureWater = 275;
const int pureAir = 611;

const int tempLow = 0;
const int tempHigh = 40;

const int water_sensor = A0;
const int LedGreen = 12;
const int LedRed = 13;
const int pump = 8;

const int OnHour = 6;
const int OnMinute = 0;
const int OffHour = 19;
const int OffMinute = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(LedRed, OUTPUT);
  pinMode(LedGreen, OUTPUT);
  pinMode(pump, OUTPUT);

  /* Initializing time object */
  rtc.begin();

}

void loop()
{
  tempretureCheck();
  t = rtc.getTime();
  /* Sausa */
  if(t.hour >= OnHour && t.hour <= OffHour || testingMode == 1){
    ReadyToWater();
  }
  else {
    NotWateringTime();
  }
  
}

void ReadyToWater(){
  int moist_proc = waterSensor();
  Serial.println(moist_proc);
  if(moist_proc < wateringRangeBot){
    startWatering();
  }
  /* Dregna */
  else if(moist_proc > wateringRangeBot && moist_proc <= wateringRangeTop + paklaida){
    digitalWrite(LedGreen, HIGH);
    digitalWrite(LedRed, LOW);
    digitalWrite(pump, LOW);
    delay(1000);
  }
  else if(moist_proc > wateringRangeTop + paklaida){
    digitalWrite(LedGreen, LOW);
    digitalWrite(pump, LOW);
    for (int i = 0; i <= 1; i++){
      digitalWrite(LedRed, HIGH);
      delay(50);
      digitalWrite(LedRed, LOW);
      delay(50);
    }
    delay(2000);
  }
  
}

void NotWateringTime(){
  int waterProc = waterSensor();
  Serial.println(waterProc);
  digitalWrite(pump, LOW);
  if(waterProc <= wateringRangeTop && waterProc >= wateringRangeBot){
    digitalWrite(LedRed, LOW);
    digitalWrite(LedGreen, HIGH);
    delay(50);
    digitalWrite(LedGreen, LOW);
    delay(5000);
  }
  else{
    digitalWrite(LedGreen, LOW);
    digitalWrite(LedRed, HIGH);
    delay(50);
    digitalWrite(LedRed, LOW);
    delay(5000);
  }
  
  
}
int waterSensor(){

  int moist_val = analogRead(water_sensor);
  int moist_proc = map(moist_val, pureAir, pureWater, 0, 100);
  
  return moist_proc;
}

void tempretureCheck(){
  float temp = rtc.getTemp();
  if(temp > tempHigh || temp < tempLow){
    Serial.print("Currant temp is: ");
    Serial.println(temp);
    for (int i = 0; i <= 6; i++){
      digitalWrite(LedRed, HIGH);
      delay(50);
      digitalWrite(LedRed, LOW);
      delay(50);
    }
  }
}

unsigned long mTime;

void startWatering(){
  long currantTime = millis();
  digitalWrite(LedGreen, LOW);
  digitalWrite(LedRed, HIGH);
  if(isTimeSet == 0){
    mTime = millis();
    isTimeSet = 1;
  }
  /* Open valve for wateringInterval min and after that wait for wateringPauseTime min */
  long timeDiff = currantTime - mTime;
  Serial.print("Time passed: ");
  Serial.println(timeDiff);
  if(timeDiff < wateringInterval * 60000){
    digitalWrite(pump, HIGH);
  }
  /* Watering Pause */
  else if(timeDiff > wateringInterval * 60000 && timeDiff < wateringPauseTime * 60000){
    digitalWrite(pump, LOW);
  }
  /* Pause expired */
  else if(timeDiff > wateringPauseTime * 60000){
    isTimeSet = 0;
  }
  delay(100);
}
