/*
 * Green house watering system V2
 */
#include <Metro.h>

/* Real time library */
#include <DS3232RTC.h>
#include <TimeLib.h>
#include <Wire.h>

/* LCD */
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

/* LCD Back light */
const int OFF = 0;
const int ON = 1;
int backlight_state = ON;
bool button_released = true; // Ar po paspaudimo buvo atleistas mygtukas
unsigned long btn_last_press = 0; // Tracks time since last button press
bool menu_open = false;
int menu_num = 0;

/* Predefined Buttons */
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

/* Pins */
const int water_sensor = A2;
const int pump = 2;
const int BACKLIGHT_PIN = 10;
const int buttons = 0;

bool isItNight = false;  // Keeps track if it's night. True for redundancy
bool pumpOn = false;

/* Constant values */
const int pureWater = 260;
const int drySoil = 597;

/* Errors variables */
bool error = false;
bool errorLedsState = false;

/* Editable default variables */
int OnHour = 6;
int OffHour = 24;
int LowBar = 70;
int HighBar = 80;

const int LCDTimeOut = 10; // LCD time out in SECONDS

String menuItems[] = {"Dregna nuo:", "Sausa nuo:", "Laistoma nuo (h):", "Laistoma iki (h):"};

Metro VandensMetro = Metro(1000);
Metro TimeCheckMetro = Metro(1000);
Metro ErrorCheckMetro = Metro(500);

void setup() {
  lcd.begin(16, 2);

  Serial.begin(9600);
    setSyncProvider(RTC.get);
    if (timeStatus() != timeSet)
        Serial.println("Unable to sync with the RTC");
    else
        Serial.println("RTC has set the system time");
        
  pinMode(pump, OUTPUT);

  digitalWrite(BACKLIGHT_PIN, LOW);

  checkDayTime();
}

void backLightControll(String mode = "toggle"){
  if(mode == "toggle"){
    if (backlight_state == ON) {
      pinMode(BACKLIGHT_PIN,OUTPUT);
      backlight_state = OFF;
    }
    else if (backlight_state == OFF) {
      pinMode(BACKLIGHT_PIN,INPUT);
      backlight_state = ON;
    }
  }
  else if(mode == "OFF"){
    pinMode(BACKLIGHT_PIN,OUTPUT);
    backlight_state = OFF;
  }
  else if(mode == "ON"){
    pinMode(BACKLIGHT_PIN,INPUT);
    backlight_state = ON;
  }
    
}

void LCDUpdater(bool btnPressed = false) {
  if(btnPressed){
    btn_last_press = millis();
  }
  if(millis() - btn_last_press >= LCDTimeOut * 1000 && millis() - btn_last_press < LCDTimeOut * 1000 + 1000){
    backLightControll("OFF");
  }
}

String make_str(String str){
    for(int i = 0; i < (16 - str.length()); i++) {
        str += ' '; 
    }
    return str;
}

void writeToDisplay(String firstText, String secondText=" "){
        
        lcd.setCursor(0, 0);
        lcd.print(firstText);
        lcd.print("                ");

        lcd.setCursor(0,1);
        lcd.print(secondText);
        lcd.print("                ");
}

void checkDayTime() {
    if (error) return;
    /* Turn off watering */
    int valanda = hour();
    if (valanda >= OffHour || valanda < OnHour) {
      if(!isItNight){
        Serial.println("Labanakt.. ZzZzZz");        
        writeToDisplay("Labanakt.. ZzZzZz");
        
        backLightControll("OFF");
      }
        isItNight = true;

        /* Turn off everything */
        digitalWrite(pump, 0);
    }
    /* Turn on watering */
    else if (valanda >= OnHour) {
      if(isItNight){
        Serial.println("Labas rytas!");
        writeToDisplay("Labas rytas!");
      }
        isItNight = false;
    } else {
        isItNight = true;  // For redundancy just turn off watering
        error = true;
    }
}

