#include "bme280_api.h"

/*!
 * @brief This function starts execution of the program.
 */
int main(int argc, char* argv[])
{
    struct bme280_dev dev;

    struct identifier id;

    /* Variable to define the result */
    int8_t rslt = BME280_OK;

    if (argc < 2)
    {
        fprintf(stderr, "Missing argument for i2c bus.\n");
        exit(1);
    }

    if ((id.fd = open(argv[1], O_RDWR)) < 0)
    {
        fprintf(stderr, "Failed to open the i2c bus %s\n", argv[1]);
        exit(1);
    }

    /* Make sure to select BME280_I2C_ADDR_PRIM or BME280_I2C_ADDR_SEC as needed */
    id.dev_addr = BME280_I2C_ADDR_PRIM;

    dev.intf = BME280_I2C_INTF;
    dev.read = user_i2c_read;
    dev.write = user_i2c_write;
    dev.delay_us = user_delay_us;

    /* Update interface pointer with the structure that contains both device address and file descriptor */
    dev.intf_ptr = &id;

    if (ioctl(id.fd, I2C_SLAVE, id.dev_addr) < 0)
    {
        fprintf(stderr, "Failed to acquire bus access and/or talk to slave.\n");
        exit(1);
    }

    /* Initialize the bme280 */
    rslt = bme280_init(&dev);
    if (rslt != BME280_OK)
    {
        fprintf(stderr, "Failed to initialize the device (code %+d).\n", rslt);
        exit(1);
    }

    rslt = stream_sensor_data_forced_mode(&dev);
    if (rslt != BME280_OK)
    {
        fprintf(stderr, "Failed to stream sensor data (code %+d).\n", rslt);
        exit(1);
    }

    return 0;
}
