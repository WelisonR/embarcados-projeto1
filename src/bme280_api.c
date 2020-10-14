/**\
 * Copyright (c) 2020 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

/*! @file bme280_api.c
 * @brief Sensor API for BME280 sensor
 */

/*!
 * Minor changes, adds and improvements by WelisonR.
 * @file       bme280_api.c
 * @date       2020-10-11
 * @version    v3.5.1
 */

#include "bme280_api.h"

/* Global variables */
struct bme280_dev device;
struct identifier id;

/*!
 * @brief This function reading the sensor's registers through I2C bus.
 */
int8_t user_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr)
{
    write(id.file_descriptor, &reg_addr, 1);
    read(id.file_descriptor, data, len);
    return BME280_OK;
}

/*!
 * @brief This function provides the delay for required time (Microseconds) as per the input provided in some of the
 * APIs
 */
void user_delay_us(uint32_t period, void *intf_ptr)
{
    usleep(period);
}

/*!
 * @brief This function for writing the sensor's registers through I2C bus.
 */
int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr)
{
    uint8_t *buf;

    buf = malloc(len + 1);
    buf[0] = reg_addr;
    memcpy(buf + 1, data, len);
    if (write(id.file_descriptor, buf, len + 1) < (uint16_t)len)
    {
        return BME280_E_COMM_FAIL;
    }

    free(buf);

    return BME280_OK;
}

/*!
 * @brief Function used to setup the read configurations to bme280 sensor.
 */
int8_t setup_bme280_read_mode()
{
	int8_t response = BME280_OK;
	uint8_t device_settings_options;

	/* Recommended mode of operation: Indoor navigation */
	device.settings.osr_h = BME280_OVERSAMPLING_1X;
	device.settings.osr_p = BME280_OVERSAMPLING_16X;
	device.settings.osr_t = BME280_OVERSAMPLING_2X;
	device.settings.filter = BME280_FILTER_COEFF_16;

	device_settings_options = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

    /* Set the sensor settings */
	response = bme280_set_sensor_settings(device_settings_options, &device);

    if (response != BME280_OK)
    {
        fprintf(stderr, "Failed to set sensor settings (code %+d).", response);
        return response;
    }

    /* Set the sensor to normal mode */
	response = bme280_set_sensor_mode(BME280_NORMAL_MODE, &device);

    if (response != BME280_OK)
    {
        fprintf(stderr, "Failed to set sensor mode (code %+d).", response);
	    return response;
    }

	return response;
}

/*!
 * @brief Function used to setup and initialize all the connection with bme280 sensor.
 */
void setup_bme280()
{
    int8_t response = BME280_OK;

    if ((id.file_descriptor = open(I2C_PATH, O_RDWR)) < 0)
    {
        fprintf(stderr, "Failed to open the i2c bus %s\n", I2C_PATH);
        raise(SIGABRT);
    }

    /* Select BME280_I2C_ADDR_PRIM or BME280_I2C_ADDR_SEC as needed */
    id.device_address = BME280_I2C_ADDR_PRIM;

    if (ioctl(id.file_descriptor, I2C_SLAVE, id.device_address) < 0)
    {
        fprintf(stderr, "Failed to acquire bus access and/or talk to slave.\n");
        raise(SIGABRT);
    }

    /* Make sure to select BME280_I2C_ADDR_PRIM or BME280_I2C_ADDR_SEC as needed */
    device.intf = BME280_I2C_INTF;
    device.read = user_i2c_read;
    device.write = user_i2c_write;
    device.delay_us = user_delay_us;

    /* Update interface pointer with the structure that contains both device address and file descriptor */
    device.intf_ptr = &id;

    /* Initialize the bme280 */
    response = bme280_init(&device);
    if (response != BME280_OK)
    {
        fprintf(stderr, "Failed to initialize the device (code %+d).\n", response);
        raise(SIGABRT);
    }

    response = setup_bme280_read_mode();
    if (response != BME280_OK)
    {
        fprintf(stderr, "Failed to stream sensor data (code %+d).\n", response);
        raise(SIGABRT);
    }
}

/*!
 * @brief Function used to get the temperature value from bme280 sensor.
 */
float get_bme280_temperature()
{
    uint32_t minimum_delay = bme280_cal_meas_delay(&device.settings);
    /* Structure to store temperature, humidity and pressure from bme280 */
    struct bme280_data sensor_data;

    user_delay_us(minimum_delay, device.intf_ptr);
    usleep(200000); /* Safe read */

    int8_t response = bme280_get_sensor_data(BME280_ALL, &sensor_data, &device);
    if (response != BME280_OK)
    {
        fprintf(stderr, "Failed to get sensor data (code %+d).", response);
        return -1.0f;
    }

    return sensor_data.temperature;
}