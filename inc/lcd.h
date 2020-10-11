
/*
*
* by Lewis Loflin www.bristolwatch.com lewis@bvu.net
* http://www.bristolwatch.com/rpi/i2clcd.htm
* Using wiringPi by Gordon Henderson
*
*
* Port over lcd_i2c.py to C and added improvements.
* Supports 16x2 and 20x4 screens.
* This was to learn now the I2C lcd displays operate.
* There is no warrenty of any kind use at your own risk.
*
*/

#ifndef LCD_H_
#define LCD_H_

/* header includes */
#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>

/* Definitions to I2C communication */
#define I2C_ADDR          0x27          // I2C device address
#define LCD_CHR           1             // Mode - Sending data
#define LCD_CMD           0             // Mode - Sending command
#define LINE1             0x80          // 1st line
#define LINE2             0xC0          // 2nd line
#define LCD_BACKLIGHT     0x08          // On
#define ENABLE            0b00000100    // Enable bit
// #define LCD_BACKLIGHT  0x00          // Off

/*!
 * @brief Function used to convert float to string and display on LCD.
 *
 * @param[in]  fd         :   File descriptor to establish connection via i2c.
 * @param[in]  value      :   Float value to write to LCD.
 *
 */
void write_float(int fd, float value);

/*!
 * @brief Function used to convert integer to string and display on LCD.
 *
 * @param[in]  fd         :   File descriptor to establish connection via i2c.
 * @param[in]  value      :   Integer value to write to LCD.
 *
 */
void write_int(int fd, int value);

/*!
 * @brief Function used to write char to LCD.
 *
 * @param[in]  fd         :   File descriptor to establish connection via i2c.
 * @param[in]  value      :   Char to write to LCD.
 *
 */
void write_char(int fd, char value);

/*!
 * @brief Function used to write strings to LCD.
 *
 * @param[in]  fd       :   File descriptor to establish connection via i2c.
 * @param[in]  str      :   String to write to LCD.
 *
 */
void write_string(int fd, const char *str);


/*!
 * @brief Clear LCD display and go to row 0 column 0.
 *
 * @param[in]  fd       :   File descriptor to establish connection via i2c.
 *
 */
void clear_lcd(int fd);

/*!
 * @brief Function used to set the cursor location.
 *
 * @param[in]  fd       :   File descriptor to establish connection via i2c.
 * @param[in]  line     :   line to set the cursor location.
 *
 * line:
 * - LINE1: 0.
 * - LINE2: 1.
 *
 */
void set_cursor_location(int fd, int line);

/*!
 * @brief Function used to send byte to data pins.
 *
 * @param[in]  fd       :   File descriptor to establish connection via i2c.
 * @param[in]  bits     :   Bits to be toggled.
 * @param[in]  mode     :   Operation mode (data or command)
 *
 * mode:
 * - LCD_CHR (data): 0.
 * - LCD_CHR (command): 1.
 *
 */
void lcd_byte(int fd, int bits, int mode);

/*!
 * @brief Function used to toggle the enable pin on LCD display.
 *
 * @param[in]  fd       :   File descriptor to establish connection via i2c.
 * @param[in]  bits     :   Bits to be toggled.
 *
 */
void lcd_toggle_enable(int fd, int bits);

/*!
 * @brief Function used to setup the initial states of the lcd display.
 *
 * @param[in]  fd     :   File descriptor to establish connection via i2c.
 *
 */
void lcd_init(int fd);

#endif /* LCD_H_ */
