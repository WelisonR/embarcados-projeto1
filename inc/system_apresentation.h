#ifndef SYSTEM_APRESENTATION_H_
#define SYSTEM_APRESENTATION_H_

/* header includes */
#include <menu.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "system_monitor.h"

void setup_ncurses_initial_configs();
void display_text(WINDOW *win, int line, int column, char *text);
void display_text_center(WINDOW *win, int starty, int startx, int width, char *string, chtype color);
void setup_window_title(WINDOW *win, char *title);
void read_float(WINDOW *win, char *message);
void set_potentiometer_reference_temperature();
void set_keyboard_reference_temperature();
void set_hysteresis();
void setup_input_menu();
void setup_iterative_menu();
void* setup_menu_windows();
void clean_ncurses_alocation();
void init_system_apresentation(struct system_data *env_data);

#endif /* SYSTEM_APRESENTATION_H_ */
