## About
Arduino based project, for watering green house crops. 

## Components
- Arduino UNO
- Robot D1 LCD 16x2 display with buttons
- DS3231 real time clock
- Capacitive Soil Moisture Sensor v2.0
- 12v solenoid valve

## Functions
- Set moisture ranges. Acording to them the solenoid valve will open.
- Set watering time. Useful for crops, which shouldn't be watered at night.
- Easy to use menu for changing all the above aspects of the watering cycle.
- Ability to turn off LCD backlight with the `select` button.
- By default display allways shows water in soil perecntage and if the valve is open. 
- If errors occurre, you can add desired code to be executed. 

## How to use
- Connect DS3231 real time clock to arduino and set the current date and time. I used [DS1307RTC](https://www.rduino.cc/reference/en/libraries/ds1307rtc/) library to set date and time and [DS3232RTC](https://github.com/JChristensen/DS3232RTC) to read date and time. 
- Calibrate your moisture sensor by changing the `drySoil` and `pureWater` variables so that they match you highest and lowest sensor readings.
- Connect a 12v power supply and connect the `vin` pin to the positive valve side. Add a transistor to the negative valve side and controll it with `D2` pin.

## Pin layout
- `A0` used by the D1 Robot LCD for mapping buttons.
- `A2` mapped to recieve analog data from the moisture sensor.
- `D2` solenoid valve.
- `D10` LCD backlight. **IMPORTANT** Only this pin can be used to turn off the backlight. Changing the script for controlling the backlight is not advised, because on some Uno boards it can cause damage.

