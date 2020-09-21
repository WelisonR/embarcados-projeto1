# Exercício 2 - Comunicação I2C

Neste trabalho utilizou-se do protocolo de comunicação I2C com a linguagem C para comunicar a placa Raspberry Pi com um Sensor de Pressão, Umidade e Temperatura modelo BME280.

*   Maiores informações sobre os requisitos do exercício podem ser encontradas no arquivo PDF [exercicio_2_I2C_enunciado](https://github.com/WelisonR/embarcados/blob/master/entregas/ex2_bme280_welisonregis/exercicio_2_I2C_enunciado.pdf) na pasta principal do projeto.

## Execução

Para executar o projeto, prossiga com as instruções:

1. Na pasta principal com o código em C, digite o comando:

```sh
make
```

2. Ainda em `/c`, execute o programa:

```
bin/bin /dev/i2c-1
```

Observações:
*   Para interromper a execução, proceda com `CTRL+C`.

## Autor

*   Estudante: Welison Lucas Almeida Regis.
*   Matrícula: 17/0024121.