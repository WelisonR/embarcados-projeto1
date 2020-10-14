# Controle de Temperatura em Ambiente Fechado

**Disciplina**: Fundamentos de Sistemas Embarcados (UnB) - Professor Renato Coral.<br>
**Tópicos**: Controle de temperatura, integração entre dispositivos, arduino, raspberry, GPIO, UART, I2C.<br>

## Sobre o Projeto 01

O trabalho tem por objetivo a realização do **controle de temperatura de um sistema através de dois dispositivos — um resistor de 15 Watts e uma ventoinha** que puxa o ar externo (a temperatura ambiente) para reduzir a temperatura. Define-se uma temperatura de referência por meio de potenciômetro ou entrada no terminal e, acrescido de um valor de hísterese, controla-se a temperatura interna do sistema com base nas premissas abaixo:

*   Se temperatura_interna < temperatura_referencia - hísterese/2.0, então liga a resistência e desliga o ventilador.
*   Se temperatura_interna > temperatura_referencia + hístereses/2.0, então desliga a resistência e liga o ventilador.

Maiores informações e requisitos do trabalho disponíveis em:
*   [Gitlab - FSE - Projeto 1](https://gitlab.com/fse_fga/projetos/projeto-1).

## Demonstração do Projeto

![Demonostração do programa](./assets/program-execution.gif)

*Demonstração realizada as 16h03 em 14/10/2020 durante uma live da disciplina. Pequeno delay associado.*

## Execução do Projeto

Para executar o projeto, faça o clone do repositório e, na pasta principal, proceda com os comandos:

*   Compilar: `make`.
*   Executar: `bin/bin`.

Observações:
*   Seguir configuração do ambiente físico descrito no [trabalho 01](https://gitlab.com/fse_fga/projetos/projeto-1).
*   Instalação das bibliotecas necessárias da raspberry, arduino e demais descritas no Makefile.
*   Aumentar a janela do terminal em caso de dificuldades de visualização da interface.

## Instruções de Uso

O menu é seccionado em três grandes regiões: informações sobre o sistema; menu de opções; e entradas ou saídas do usuário ou sistema.

No menu de opções, pode-se optar pelos itens abaixo:

*   1. Definir temperatura de referência como potenciômetro.
    *   **Comando**: pressione enter.
*   2. Definir temperatura de referência através do teclado:
    *   **Comando**: pressione enter e digite o valor de ponto flutuante.
*   3. Definir o valor da hísterese:
    *   **Comando**: pressione enter e digite o valor de ponto flutuante.

## Autor

|Matrícula | Estudante |
| -- | -- |
| 17/0024121  |  Welison Lucas Almeida Regis |

## Referências

*   [Driver da Bosh para o sensor BME280](https://github.com/BoschSensortec/BME280_driver)
*   [Driver do controle do LCD 16x2 em C](http://www.bristolwatch.com/rpi/i2clcd.htm)
*   [Descrição do trabalho prático 01](https://gitlab.com/fse_fga/projetos/projeto-1)