#ifndef SYSTEM_MONITOR_H_
#define SYSTEM_MONITOR_H_

/* header includes */
#include <stdio.h>
#include <time.h>
/* header includes */

/* Structure that stores the system temperature */
struct system_temperature
{
    // Internal temperature (°C);
    float internal;
    // External temperature (°C);
    float external;
    // Reference temperature (°C) - defined from terminal or potentiometer.
    float reference;
};

/*!
 * @brief This API used to save the current date (DD-MM-YYYY) into a string.
 *
 * @param[in, out] formatted_date    :   string with formatted hour.
 *
 */
void set_current_formatted_date(char *formatted_date);


/*!
 * @brief This API used to save the current hour (HH:MM:SS) into a string.
 *
 * @param[in, out] fomatted_hour    :   string with formatted hour.
 *
 */
void set_current_formatted_hour(char *formatted_hour);

/*!
 * @brief Function used to store in csv file the measured temperatures.
 *
 * @param[in] system_temperature    :   Structure instance of system_temperature.
 * @param[out] csv file             :   Store temperature data into a csv file.
 *
 */
void store_temperature_data(struct system_temperature *temperatures);

#endif /* SYSTEM_MONITOR_H_ */