# Raspberry Pi Pico Projects

Hereinafter, there is a list with a short description of every project done using the Raspberry Pi Pico:
* BME680: Temperature, humidity, pressure and gas resistance sensor for indoor air quality.
* Weather Forecast: Temperature, humidity and pressure sensor by using DPS310 and SHT-4x sensors

## BME680
Raspberry Pi Pico code for interfacing with a BME680 module.
Nowadays, this project is not connected to Bosch API, so the gas resistance measurements do not give us any feedback.

## Weather Forecast
In this case, the hardware design of the sensor has been developed by myself. It can be found at the following repository: https://github.com/jaumefe/Electronic_Projects/tree/main/Home_Automation/Weather_Forecast

DPS310 sensor has some internal default and it does not give the appropriated pressure measurements.

In order to obtain pressure measurements, BME680 sensor has been added so it can be compared the results of both sensors.

In folder Data, there is a Python script so as to see the output send by USB through the Raspberry Pi Pico

## LCD
Some tests on transforming a Arduino library into a RP2040 library for LCD panels

## freeRTOS
Testing the freeRTOS library for RP2040
