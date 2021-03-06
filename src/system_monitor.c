#include "system_monitor.h"
#include "lcd.h"

/*!
 * @brief This API used to save the current date (DD-MM-YYYY) into a string.
 */
void set_current_formatted_date(char *formatted_date) {
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    sprintf(formatted_date, "%02d-%02d-%04d",
        timeinfo->tm_mday, timeinfo->tm_mon+1, 1900+timeinfo->tm_year);
}

/*!
 * @brief This API used to save the current hour (HH:MM:SS) into a string.
 */
void set_current_formatted_hour(char *formatted_hour) {
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    sprintf(formatted_hour, "%02d:%02d:%02d",
        timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

/*!
 * @brief This API used to store in csv file the datetime with temperatures.
 */
void store_temperature_data(struct system_data *temperatures) {
    char filepath[] = "data/temperature_monitor.csv";
    FILE *fp = fopen(filepath,"a+");
    fseek (fp, 0, SEEK_END);
    
    /* If file is not defined, return */
    if(fp == NULL) {
        // printf(">> Não foi possível salvar a média dos dados medidos.\n");
        return;
    }

    /* Add header if file is empty */
    if(ftell(fp) == 0) {
        fprintf(fp, "Data e Hora (DD-MM-YYYY HH:MM:SS), TI (°C), TE (°C), TR (°C)\n");
    }

    /* Recover data as DD-MM-YYYY */
    const int date_size = 11;
    char formatted_date[date_size];
    set_current_formatted_date(formatted_date);

    /* Recover current hour as HH:MM:SS */
    const int hour_size = 9;
    char formatted_hour[hour_size];
    set_current_formatted_hour(formatted_hour);

    /* Store datetime, external temperature, internal temperature and reference temperature */
    fprintf(fp, "%s %s, %0.2lf, %0.2lf, %0.2lf\n",
        formatted_date, formatted_hour, temperatures->internal_temperature,
        temperatures->external_temperature, temperatures->reference_temperature);

    // printf(">> Temperaturas salvas em %s.\n", filepath);
    fclose(fp);
}

void display_temperatures_lcd(int file_descriptor, struct system_data *temperature) {
    char lcd_line1[17], lcd_line2[17];

    sprintf(lcd_line1, "TI%.2f TE%.2f",
        temperature->internal_temperature, temperature->external_temperature);
    sprintf(lcd_line2, "TR%.2f", temperature->reference_temperature);

    clear_lcd(file_descriptor);
    set_cursor_location(file_descriptor, LINE1);
    write_string(file_descriptor, lcd_line1);
    set_cursor_location(file_descriptor, LINE2);
    write_string(file_descriptor, lcd_line2);
}
