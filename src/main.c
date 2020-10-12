#include "bme280_api.h"
#include "bcm2835_api.h"
#include "lcd.h"
#include "system_monitor.h"
#include "uart_api.h"

/* Global variables */
struct system_data enviroment_data;
int lcd_file_descriptor;

/* Main functions */
void handle_all_interruptions(int signal);
void set_system_temperatures();
void control_actuators();
void update_control_system_temperature();
void store_display_temperature();


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

    enviroment_data.hysteresis = 4;
    
    /* Setup lcd display */
    if (wiringPiSetup () == -1) exit (1);
    lcd_file_descriptor = wiringPiI2CSetup(I2C_ADDR);
    lcd_init(lcd_file_descriptor);

    /* Setup actuators devices */
    setup_devices();

    /* Setup bme280 - External temperature */
    setup_bme280();

    while(1) {
        update_control_system_temperature();
        store_display_temperature();
    }

    return 0;
}

void handle_all_interruptions(int signal) {
    printf("\nEnding the program... Finishing all connections!\n");
    handle_actuators_interruption(signal);
}

void set_system_temperatures() {
    float external_temperature = get_bme280_temperature();
    if(external_temperature > 0) {
        enviroment_data.external_temperature = external_temperature;
    }

    float internal_temperature = uart(ASK_INTERNAL_TEMPERATURE);
    if(internal_temperature > 0) {
        enviroment_data.internal_temperature = internal_temperature;
    }

    float reference_temperature = uart(ASK_REFERENCE_TEMPERATURE);
    if(reference_temperature > 0) {
        enviroment_data.reference_temperature = reference_temperature;
    }
}

void control_actuators() {
    float reference_temperature = enviroment_data.reference_temperature;
    float internal_temperature = enviroment_data.internal_temperature;
    float hysteresis = enviroment_data.hysteresis;

    if (internal_temperature > reference_temperature + hysteresis/2.0) {
        enable_ventilator();
        disable_resistence();
    }
    else if (internal_temperature < reference_temperature - hysteresis/2.0) {
        disable_ventilator();
        enable_resistence();
    }
}

void update_control_system_temperature() {
    set_system_temperatures();
    control_actuators();
    usleep(500);
}

void store_display_temperature() {
    store_temperature_data(&enviroment_data);
    display_temperatures_lcd(lcd_file_descriptor, &enviroment_data);
    sleep(2);
}
