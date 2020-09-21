# Exercício 2 - Comunicação I2C

Neste trabalho utilizou-se do protocolo de comunicação I2C com a linguagem C para comunicar a placa Raspberry Pi com um Sensor de Pressão, Umidade e Temperatura modelo BME280.

## Execução

Para executar o projeto, prossiga:

1. Na pasta com o código C (`/c`), digite: `make`.
2. Em `/c`, execute o código: `bin/bin /dev/i2c-1`.

*   A cada dez medidas realizadas, será armazenada em um arquivo csv em `/data` a média da temperatura, pressão e umidade.