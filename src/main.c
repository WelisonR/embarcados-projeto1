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
    struct system_data enviroment_data;
    
    /* Setup lcd display  */
    if (wiringPiSetup () == -1) exit (1);
    int lcd_file_descriptor = wiringPiI2CSetup(I2C_ADDR);
    lcd_init(lcd_file_descriptor);


    return 0;
}
