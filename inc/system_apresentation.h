#ifndef SYSTEM_APRESENTATION_H_
#define SYSTEM_APRESENTATION_H_

/* header includes */
#include <menu.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "system_monitor.h"

/*!
 * @brief Function used to setup ncurses (display) initial configurations.
 */
void setup_ncurses_initial_configs();

/*!
 * @brief Function used to display text into a especific window at line, column.
 *
 * @param[in] window                  :   Window instance of ncurses.
 * @param[in] line                    :   Start line to display text.
 * @param[in] column                  :   Start column to display text.
 * @param[in] text                    :   Text to be displayed.
 * @param[out] display                :    Display text into window.
 *
 */
void display_text(WINDOW *win, int line, int column, char *text);

/*!
 * @brief Function used to display centered text into a especific window at line, column.
 *
 * @param[in] window                    :   Window instance of ncurses.
 * @param[in] starty                    :   Y coordinate of pair (X, Y)
 * @param[in] startx                    :   X coordinate of pair (X, Y)
 * @param[in] width                     :   Width to be centralized
 * @param[in] string                    :   Text to be displayed.
 * @param[in] color                     :   Color of background and foreground.
 * @param[out] display                  :   Display text into window.
 *
 */
void display_text_center(WINDOW *win, int starty, int startx, int width, char *string, chtype color);

/*!
 * @brief Function used to setup the title in header of a window.
 *
 * @param[in] window                    :   Window instance of ncurses.
 * @param[in] title                     :   Header menu title.
 * @param[out] display                  :   Display text into header window.
 *
 */
void setup_window_title(WINDOW *win, char *title);

/*!
 * @brief Function used to read float data with a specific message.
 *
 * @param[in] window                      :   Window instance of ncurses.
 * @param[in] message                     :   Message to be displayed to read float.
 * @param[out] display                    :   Display text into header window.
 *
 */
void read_float(WINDOW *win, char *message);

/*!
 * @brief Function used to set user input as reference temperature (IS_KEYBOARD_REFERENCE)
 */
void set_keyboard_reference_temperature();

/*!
 * @brief Function used to set potentiometer as reference temperature. (IS_POTENTIOMETER_REFERENCE)
 */
void set_potentiometer_reference_temperature();

/*!
 * @brief Function used to set a float value to system hysteresis.
 */
void set_hysteresis();

/*!
 * @brief Function used to read float data with a specific message.
 *
 * @param[out] display               :   Display input (float data) window with border.
 *
 */
void setup_input_menu();

/*!
 * @brief Function used to setup the system status area.
 *
 * @param[out] display    :   Display window with system variables information.
 *
 */
void setup_system_status();

/*!
 * @brief Function used to read float data with a specific message.
 *
 * @param[out] display               :   Display input (menu selection) window with border.
 *
 */
void setup_iterative_menu();

/*!
 * @brief Closure function to setup_input_menu and setup_iterative_menu. (thread)
 */
void* setup_menu_windows();

/*!
 * @brief Function used to display the system variables values.
 *
 * @param[out] display    :   Display system variables values into screen.
 *
 */
void update_system_status_window();

/*!
 * @brief Closure function to setup_system_status and update_system_status_window. (thread)
 */
void* setup_system_status_interface();

/*!
 * @brief Clean all resources allocated by ncurses.
 */
void clean_ncurses_alocation();

/*!
 * @brief Setup initial configurations to ncurses and set pointer to enviroment_data.
 */
void init_system_apresentation(struct system_data *env_data);

#endif /* SYSTEM_APRESENTATION_H_ */
