# Raspberry Pi Pico Projects

Small projects for testing hardware with Raspberry Pi Pico

Hereinafter, there is a list with a short description of every project done using the Raspberry Pi Pico:
* `Weather Forecast`: Temperature, humidity and pressure sensor by using DPS310 and SHT-4x sensors. Moreover, it is the code for using a BME680 in order to compare measurements
* `LCD`: Testing LCD panel and creating independent c and header inspired on pico-examples repository
* `freeRTOS`: Testing freeRTOS library on Raspberry Pi Pico

## Weather Forecast
In this case, the hardware design of the sensor has been developed by myself. It can be found at the following repository: https://github.com/jaumefe/Electronic_Projects/tree/main/Home_Automation/Weather_Forecast

DPS310 sensor has some internal default and it does not give the appropriated pressure measurements.

In order to obtain pressure measurements, BME680 sensor has been added so it can be compared the results of both sensors.

In folder Data, there is a Python script so as to see the output send by USB through the Raspberry Pi Pico

## LCD
Tests on pico example about i2c and LCD panel: https://github.com/raspberrypi/pico-examples/tree/master/i2c/lcd_1602_i2c

## freeRTOS
Testing the freeRTOS library for RP2040
