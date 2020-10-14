#include <pthread.h>
#include "bme280_api.h"
#include "bcm2835_api.h"
#include "lcd.h"
#include "system_monitor.h"
#include "uart_api.h"
#include "system_windows.h"

#define ALARM_MAXIMUM_CYCLE 4
#define ALARM_TIME_SIZE 500000

/* Main functions */
void handle_alarm();
void handle_all_interruptions(int signal);
void* set_system_temperatures();
void* control_actuators();
void* store_display_temperature();

/* Global variables */
struct system_data enviroment_data;
int lcd_file_descriptor;
int alarm_step = 0;

/* Program threads */
pthread_t manage_user_inputs;
pthread_t display_system_status;
pthread_t set_temperature_thread;
pthread_t control_actuators_thread;
pthread_t store_display_thead;

/* pthreads mutex controlled by alarm */
pthread_mutex_t set_temperature_mutex;
pthread_mutex_t control_actuators_mutex;
pthread_mutex_t store_display_mutex;

/*!
 * @brief This function starts execution of the program.
 */
int main(int argc, char* argv[])
{
    signal(SIGALRM, handle_alarm);
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

    /* Initialize mutex to threads */
    pthread_mutex_init(&set_temperature_mutex, NULL);
    pthread_mutex_init(&control_actuators_mutex, NULL);
    pthread_mutex_init(&store_display_mutex, NULL);

    /* Lock thread executions */
    pthread_mutex_lock(&set_temperature_mutex);
    pthread_mutex_lock(&control_actuators_mutex);
    pthread_mutex_lock(&store_display_mutex);

    /* Create system threads */
    pthread_create(&set_temperature_thread, NULL, &set_system_temperatures, NULL);
    pthread_create(&control_actuators_thread, NULL, &control_actuators, NULL);
    pthread_create(&store_display_thead, NULL, &store_display_temperature, NULL);
    pthread_create(&manage_user_inputs, NULL, &setup_menu_windows, NULL);
    usleep(10000); /* Wait thread setup of ncurses input region */
    pthread_create(&display_system_status, NULL, &setup_system_status_interface, NULL);

    /* Initialize alarm callbacks */
    ualarm(ALARM_TIME_SIZE, ALARM_TIME_SIZE);

    /* Join and finalize threads */
    pthread_join(set_temperature_thread, NULL);
    pthread_join(control_actuators_thread, NULL);
    pthread_join(store_display_thead, NULL);
    pthread_join(manage_user_inputs, NULL);
    pthread_join(display_system_status, NULL);

    return 0;
}

/*!
 * @brief Function used to handle alarm that occurs every 500ms (one step)
 * and open threads execution.
 */
void handle_alarm()
{
    /* Unlock set temperature and control actuators every 500ms */
    pthread_mutex_unlock(&set_temperature_mutex);
    pthread_mutex_unlock(&control_actuators_mutex);

    /* Unlock LCD display and data storage every 2s */
    if (alarm_step == 0) {
        pthread_mutex_unlock(&store_display_mutex);
    }

    /* Control thread execution based on step (1, 2, 3, 4) */
    alarm_step = (alarm_step + 1) % ALARM_MAXIMUM_CYCLE;
}


/*!
 * @brief Function used to handle system interruptions and close all connections.
 */
void handle_all_interruptions(int signal) {
    /* As first argument is zero, cancel alarms on Linux (and others) systems */
    ualarm(0, 0);

    /* Cancel all threads activies */
    pthread_cancel(display_system_status);
    pthread_cancel(manage_user_inputs);
    pthread_cancel(store_display_thead);
    pthread_cancel(set_temperature_thread);
    pthread_cancel(control_actuators_thread);

    /* Destroy thread mutex */
    pthread_mutex_destroy(&set_temperature_mutex);
    pthread_mutex_destroy(&control_actuators_mutex);
    pthread_mutex_destroy(&store_display_mutex);

    /* Close important system resources */
    handle_actuators_interruption();
    clear_lcd(lcd_file_descriptor);
    clear_ncurses_alocation();
    exit(0);
}

/*!
 * @brief Function used to set valid system temperatures (internal, external, reference)
 */
void* set_system_temperatures() {
    while(1) {
        pthread_mutex_lock(&set_temperature_mutex);

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
}

/*!
 * @brief Function used to control actuators (ventilador and resistor)
 * based on internal temperature, reference temperature and hysteresis.
 */
void* control_actuators() {
    while(1) {
        pthread_mutex_lock(&control_actuators_mutex);

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
}

/*!
 * @brief Function used display temperatures (internal, external and reference) into LCD.
 */
void* store_display_temperature() {
    while(1) {
        pthread_mutex_lock(&store_display_mutex);
        store_temperature_data(&enviroment_data);
        display_temperatures_lcd(lcd_file_descriptor, &enviroment_data);
    }
}
