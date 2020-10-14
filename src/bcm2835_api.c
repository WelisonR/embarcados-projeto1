#include <bcm2835.h>
#include "bcm2835_api.h"

/*!
 * @brief Function used to enable the ventilator. (HIGH/LOW logic inverted)
 */
void enable_ventilator() {
    bcm2835_gpio_write(VENTILATOR, LOW);
}

/*!
 * @brief Function used to disable the ventilator. (HIGH/LOW logic inverted)
 */
void disable_ventilator() {
    bcm2835_gpio_write(VENTILATOR, HIGH);
}

/*!
 * @brief Function used to enable the resistence. (HIGH/LOW logic inverted)
 */
void enable_resistence() {
    bcm2835_gpio_write(RESISTANCE, LOW);
}

/*!
 * @brief Function used to disable the resistence. (HIGH/LOW logic inverted)
 */
void disable_resistence() {
    bcm2835_gpio_write(RESISTANCE, HIGH);
}

/*!
 * @brief Function used to setup the initial state of the actuators.
 */
int setup_devices() {
    if (!bcm2835_init()) {
        fprintf(stderr, "Cannot setup the actuators devices.");
        raise(SIGABRT);
    }

    // Set pins as output
    bcm2835_gpio_fsel(VENTILATOR, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RESISTANCE, BCM2835_GPIO_FSEL_OUTP);

    // Turn down devices as first state
    disable_ventilator();
    disable_resistence();

    return BCM2835_SUCESS;
}

/*!
 * @brief Function used to handle program interruption, disable devices and exit.
 */
void handle_actuators_interruption() {
    disable_ventilator();
    disable_resistence();
    bcm2835_close();
}
