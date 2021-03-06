#include "system_windows.h"

/* Header definitions */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 4
#define ENTER_KEY 10

#define DISPLAY_WIDTH COLS
#define HEADER_X 0
#define HEADER_Y 9
#define SYSTEM_STATUS_HEIGHT 9
#define MENU_HEIGHT 10
#define INPUT_FIELD_HEIGHT 5

#define MAX_SYSTEM_TEMPERATURE 100.0f
#define MIN_SYSTEM_TEMPERATURE 10.0f

#define MIN_SYSTEM_HYSTERESIS 0.0f
#define MAX_SYSTEM_HYSTERESIS 100.0f

/* Global variables */
ITEM **list_items;
MENU *selection_menu;
WINDOW *main_menu_window;
WINDOW *float_input_window;
WINDOW *system_status_window;

int user_input;
int choices_size;
char *selected_item_name;
char selected_item_value;

struct system_data *system_display_value = NULL;

char *MENU_CHOICES[] = {
    "1. Set reference temperature as potentiometer.",
    "2. Set reference temperature from keyboard.",
    "3. Set system histeresis.",
    " ",
    "Press CTRL + C to stop the program!",
    (char *)NULL,
};

/*!
 * @brief Function used to setup ncurses (display) initial configurations.
 */
void setup_ncurses_initial_configs()
{
    /* Initialize curses */
    initscr();
    start_color();
    cbreak();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
}

/*!
 * @brief Function used to display text into a especific window at line, column.
 */
void display_text(WINDOW *win, int line, int column, char *text)
{
    if (win == NULL)
        win = stdscr;
    wmove(win, line, column);
    wclrtoeol(win);
    mvwprintw(win, line, column, text);
    wrefresh(win);
    box(win, 0, 0);
    wrefresh(win);
}

/*!
 * @brief Function used to display centered text into a especific window at line, column.
 */
void display_text_center(WINDOW *win, int starty, int startx, int width, char *string, chtype color)
{
    int length, x, y;
    float temp;

    if (win == NULL)
        win = stdscr;
    getyx(win, y, x);
    if (startx != 0)
        x = startx;
    if (starty != 0)
        y = starty;
    if (width == 0)
        width = 80;

    length = strlen(string);
    temp = (width - length) / 2;
    x = startx + (int)temp;
    wattron(win, color);
    mvwprintw(win, y, x, "%s", string);
    wattroff(win, color);
    refresh();
}

/*!
 * @brief Function used to setup the title in header of a window.
 */
void setup_window_title(WINDOW *win, char *title)
{
    /* Print a border around the main window and print a title */
    box(win, 0, 0);
    display_text_center(win, 1, 0, DISPLAY_WIDTH, title, COLOR_PAIR(1));
    mvwaddch(win, 2, 0, ACS_LTEE);
    mvwhline(win, 2, 1, ACS_HLINE, DISPLAY_WIDTH - 2);
    mvwaddch(win, 2, DISPLAY_WIDTH - 1, ACS_RTEE);
    refresh();
}

/*!
 * @brief Function used to read float data with a specific message.
 */
float read_float(WINDOW *win, char *message, float minimum_value, float maximum_value)
{
    float value = -1;
    char error_message[50];

    echo();
    display_text(float_input_window, 3, 1, message);
    wscanw(float_input_window, "%f", &value);
    noecho();
    wrefresh(float_input_window);

    if(value < minimum_value || value > maximum_value) {
        sprintf(error_message, " >> Valid interval: %.2f - %.2f", minimum_value, maximum_value);
        display_text(float_input_window, 3, 1, error_message);
        return -1;
    }

    return value;
}

/*!
 * @brief Function used to set user input as reference temperature (IS_KEYBOARD_REFERENCE)
 */
void set_keyboard_reference_temperature(float value)
{
    system_display_value->reference_temperature_type = IS_KEYBOARD_REFERENCE;
    system_display_value->reference_temperature = value;
}

 /*!
 * @brief Function used to set potentiometer as reference temperature. (IS_POTENTIOMETER_REFERENCE)
 */
void set_potentiometer_reference_temperature()
{
    system_display_value->reference_temperature_type = IS_POTENTIOMETER_REFERENCE;
}

/*!
 * @brief Function used to set a float value to system hysteresis.
 */
void set_hysteresis(float value)
{
    system_display_value->hysteresis = value;
}

/*!
 * @brief Function used to read float data with a specific message.
 *
 * @param[out] display               :   Display input (float data) window with border.
 *
 */
void setup_input_menu()
{
    float_input_window = newwin(INPUT_FIELD_HEIGHT, DISPLAY_WIDTH, MENU_HEIGHT + HEADER_Y, HEADER_X);
    setup_window_title(float_input_window, "System Inputs/Outputs");
    keypad(float_input_window, TRUE);
    box(float_input_window, 0, 0);
    wrefresh(float_input_window);
}

/*!
 * @brief Function used to setup the system status area.
 *
 * @param[out] display    :   Display window with system variables information.
 *
 */
void setup_system_status()
{
    system_status_window = newwin(SYSTEM_STATUS_HEIGHT, COLS, 0, 0);
    setup_window_title(system_status_window, "System Data Values");
    box(system_status_window, 0, 0);
    wrefresh(system_status_window);
}

/*!
 * @brief Function used to read float data with a specific message.
 */