int waterSensor() {
    const long moist_val = analogRead(water_sensor);
    Serial.print("Analog: ");
    Serial.println(moist_val);
    const int moist_proc = map(moist_val, drySoil, pureWater, 0, 100);
    Serial.print("Procentai: ");
    Serial.println(moist_proc);

    return moist_proc;
}

// read the buttons
int read_LCD_buttons()
{
 int adc_key_in = analogRead(buttons);
 if (adc_key_in > 1000){return btnNONE;}
 else if (adc_key_in < 100){return btnRIGHT;}
 else if (adc_key_in < 250){return btnUP;}
 else if (adc_key_in < 450){return btnDOWN;}
 else if (adc_key_in < 650){return btnLEFT;}
 else if (adc_key_in < 850){return btnSELECT;}
 else{return btnNONE;}
}

void menu(String button){
  LCDUpdater(true);
  if(!menu_open && button == "DOWN"){
    menu_open = true;
    menu_num = 0;
  }
  
  if(menu_open){
    if(button == "DOWN"){
      menu_num++;
    }
    else if(button == "UP"){
      menu_num--;
    }
    else if(button == "RIGHT"){
      if(menu_num == 1) HighBar++;
      if(menu_num == 2) LowBar++;
      if(menu_num == 3) OnHour++;
      if(menu_num == 4) OffHour++;
    }
    else if(button == "LEFT"){
      if(menu_num == 1) HighBar--;
      if(menu_num == 2) LowBar--;
      if(menu_num == 3) OnHour--;
      if(menu_num == 4) OffHour--;
    }

    // Check if menu has reached end
    if (menu_num > 4 || menu_num < 1){
      menu_open = false;
      menu_num = 0;
    }
    
    switch(menu_num){
      case 1:
      {
        writeToDisplay(menuItems[menu_num - 1], String(HighBar));
        break;
      }
      case 2:
      {
        writeToDisplay(menuItems[menu_num - 1], String(LowBar));
        break;
      }
      case 3:
      {
        writeToDisplay(menuItems[menu_num - 1], String(OnHour));
        break;
      }
      case 4:
      {
        writeToDisplay(menuItems[menu_num - 1], String(OffHour));
        break;
      }
    }
  }
}

void loop() {
  LCDUpdater();
   int button_pressed = read_LCD_buttons();

   if(button_pressed == btnNONE){
    button_released = true;
   }
   
  if(button_released){
    switch (button_pressed){
   case btnRIGHT:
     {
      button_released = false;
      menu("RIGHT");
     break;
     }
   case btnLEFT:
     {
     button_released = false;
     menu("LEFT");
     break;
     }
   case btnUP:
     {
     button_released = false;
     menu("UP");
     break;
     }
   case btnDOWN:
     {
     menu("DOWN");
     button_released = false;
     break;
     }
   case btnSELECT:
     {
     backLightControll();
     button_released = false;
     break;
     }
     case btnNONE:
     {
     break;
     }
 }
  }
  if (VandensMetro.check()) {      
        
        if (error) return;
        if (isItNight) {
            digitalWrite(pump, LOW);
            return;
        }

        int proc = waterSensor();
        if(!menu_open){
          if (proc < LowBar) {
            digitalWrite(pump, HIGH);
            pumpOn = true;

            writeToDisplay(String("Dregme: ") += String(proc), "Laistoma");

        } else if (proc <= HighBar) {
            // Pompos veikimas nekeiciamas
            String pumpStatus = "Nelaistoma";
            if(pumpOn){
              String pumpStatus = "Laistoma";
            }

            writeToDisplay(String("Dregme: ") += String(proc), pumpStatus);

        } else if (proc > HighBar) {
            digitalWrite(pump, LOW);
            pumpOn = false;

            writeToDisplay(String("Dregme: ") += String(proc), "Nelaistoma");

        }
        }        
    }

    /* Check if it's night time */
    if (TimeCheckMetro.check()) {
      if(!menu_open){
        checkDayTime();
      }
    }

    /* Checkign if error exists */
    if (error) {
        if (ErrorCheckMetro.check()) {

        }
    }
    
}
