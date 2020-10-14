#include <pthread.h>
#include "bme280_api.h"
#include "bcm2835_api.h"
#include "lcd.h"
#include "system_monitor.h"
#include "uart_api.h"
#include "system_windows.h"

/* Global variables */
struct system_data enviroment_data;
int lcd_file_descriptor;

/* Main functions */
void handle_all_interruptions(int signal);
void set_system_temperatures();
void control_actuators();
void* update_control_system_temperature();
void* store_display_temperature();

/* Program threads */
pthread_t manage_user_inputs;
pthread_t display_system_status;
pthread_t manage_temperature_thread;
pthread_t store_display_thead;


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

    /* Setup initial state to system data */
    enviroment_data.internal_temperature = 0;
    enviroment_data.external_temperature = 0;
    enviroment_data.reference_temperature = 40;
    enviroment_data.hysteresis = 4;
    enviroment_data.reference_temperature_type = IS_POTENTIOMETER_REFERENCE;

    /* Setup lcd display */
    if (wiringPiSetup () == -1) exit (1);
    lcd_file_descriptor = wiringPiI2CSetup(I2C_ADDR);
    lcd_init(lcd_file_descriptor);

    /* Setup actuators devices */
    setup_devices();

    /* Setup bme280 - External temperature */
    setup_bme280();

    /* Initialize system apresentation with ncurses */
    init_system_apresentation(&enviroment_data);

    /* Create system theareds */
    pthread_create(&manage_temperature_thread, NULL, &update_control_system_temperature, NULL);
    pthread_create(&store_display_thead, NULL, &store_display_temperature, NULL);
    pthread_create(&manage_user_inputs, NULL, &setup_menu_windows, NULL);
    usleep(10000); /* Wait thread setup of ncurses input region */
    pthread_create(&display_system_status, NULL, &setup_system_status_interface, NULL);

    /* Join and finalize threads */
    pthread_join(manage_temperature_thread, NULL);
    pthread_join(store_display_thead, NULL);
    pthread_join(manage_user_inputs, NULL);
    pthread_join(display_system_status, NULL);

    return 0;
}

/*!
 * @brief Function used to handle system interruptions and close all connections.
 */
void handle_all_interruptions(int signal) {
    /* Cancel all threads activies */
    pthread_cancel(manage_user_inputs);
    pthread_cancel(display_system_status);
    pthread_cancel(manage_temperature_thread);
    pthread_cancel(store_display_thead);

    /* Close important system resources */
    handle_actuators_interruption();
    clear_lcd(lcd_file_descriptor);
    clear_ncurses_alocation();
}

/*!
 * @brief Function used to set valid system temperatures (internal, external, reference)
 */
void set_system_temperatures() {
    float external_temperature = get_bme280_temperature();
    if(external_temperature >= 0) {
        enviroment_data.external_temperature = external_temperature;
    }

    float internal_temperature = uart(ASK_INTERNAL_TEMPERATURE);
    if(internal_temperature >= 0) {
        enviroment_data.internal_temperature = internal_temperature;
    }

    if(enviroment_data.reference_temperature_type == IS_POTENTIOMETER_REFERENCE) {
        float reference_temperature = uart(ASK_REFERENCE_TEMPERATURE);
        if(reference_temperature >= 0) {
            enviroment_data.reference_temperature = reference_temperature;
        }
    }

}

/*!
 * @brief Function used to control actuators (ventilador and resistor)
 * based on internal temperature, reference temperature and hysteresis.
 */
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

/*!
 * @brief Function used to update data and actuators status every 500ms.
 */
void* update_control_system_temperature() {
    while(1) {
        set_system_temperatures();
        control_actuators();
        usleep(500000);
    }
}

/*!
 * @brief Function used to store system data (csv) and display temperatures into LCD.
 */
void* store_display_temperature() {
    while(1) {
        store_temperature_data(&enviroment_data);
        display_temperatures_lcd(lcd_file_descriptor, &enviroment_data);
        sleep(2);
    }
}
