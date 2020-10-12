#include "bme280_api.h"
#include "bcm2835_api.h"
#include "lcd.h"
#include "system_monitor.h"
#include "uart_api.h"

void handle_all_interruptions(int signal) {
    printf("\nEnding the program... Finishing all connections!\n");
    handle_actuators_interruption(signal);
}

/*!
 * @brief This function starts execution of the program.
 */
int main(int argc, char* argv[])
{
    signal(SIGHUP, handle_all_interruptions);
    signal(SIGINT, handle_all_interruptions);
    signal(SIGQUIT, handle_all_interruptions);
    signal(SIGILL, handle_all_interruptions);
    signal(SIGABRT, handle_all_interruptions);
    signal(SIGBUS, handle_all_interruptions);
    signal(SIGSEGV, handle_all_interruptions);

    struct system_data enviroment_data;
    
    /* Setup lcd display */
    if (wiringPiSetup () == -1) exit (1);
    int lcd_file_descriptor = wiringPiI2CSetup(I2C_ADDR);
    lcd_init(lcd_file_descriptor);

    /* Setup actuators devices */
    setup_devices();

    /* Setup bme280 - External temperature */
    setup_bme280();

    while(1) {
        enviroment_data.external_temperature = get_bme280_temperature();
        enviroment_data.internal_temperature = uart(ASK_INTERNAL_TEMPERATURE);
        sleep(1);
        enviroment_data.reference_temperature = uart(ASK_REFERENCE_TEMPERATURE);

        store_temperature_data(&enviroment_data);

        sleep(3);
    }


    return 0;
}
