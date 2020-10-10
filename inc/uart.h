#ifndef UART_H_
#define UART_H_

/* header includes */
#include <stdio.h>
#include <string.h>
#include <unistd.h>         // Used for UART
#include <fcntl.h>          // Used for UART
#include <termios.h>        // Used for UART

/* Definitions to UART communication */
#define UART_FAIL                   -1
#define UART_PATH                   "/dev/serial0"
#define ASK_INTERNAL_TEMPERATURE    0xA1
#define ASK_REFERENCE_TEMPERATURE   0xA2

/*!
 * @brief Function used to store in csv file the measured temperatures.
 *
 * @param[in]  uart_path     :   System path to open the uart enviroment.
 * @param[out] fd            :   File descriptor of uart connection.
 *
 */
int setup_uart(const char *uart_path);

/*!
 * @brief Closure of write function to track number of writed bytes.
 *
 * @param[in] fd                :   File descriptor of uart connection.
 * @param[in] buf               :   Content to write through communication.
 * @param[in] count             :   Content size to write.
 * @param[out] message_size     :   Size of the message writed through communication.
 *
 */
ssize_t write_uart(int fd, const void *buf, size_t count);

/*!
 * @brief Closure of read function to track number of read bytes.
 *
 * @param[in] fd                :   File descriptor of uart connection.
 * @param[in] buf               :   Content to red through communication.
 * @param[in] count             :   Content size to read.
 * @param[out] message_size     :   Size of the message read through communication.
 *
 */
ssize_t read_uart(int fd, void *buf, size_t count);
/*!
 * @brief Function used to ask float data via uart.
 *
 * @param[in] uart0_filestream       :   File descriptor of uart connection.
 * @param[in] command_type           :   Available commands via uart.
 * @param[out] response              :   Asked float value.
 *
 * command_type:
 * - ASK_INTERNAL_TEMPERATURE: ask the internal temperature of the system.
 * - ASK_REFERENCE_TEMPERATURE ask the reference temperature based on potentiometer.
 */
float ask_float_data(int uart0_filestream, int command_type);

/*!
 * @brief Function used to recover internal temperature and reference temperature.
 *
 * @param[in] command_type           :   Available commands via uart.
 * @param[out] response              :   Asked float value.
 *
 * command_type:
 * - ASK_INTERNAL_TEMPERATURE: ask the internal temperature of the system.
 * - ASK_REFERENCE_TEMPERATURE ask the reference temperature based on potentiometer.
 */
float uart(int command_type);

#endif /* UART_H_ */
