# Temperature and Humidity Sensor

## Goals
* Monitor temperature and humidity (Duh...)
* Use MQTT to send data to central location
* Be able to modify basic parameters, such as time between check, alarm thresholds, etc.) without needing to recompile and download the code.

## To Do
* How to modify basic parameters without re-compiling and downloading the code
* Build from scratch using ESP8266 instead of Adafruit Huzzah ESP8266 for cleaner build?
* calculate how long the stuff after the calculations to determine how long to sleep takes to make the run time more accurate
* determine and report battery status
* put the system to sleep to save battery
* add variable to note which pin the LED is connected to that will be "blinked" in config.h and update pinModes with that variable
* wait longer for serial connection to come up so that all details show up on the serial connection
* determine if connected to batter or other power

## Done
* 
