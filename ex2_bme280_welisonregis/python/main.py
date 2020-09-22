from time import *

# Requires RPi_I2C_driver.py
import RPi_I2C_driver
import smbus2
import bme280

# Algorithm constants
BME_I2C_PORT = 1
BME_I2C_ADDRESS = 0x76

# Setup LCD display
mylcd = RPi_I2C_driver.lcd()

# Setup bme280 device settings
i2c_port = BME_I2C_PORT
address = BME_I2C_ADDRESS 
bus = smbus2.SMBus(i2c_port)
bme280_parameters = bme280.load_calibration_params(bus, address)

print("Temperature, Pressure, Humidity")

while True:
    # Retrieve bme280 sensor data
    data = bme280.sample(bus, address, bme280_parameters)
    temperature = data.temperature
    humidity = data.humidity
    pressure = data.pressure
    
    print(f"{temperature:.2f} °C, {pressure:.2f} hPa, {humidity:.2f} %")

    tempereture_humidity_str = f"T {temperature:.2f}"
    pressure_str = f"U {humidity:.2f} P {pressure:.2f}"

    # Display data in 16x2 LCD
    mylcd.lcd_display_string(tempereture_humidity_str, 1)
    mylcd.lcd_display_string(pressure_str, 2)

    sleep(1)
