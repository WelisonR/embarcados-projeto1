#include "uart_api.h"

/*!
 * @brief Function used to store in csv file the measured temperatures.
 */
int setup_uart(const char *uart_path) {
    int uart0_filestream = open(uart_path, O_RDWR | O_NOCTTY | O_NDELAY);

    if (uart0_filestream == -1) {
        // printf("Erro - Não foi possível iniciar a UART.\n");
        return UART_FAIL;
    }
    else {
        // printf("UART inicializada!\n");
    }

    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    return uart0_filestream;
}

/*!
 * @brief Closure of write function to track number of writed bytes.
 */
ssize_t write_uart(int fd, const void *buf, size_t count) {
    ssize_t message_size = write(fd, buf, count);
    if(message_size < 0) {
        // printf("Houve um erro no envio da mensagem à UART!\n");
    } else {
        // printf("%03d bytes escritos na UART!\n", message_size);
    }

    return message_size;
}

/*!
 * @brief Closure of read function to track number of read bytes.
 */
ssize_t read_uart(int fd, void *buf, size_t count) {
    ssize_t response_size = read(fd, buf, count);
    if(response_size < 0) {
        // printf("Houve um erro na leitura da mensagem da UART!\n");
    } else {
        // printf("%03d bytes lidos da UART!\n", response_size);
    }

    return response_size;
}

/*!
 * @brief Function used to ask float data via uart.
 */
float ask_float_data(int uart0_filestream, int command_type) {
    char matricula[5] = {4, 1, 2, 1, '\0'};
    unsigned char command[2] = {command_type, '\0'};

    ssize_t message_size = write_uart(uart0_filestream, &command[0], strlen((char*)command));
    if(message_size < 0) return (float) UART_FAIL;
    message_size = write_uart(uart0_filestream, &matricula[0], strlen(matricula));
    if(message_size < 0) return (float) UART_FAIL;

    usleep(30000);

    float response = (float) UART_FAIL;
    message_size = read_uart(uart0_filestream, (void*) &response, sizeof(float));
    if(message_size < 0) return (float) UART_FAIL;

    return response;
}

/*!
 * @brief Function used to recover internal temperature and reference temperature.
 */
float uart(int command_type) {
    float response = -1.0f;
    int uart0_filestream = setup_uart(UART_PATH);

    if(uart0_filestream == UART_FAIL) return (float) UART_FAIL;
    response = ask_float_data(uart0_filestream, command_type);

    close(uart0_filestream);

    return response;
}
