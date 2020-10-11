#include "bme280_api.h"
#include "bcm2835_api.h"
#include "lcd.h"
#include "system_monitor.h"
#include "uart_api.h"

/*!
 * @brief This function starts execution of the program.
 */
int main(int argc, char* argv[])
{
    // setup_devices();
    // signal(SIGINT, handle_interruption);

    // while(1) {
    //     sleep(5);
    //     enable_ventilator();
    //     sleep(15);
    // }

    // return 0;

    // struct bme280_dev device;
    // int8_t device_setup_status = setup_bme280(&device);

    // if(device_setup_status == BME280_SETUP_FAIL) {
    //     exit(1);
    // }

    // int8_t device_response = BME280_OK;
    // device_response = stream_sensor_data_forced_mode(&device);
    // if (device_response != BME280_OK)
    // {
    //     fprintf(stderr, "Failed to stream sensor data (code %+d).\n", device_response);
    //     exit(1);
    // }

    // return 0;
}