void setup_iterative_menu()
{
    /* Create items */
    choices_size = ARRAY_SIZE(MENU_CHOICES);
    list_items = (ITEM **)calloc(choices_size, sizeof(ITEM *));
    for (int i = 0; i < choices_size; ++i)
    {
        /* empty description, first char is the command type */
        list_items[i] = new_item(MENU_CHOICES[i], "");
    }
    /* Empty row and exit row are not selectable */
    item_opts_off(list_items[3], O_SELECTABLE);
    item_opts_off(list_items[4], O_SELECTABLE);

    /* Crate menu */
    selection_menu = new_menu((ITEM **)list_items);

    /* Create the window to be associated with the menu */
    main_menu_window = newwin(MENU_HEIGHT, DISPLAY_WIDTH, HEADER_Y, HEADER_X);
    keypad(main_menu_window, TRUE);

    WINDOW *iterative_menu = subwin(main_menu_window, MENU_HEIGHT - 4, DISPLAY_WIDTH - 1, HEADER_Y + 4, HEADER_X + 1);

    /* Set main window and sub window */
    set_menu_win(selection_menu, main_menu_window);
    set_menu_sub(selection_menu, iterative_menu);
    refresh();

    /* Set menu mark to the string " * " */
    set_menu_mark(selection_menu, " * ");

    setup_window_title(main_menu_window, "Definition of System Variables");

    /* Post the menu */
    post_menu(selection_menu);
    wrefresh(main_menu_window);

    /* Execute until CTRL + C */
    while (1)
    {
        user_input = wgetch(main_menu_window);

        switch (user_input)
        {
        case KEY_DOWN:
            menu_driver(selection_menu, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(selection_menu, REQ_UP_ITEM);
            break;
        case ENTER_KEY:
            selected_item_name = (char *)item_name(current_item(selection_menu));
            selected_item_value = *selected_item_name;
            if (selected_item_value == '1')
            {
                display_text(float_input_window, 3, 1, " Potentiometer as reference temperature!");
                wrefresh(float_input_window);
                set_potentiometer_reference_temperature();
            }
            else if (selected_item_value == '2')
            {
                float temp = read_float(float_input_window, " Type the reference temperature >> ",
                    MIN_SYSTEM_TEMPERATURE, MAX_SYSTEM_TEMPERATURE);
                if(temp != -1) {
                    set_keyboard_reference_temperature(temp);
                }
            }
            else if (selected_item_value == '3')
            {
                float temp = read_float(float_input_window, " Type the hysteresis value >> ",
                    MIN_SYSTEM_HYSTERESIS, MAX_SYSTEM_HYSTERESIS);
                if(temp != -1) {
                    set_hysteresis(temp);
                }
            }
            move(13, 1);
            pos_menu_cursor(selection_menu);
            break;
        }
        wrefresh(main_menu_window);
        refresh();
    }
}

/*!
 * @brief Function used to display the system variables values.
 *
 * @param[out] display    :   Display system variables values into screen.
 *
 */
void update_system_status_window()
{
    char external_temperature_str[40], internal_temperature_str[40], reference_temperature_str[40];
    char reference_temperature_type_str[50];
    char system_hysteresis_str[40];

    while(1) {
        if(system_display_value->reference_temperature_type == IS_POTENTIOMETER_REFERENCE) {
            sprintf(reference_temperature_type_str, "Reference temperature origin: POTENTIOMETER.");
        }
        else {
            sprintf(reference_temperature_type_str, "Reference temperature origin: KEYBOARD.");
        }
        sprintf(reference_temperature_str, "Reference temperature: %.2f °C.",
            system_display_value->reference_temperature);
        sprintf(internal_temperature_str, "Internal temperature: %.2f °C.",
            system_display_value->internal_temperature);
        sprintf(external_temperature_str, "External temperature: %.2f °C.",
            system_display_value->external_temperature);
        sprintf(system_hysteresis_str, "System hysteresis: %.2f °C.",
            system_display_value->hysteresis);

        display_text(system_status_window, 3, 4, reference_temperature_type_str);
        display_text(system_status_window, 4, 4, reference_temperature_str);
        display_text(system_status_window, 5, 4, internal_temperature_str);
        display_text(system_status_window, 6, 4, external_temperature_str);
        display_text(system_status_window, 7, 4, system_hysteresis_str);
        wrefresh(system_status_window);
        usleep(500000);
    }
}

/*!
 * @brief Closure function to setup_input_menu and setup_iterative_menu. (thread)
 */
void *setup_menu_windows()
{
    while(1) {
        setup_input_menu();
        setup_iterative_menu();
    }
}

/*!
 * @brief Closure function to setup_system_status and update_system_status_window. (thread)
 */
void* setup_system_status_interface() {
    while(1) {
        setup_system_status();
        update_system_status_window();
    }
}

/*!
 * @brief Clean all resources allocated by ncurses.
 */
void clear_ncurses_alocation()
{
    /* Unpost and free all the memory taken up */
    unpost_menu(selection_menu);
    for (int i = 0; i < choices_size; ++i)
    {
        free_item(list_items[i]);
    }
    free_menu(selection_menu);

    delwin(main_menu_window);
    delwin(float_input_window);

    endwin();
}

/*!
 * @brief Setup initial configurations to ncurses and set pointer to system_display_value.
 */
void init_system_apresentation(struct system_data *env_data)
{
    system_display_value = env_data;
    setup_ncurses_initial_configs();
}
