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
 * @date       2020-09-22
 * @version    v3.5.1
 */

#include "bme280_api.h"

/*!
 * @brief This function reading the sensor's registers through I2C bus.
 */
int8_t user_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr)
{
    struct identifier id;

    id = *((struct identifier *)intf_ptr);

    write(id.fd, &reg_addr, 1);
    read(id.fd, data, len);

    return 0;
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
    struct identifier id;

    id = *((struct identifier *)intf_ptr);

    buf = malloc(len + 1);
    buf[0] = reg_addr;
    memcpy(buf + 1, data, len);
    if (write(id.fd, buf, len + 1) < (uint16_t)len)
    {
        return BME280_E_COMM_FAIL;
    }

    free(buf);

    return BME280_OK;
}

/*!
 * @brief This API used to save the sensor temperature, pressure and humidity data.
 */
void save_sensor_data(struct bme280_data *sensor_data) {
    float temperature, pressure, humidity;

#ifdef BME280_FLOAT_ENABLE
    temperature = sensor_data->temperature;
    pressure = 0.01 * sensor_data->pressure;
    humidity = sensor_data->humidity;
#else
#ifdef BME280_64BIT_ENABLE
    temperature = 0.01f * sensor_data->temperature;
    pressure = 0.0001f * sensor_data->pressure;
    humidity = 1.0f / 1024.0f * sensor_data->humidity;
#else
    temperature = 0.01f * sensor_data->temperature;
    pressure = 0.01f * sensor_data->pressure;
    humidity = 1.0f / 1024.0f * sensor_data->humidity;
#endif
#endif

    sensor_data->temperature = temperature;
    sensor_data->pressure = pressure;
    sensor_data->humidity = humidity;

    printf("%0.2lf °C, %0.2lf hPa, %0.2f%%\n", 
        sensor_data->temperature, sensor_data->pressure, sensor_data->humidity);
}

/*!
 * @brief This API used to save the current date (DD-MM-YYYY) into a string.
 */
void set_current_formatted_date(char *formatted_date) {
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    sprintf(formatted_date, "%02d-%02d-%04d",
        timeinfo->tm_mday, timeinfo->tm_mon+1, 1900+timeinfo->tm_year);
}

/*!
 * @brief This API used to save the current hour (HH:MM:SS) into a string.
 */
