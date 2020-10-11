#include "bcm2835_api.h"

/*!
 * @brief Function used to enable a specific device.
 */
void enable_device(int device) {
    bcm2835_gpio_write(device, LOW);
}

/*!
 * @brief Function used to disable a specific device.
 */
void disable_device(int device) {
    bcm2835_gpio_write(device, HIGH);
}

/*!
 * @brief Function used to setup the initial state of the actuators.
 */
int setup_devices() {
    if (!bcm2835_init()) {
        fprintf(stderr, "Cannot setup the actuators devices.");
        return BCM2835_FAIL;
    }

    // Set pins as output
    bcm2835_gpio_fsel(VENTILATOR, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RESISTANCE, BCM2835_GPIO_FSEL_OUTP);

    // Turn down devices as first state
    disable_device(VENTILATOR);
    disable_device(RESISTANCE);

    return BCM2835_SUCESS;
}

/*!
 * @brief Function used to handle program interruption, disable devices and exit.
 */
void handle_interruption(int signal) {
    printf("Signal: %d. Shutting down...\n", signal);
    disable_device(VENTILATOR);
    disable_device(RESISTANCE);
    bcm2835_close();
    exit(0);
}
