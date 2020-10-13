#include "system_apresentation.h"

/* Header definitions */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 4
#define ENTER_KEY 10

#define DISPLAY_WIDTH 60
#define HEADER_X 0
#define HEADER_Y 5
#define MENU_HEIGHT 16
#define INPUT_FIELD_HEIGHT 5

/* Global variables */
ITEM **list_items;
MENU *selection_menu;
WINDOW *main_menu_window;
WINDOW *float_input_window;

int user_input;
int choices_size;
char *selected_item_name;
char selected_item_value;
float value;

struct system_data *enviroment_data;

char *MENU_CHOICES[] = {
    "1. Set reference temperature as potentiometer.",
    "2. Set reference temperature from keyboard.",
    "3. Set system histeresis.",
    " ",
    "Press CTRL + C to stop the program!",
    (char *)NULL,
};

void setup_ncurses_initial_configs()
{
    /* Initialize curses */
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
}

void display_text(WINDOW *win, int line, int column, char *text)
{
    if (win == NULL)
        win = stdscr;
    wmove(win, line, column);
    wclrtoeol(win);
    mvwprintw(win, line, column, text);
    box(float_input_window, 0, 0);
}

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

void read_float(WINDOW *win, char *message)
{
    value = -1;

    echo();
    display_text(float_input_window, 3, 1, message);
    wscanw(float_input_window, "%f", &value);
    noecho();

    wrefresh(float_input_window);
}

void setup_input_menu()
{
    float_input_window = newwin(INPUT_FIELD_HEIGHT, DISPLAY_WIDTH, MENU_HEIGHT + 6, HEADER_X);
    setup_window_title(float_input_window, "Input do Usuário");
    keypad(float_input_window, TRUE);
    box(float_input_window, 0, 0);
    wrefresh(float_input_window);
}

void set_keyboard_reference_temperature()
{
    enviroment_data->reference_temperature_type = IS_KEYBOARD_REFERENCE;
    enviroment_data->reference_temperature = value;
}

void set_potentiometer_reference_temperature()
{
    enviroment_data->reference_temperature_type = IS_POTENTIOMETER_REFERENCE;
}

void set_hysteresis()
{
    enviroment_data->hysteresis = value;
}

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

    setup_window_title(main_menu_window, "Definição de Váriáveis do Sistema");

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
                read_float(float_input_window, " Type the reference temperature >> ");
                set_keyboard_reference_temperature();
            }
            else if (selected_item_value == '3')
            {
                read_float(float_input_window, " Type the hysteresis value >> ");
                set_hysteresis();
            }
            move(8, 1);
            pos_menu_cursor(selection_menu);
            break;
        }
        wrefresh(main_menu_window);
        refresh();
    }
}

void *setup_menu_windows()
{
    setup_input_menu();
    setup_iterative_menu();
}

void clean_ncurses_alocation()
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

void init_system_apresentation(struct system_data *env_data)
{
    enviroment_data = env_data;
    setup_ncurses_initial_configs();
}