void set_current_formatted_hour(char *formatted_hour) {
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    sprintf(formatted_hour, "%02d:%02d:%02d",
        timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

/*!
 * @brief This API used to store in csv file the sensor temperature, pressure and humidity mean.
 */
void store_sensor_data_mean(struct bme280_data *sensor_data, int n) {
    char filepath[] = "data/data_track.csv";
    FILE *fp = fopen(filepath,"a+");
    fseek (fp, 0, SEEK_END);
    
    /* If file is not defined, return */
    if(fp == NULL) {
        printf(">> Não foi possível salvar a média dos dados medidos.\n");
        return;
    }

    /* Add header if file is empty */
    if(ftell(fp) == 0) {
        fprintf(fp, "Temperatura (°C),Pressão (hPa),Umidade (%%),Data (DD-MM-YYYY),Hora (HH:MM:SS)\n");
    }

    /* Data structure to store temperature, pressure and humidity mean */
    struct bme280_data sensor_data_mean;
    sensor_data_mean.temperature = 0.0;
    sensor_data_mean.pressure = 0.0;
    sensor_data_mean.humidity = 0.0;

    /* Compute the data mean over n samples */
    for (int i = 0; i < n; i++) {
        sensor_data_mean.temperature += sensor_data[i].temperature;
        sensor_data_mean.pressure += sensor_data[i].pressure;
        sensor_data_mean.humidity += sensor_data[i].humidity;
    }
    sensor_data_mean.temperature /= (float) n;
    sensor_data_mean.pressure /= (float) n;
    sensor_data_mean.humidity /= (float) n;

    /* Store data mean rounded with one decimal place */
    fprintf(fp, "%0.2lf,%0.2lf,%0.2lf",
        sensor_data_mean.temperature, sensor_data_mean.pressure, sensor_data_mean.humidity);

    /* Store data as DD-MM-YYYY */
    const int date_size = 11;
    char formatted_date[date_size];
    set_current_formatted_date(formatted_date);
    fprintf(fp, ",%s", formatted_date);

    /* Store current hour as HH:MM:SS */
    const int hour_size = 9;
    char formatted_hour[hour_size];
    set_current_formatted_hour(formatted_hour);
    fprintf(fp, ",%s\n", formatted_hour);

    printf(">> Média dos dados salvo em %s.\n", filepath);
    fclose(fp);
}

/*!
 * @brief This API reads the sensor temperature, pressure and humidity data in forced mode.
 */
int8_t stream_sensor_data_forced_mode(struct bme280_dev *dev)
{
    /* Variable to define the result */
    int8_t device_response = BME280_OK;

    /* Variable to define the selecting sensors */
    uint8_t settings_sel = 0;

    /* Variable to store minimum wait time between consecutive measurement in force mode */
    uint32_t req_delay;

    /* Recommended mode of operation: Indoor navigation */
    dev->settings.osr_h = BME280_OVERSAMPLING_1X;
    dev->settings.osr_p = BME280_OVERSAMPLING_16X;
    dev->settings.osr_t = BME280_OVERSAMPLING_2X;
    dev->settings.filter = BME280_FILTER_COEFF_16;

    settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

    /* Set the sensor settings */
    device_response = bme280_set_sensor_settings(settings_sel, dev);
    if (device_response != BME280_OK)
    {
        fprintf(stderr, "Failed to set sensor settings (code %+d).", device_response);

        return device_response;
    }

    printf("Temperature, Pressure, Humidity\n");

    /*Calculate the minimum delay required between consecutive measurement based upon the sensor enabled
     *  and the oversampling configuration. */
    req_delay = bme280_cal_meas_delay(&dev->settings);

    /* Structure to get the pressure, temperature and humidity values */
    struct bme280_data sensor_data[SENSOR_DATA_MEAN_SIZE];

    /* Iterator to track the mean every SENSOR_DATA_MEAN_SIZE seconds */
    int it = 0;

    /* Continuously stream sensor data */
    while (1)
    {
        /* Set the sensor to forced mode */
        device_response = bme280_set_sensor_mode(BME280_FORCED_MODE, dev);
        if (device_response != BME280_OK)
        {
            fprintf(stderr, "Failed to set sensor mode (code %+d).", device_response);
            break;
        }

        /* Wait for the measurement to complete and print data */
        dev->delay_us(req_delay, dev->intf_ptr);
        /* sleep 1 second to give space between measures */
        sleep(1);

        device_response = bme280_get_sensor_data(BME280_ALL, &sensor_data[it], dev);
        if (device_response != BME280_OK)
        {
            fprintf(stderr, "Failed to get sensor data (code %+d).", device_response);
            break;
        }

        /* Store one measure from sensor to array */
        save_sensor_data(&sensor_data[it]);

        /* Store the mean of the data in a csv file every 10 seconds */
        if(it == SENSOR_DATA_MEAN_SIZE-1) {
            store_sensor_data_mean(sensor_data, SENSOR_DATA_MEAN_SIZE);
        }

        it = (it+1)%SENSOR_DATA_MEAN_SIZE;
    }

    return device_response;
}

/*!
 * @brief This API setup the initial environment used by bme280.
 */
int8_t setup_bme280(struct bme280_dev *device) {
    // struct bme280_dev device;
    struct identifier id;

    /* Variable to define the result */
    int8_t device_response = BME280_OK;

    if ((id.fd = open(I2C_PATH, O_RDWR)) < 0)
    {
        fprintf(stderr, "Failed to open the i2c bus %s\n", I2C_PATH);
        exit(1);
    }

    /* Make sure to select BME280_I2C_ADDR_PRIM or BME280_I2C_ADDR_SEC as needed */
    id.dev_addr = BME280_I2C_ADDR_PRIM;

    device->intf = BME280_I2C_INTF;
    device->read = user_i2c_read;
    device->write = user_i2c_write;
    device->delay_us = user_delay_us;

    /* Update interface pointer with the structure that contains both device address and file descriptor */
    device->intf_ptr = &id;

    if (ioctl(id.fd, I2C_SLAVE, id.dev_addr) < 0)
    {
        fprintf(stderr, "Failed to acquire bus access and/or talk to slave.\n");
        return BME280_SETUP_FAIL;
    }

    /* Initialize the bme280 */
    device_response = bme280_init(device);
    if (device_response != BME280_OK)
    {
        fprintf(stderr, "Failed to initialize the device (code %+d).\n", device_response);
        return BME280_SETUP_FAIL;
    }

    return BME280_OK;
}