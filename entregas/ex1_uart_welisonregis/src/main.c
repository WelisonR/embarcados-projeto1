#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         // Used for UART
#include <fcntl.h>          // Used for UART
#include <termios.h>        // Used for UART
#define ASK_INT 0xA1
#define ASK_FLOAT 0xA2
#define ASK_STRING 0xA3
#define SEND_INT 0xB1
#define SEND_FLOAT 0xB2
#define SEND_STRING 0xB3

/* Global variables */
char matricula[5] = {4, 1, 2, 1, '\0'};
/* Global variables */

void clearOutputs() {
    #if defined _WIN32
        system("cls");
    #elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
        system("clear");
    #elif defined (__APPLE__)
        system("clear");
    #endif
}

int setupUART(const char *uart_path) {
    int uart0_filestream = -1;
    uart0_filestream = open(uart_path, O_RDWR | O_NOCTTY | O_NDELAY);

    if (uart0_filestream == -1) {
        printf("Erro - Não foi possível iniciar a UART.\n");
    }
    else {
        printf("UART inicializada!\n");
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

ssize_t writeUART(int fd, const void *buf, size_t count) {
    ssize_t message_size = write(fd, buf, count);
    if(message_size < 0) {
        printf("Houve um erro no envio da mensagem à UART!\n");
    } else {
        printf("%03d bytes escritos na UART!\n", message_size);
    }

    return message_size;
}

ssize_t readUART(int fd, void *buf, size_t count) {
    ssize_t response_size = read(fd, buf, count);
    if(response_size < 0) {
        printf("Houve um erro na leitura da mensagem da UART!\n");
    } else {
        printf("%03d bytes lidos da UART!\n", response_size);
    }

    return response_size;
}

void askIntData(int uart0_filestream) {
    unsigned char command[2] = {ASK_INT, '\0'};

    ssize_t message_size = writeUART(uart0_filestream, &command[0], strlen((char*)command));
    if(message_size < 0) return;
    message_size = writeUART(uart0_filestream, &matricula[0], strlen(matricula));
    if(message_size < 0) return;

    sleep(1);

    int response = -1;

    ssize_t response_size = readUART(uart0_filestream, (void*) &response, sizeof(int));
    if(response_size > 0) {
        printf("\nInteiro lido: %d\n", response);
    }
}

void askFloatData(int uart0_filestream) {
    unsigned char command[2] = {ASK_FLOAT, '\0'};

    ssize_t message_size = writeUART(uart0_filestream, &command[0], strlen((char*)command));
    if(message_size < 0) return;
    message_size = writeUART(uart0_filestream, &matricula[0], strlen(matricula));
    if(message_size < 0) return;

    sleep(1);

    float response = -1.0f;

    ssize_t response_size = readUART(uart0_filestream, (void*) &response, sizeof(float));
    if(response_size > 0) {
        printf("\nFloat lido: %f\n", response);
    }
}

void askStringData(int uart0_filestream) {
    unsigned char command[2] = {ASK_STRING, '\0'};
    unsigned char string_size = -1;

    ssize_t message_size = writeUART(uart0_filestream, &command[0], strlen((char*)command));
    if(message_size < 0) return;
    message_size = writeUART(uart0_filestream, &matricula[0], strlen(matricula));
    if(message_size < 0) return;

    sleep(1);

    ssize_t response_size = readUART(uart0_filestream, &string_size, sizeof(unsigned char));
    if(string_size <= 0) return;

    char *response = (char *) malloc((int) string_size+1);

    response_size = readUART(uart0_filestream, (void*) &response[0], string_size);
    if(response_size > 0) {
        response[string_size] = '\0';
        printf("\nString lida: %s\n", response);
    }

    free(response);
}

void sendIntData(int uart0_filestream, int typed_int) {
    unsigned char command[2] = {SEND_INT, '\0'};

    ssize_t message_size = writeUART(uart0_filestream, &command[0], strlen((char*)command));
    if(message_size < 0) return;
    message_size = writeUART(uart0_filestream, &typed_int, sizeof(int));
    if(message_size < 0) return;
    message_size = writeUART(uart0_filestream, &matricula[0], strlen(matricula));
    if(message_size < 0) return;

    sleep(1);

    int response = -1;

    ssize_t response_size = readUART(uart0_filestream, (void*) &response, sizeof(int));
    if(response_size > 0) {
        printf("\nInteiro recebido: %d\n", response);
    }
}

void sendFloatData(int uart0_filestream, float typed_float) {
    unsigned char command[2] = {SEND_FLOAT, '\0'};

    ssize_t message_size = writeUART(uart0_filestream, &command[0], strlen((char*)command));
    if(message_size < 0) return;
    message_size = writeUART(uart0_filestream, &typed_float, sizeof(float));
    if(message_size < 0) return;
    message_size = writeUART(uart0_filestream, &matricula[0], strlen(matricula));
    if(message_size < 0) return;

    sleep(1);

    float response = -1.0f;

    ssize_t response_size = readUART(uart0_filestream, (void*) &response, sizeof(float));
    if(response_size > 0) {
        printf("\nFloat recebido: %f\n", response);
    }
}

void sendStringData(int uart0_filestream, char *typed_string) {
    unsigned char command[2] = {SEND_STRING, '\0'};
    unsigned char typed_string_size = strlen(typed_string);

    ssize_t message_size = writeUART(uart0_filestream, &command[0], strlen((char*)command));
    if(message_size < 0) return;
    message_size = writeUART(uart0_filestream, &typed_string_size, sizeof(unsigned char));
    if(message_size < 0) return;
    message_size = writeUART(uart0_filestream, &typed_string[0], strlen((char*)typed_string));
    if(message_size < 0) return;
    message_size = writeUART(uart0_filestream, &matricula[0], strlen(matricula));
    if(message_size < 0) return;

    sleep(1);

    unsigned char response_string_size = -1;

    ssize_t response_size = readUART(uart0_filestream, &response_string_size, sizeof(unsigned char));
    if(response_string_size <= 0) return;

    char *response = (char *) malloc((int) (response_string_size+1));

    response_size = readUART(uart0_filestream, (void*) &response[0], response_string_size);
    if(response_size > 0) {
        response[response_string_size] = '\0';
        printf("\nString lida: %s\n", response);
    }

    free(response);
}

int displayMenuOptions() {
    int user_choice = -1;

    printf(" ___      _______  ___   _______  _______  ______      __   __  ____   \n"); 
    printf("|   |    |       ||   | |       ||       ||    _ |    |  | |  ||    | \n");
    printf("|   |    |    ___||   | |_     _||   _   ||   | ||    |  |_|  | |   | \n");
    printf("|   |    |   |___ |   |   |   |  |  | |  ||   |_||_   |       | |   | \n");
    printf("|   |___ |    ___||   |   |   |  |  |_|  ||    __  |  |       | |   | \n");
    printf("|       ||   |___ |   |   |   |  |       ||   |  | |   |     |  |   | \n");
    printf("|_______||_______||___|   |___|  |_______||___|  |_|    |___|   |___| \n");
    printf("\n");
    printf("======================================================================\n\n");
    printf("**********************************************************************\n");
    printf("***       Protocolos disponíveis para comunicação com a UART:      ***\n");
    printf("***         1. Solicitar um inteiro;                               ***\n");
    printf("***         2. Solicitar um float;                                 ***\n");
    printf("***         3. Solicitar uma string;                               ***\n");
    printf("***         4. Enviar um inteiro;                                  ***\n");
    printf("***         5. Enviar um float;                                    ***\n");
    printf("***         6. Enviar uma string;                                  ***\n");
    printf("***         0. Sair do programa.                                   ***\n");
    printf("**********************************************************************\n");
    printf("\n");
    printf(">> Digite uma opção: ");
    scanf("%d", &user_choice);

    return user_choice;
}


int main(int argc, const char * argv[]) {
    const char uart_path[] = "/dev/serial0";
    int user_choice = -1;
    int uart0_filestream = -1;

    while(user_choice != 0) {
        char wait[2];
        int typed_int;
        float typed_float;
        char typed_string[300];

        clearOutputs();
        user_choice = displayMenuOptions();

        switch(user_choice) {
            case 1:
                uart0_filestream = setupUART(uart_path);
                askIntData(uart0_filestream);
                close(uart0_filestream);
                break;
            case 2:
                uart0_filestream = setupUART(uart_path);
                askFloatData(uart0_filestream);
                close(uart0_filestream);
                break;
            case 3:
                uart0_filestream = setupUART(uart_path);
                askStringData(uart0_filestream);
                close(uart0_filestream);
                break;
            case 4:
                printf(">> Digite um inteiro: ");
                scanf("%d", &typed_int);
                uart0_filestream = setupUART(uart_path);
                sendIntData(uart0_filestream, typed_int);
                close(uart0_filestream);
                break;
            case 5:
                printf(">> Digite um float: ");
                scanf("%f", &typed_float);
                uart0_filestream = setupUART(uart_path);
                sendFloatData(uart0_filestream, typed_float);
                close(uart0_filestream);
                break;
            case 6:
                printf(">> Digite uma string: ");
                scanf(" %[^\n]s", typed_string);
                uart0_filestream = setupUART(uart_path);
                sendStringData(uart0_filestream, typed_string);
                close(uart0_filestream);
                break;
            default:
                break;
        }

        if(user_choice != 0) {
            printf("\n\n");
            printf(">> Pressione enter para continuar:\n");
            printf(">> ");
            scanf("%c%c", &wait[0], &wait[1]);
        }
    }
    
    return 0;
}