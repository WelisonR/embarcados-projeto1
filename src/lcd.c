
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

#include "lcd.h"

/*!
 * @brief Function used to convert float to string and display on LCD.
 */
void write_float(int fd, float value)   {
  char buffer[20];
  sprintf(buffer, "%4.2f",  value);
  write_string(fd, buffer);
}

/*!
 * @brief Function used to convert integer to string and display on LCD.
 */
void write_int(int fd, int value)   {
  char array1[20];
  sprintf(array1, "%d",  value);
  write_string(fd, array1);
}

/*!
 * @brief Function used to write char to LCD.
 */
void write_char(int fd, char value)   {
  lcd_byte(fd, value, LCD_CHR);
}

/*!
 * @brief Function used to write strings to LCD.
 */
void write_string(int fd, const char *str)   {
  while ( *str ) {
    lcd_byte(fd, *(str++), LCD_CHR);
  }
}

/*!
 * @brief Clear LCD display and go to row 0 column 0.
 */
void clear_lcd(int fd)   {
  lcd_byte(fd, 0x01, LCD_CMD);
  lcd_byte(fd, 0x02, LCD_CMD);
}

/*!
 * @brief Function used to set the cursor location.
 */
void set_cursor_location(int fd, int line)   {
  lcd_byte(fd, line, LCD_CMD);
}

/*!
 * @brief Function used to send byte to data pins.
 */
void lcd_byte(int fd, int bits, int mode)   {
  int bits_high;
  int bits_low;
  // uses the two half byte writes to LCD
  bits_high = mode | (bits & 0xF0) | LCD_BACKLIGHT ;
  bits_low = mode | ((bits << 4) & 0xF0) | LCD_BACKLIGHT ;

  // High bits
  wiringPiI2CReadReg8(fd, bits_high);
  lcd_toggle_enable(fd, bits_high);

  // Low bits
  wiringPiI2CReadReg8(fd, bits_low);
  lcd_toggle_enable(fd, bits_low);
}

/*!
 * @brief Function used to toggle the enable pin on LCD display.
 */
void lcd_toggle_enable(int fd, int bits)   {
  delayMicroseconds(500);
  wiringPiI2CReadReg8(fd, (bits | ENABLE));
  delayMicroseconds(500);
  wiringPiI2CReadReg8(fd, (bits & ~ENABLE));
  delayMicroseconds(500);
}

/*!
 * @brief Function used to setup the initial states of the lcd display.
 */
void lcd_init(int fd)   {
  lcd_byte(fd, 0x33, LCD_CMD); // Initialise
  lcd_byte(fd, 0x32, LCD_CMD); // Initialise
  lcd_byte(fd, 0x06, LCD_CMD); // Cursor move direction
  lcd_byte(fd, 0x0C, LCD_CMD); // 0x0F On, Blink Off
  lcd_byte(fd, 0x28, LCD_CMD); // Data length, number of lines, font size
  lcd_byte(fd, 0x01, LCD_CMD); // Clear display
  delayMicroseconds(500);
}
