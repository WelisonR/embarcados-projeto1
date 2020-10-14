#ifndef BCM2835_API_H_
#define BCM2835_API_H_

/* header includes */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

/* Definitions to bcm2835 */
#define BCM2835_SUCESS      1
#define BCM2835_FAIL        -1
#define VENTILATOR          RPI_V2_GPIO_P1_18
#define RESISTANCE          RPI_V2_GPIO_P1_16

/*!
 * @brief Function used to enable the ventilator. (HIGH/LOW logic inverted)
 */
void enable_ventilator();

/*!
 * @brief Function used to disable the ventilator. (HIGH/LOW logic inverted)
 */
void disable_ventilator();

/*!
 * @brief Function used to enable the resistence. (HIGH/LOW logic inverted)
 */
void enable_resistence();

/*!
 * @brief Function used to disable the resistence. (HIGH/LOW logic inverted)
 */
void disable_resistence();


/*!
 * @brief Function used to setup the initial state of the actuators.
 *
 * @param[out] bme2835_status       :       success or failure startup
 *
 * bme2835_status:
 * - BCM2835_FAIL: unexpected error when initializing.
 * - BCM2835_SUCESS: devices successfully initialized
 */
int setup_devices();

/*!
 * @brief Function used to handle program interruption, disable devices and exit.
 */
void handle_actuators_interruption();

#endif /* BCM2835_API_H_ */
