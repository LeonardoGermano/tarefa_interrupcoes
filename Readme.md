

# Projeto Matriz de LEDs WS2812 com Botões e LED Vermelho

Este projeto utiliza uma matriz de LEDs WS2812 (5x5), dois botões e um LED vermelho controlado pela BitDogLab. O código implementa a exibição de números de 0 a 9 na matriz de LEDs, onde o número pode ser incrementado e decrementado pressionando os botões. Além disso, um LED vermelho pisca a cada 200 ms, indicando o estado do sistema.

## Descrição

Este projeto foi desenvolvido utilizando C, aproveitando a capacidade da BitDogLab para controlar a matriz de LEDs WS2812. Ele inclui:

- **Matriz de LEDs WS2812** de 5x5, conectada ao **GPIO 7**, que exibe os números de 0 a 9.
- **Dois botões** para controlar a exibição dos números na matriz de LEDs (incremento e decremento).
- **LED vermelho separado**, conectado ao pino **GPIO 13**, que pisca a cada 200ms.

A comunicação com os LEDs WS2812 é feita utilizando o protocolo de controle específico, configurado e manipulado por meio da biblioteca `pio` da Raspberry Pi.

## Funcionalidades

- **Botão A (GPIO 5)**: Incrementa o número exibido na matriz de LEDs.
- **Botão B (GPIO 6)**: Decrementa o número exibido na matriz de LEDs.
- **LED Vermelho (GPIO 13)**: Pisca a cada 200ms como sinal de funcionamento.

## Requisitos

- BitDogLab.
- Matriz de LEDs WS2812 de 5x5 conectada ao **GPIO 7**.
- Botões conectados aos pinos GPIO 5 e GPIO 6.
- LED vermelho conectado ao pino GPIO 13.

## Estrutura do Código

1. **Inicialização dos pinos**:
   - Pinos para os botões e LED vermelho são configurados.
   - A matriz de LEDs WS2812 é inicializada utilizando o protocolo SPI para comunicação no pino GPIO 7.

2. **Funções de controle**:
   - **Funções de interrupção** para detectar pressionamentos de botão e realizar ações sem bloquear o código principal.
   - **Função de exibição de número** que mapeia os números de 0 a 9 para uma matriz 5x5 de LEDs.

3. **Função de piscar o LED vermelho**:
   - O LED vermelho pisca a cada 200ms para indicar que o sistema está ativo.

## Vídeos

Aqui estão dois vídeos para demonstrar o funcionamento do código, tanto na placa real quanto no ambiente de desenvolvimento Wokwi VS Code.

### 1. Código rodando na placa BitDogLab
![Vídeo 1](https://youtu.be/1-Gqy6KrePk)

### 2. Código rodando no Wokwi VS Code
![Vídeo 2](https://youtu.be/DZTkCRmFc2w)

## Como Rodar

### Na Placa Real:
1. Carregue o código na BitDogLab utilizando o ambiente de desenvolvimento de sua preferência (como o Visual Studio Code com a extensão para a Raspberry Pi Pico).
2. Conecte os botões e a matriz de LEDs corretamente à placa, com a matriz conectada ao **GPIO 7**.
3. Execute o código na placa e observe a matriz de LEDs funcionando.

## Contribuições

Contribuições são bem-vindas! Sinta-se à vontade para criar um **pull request** ou abrir uma **issue** para melhorias ou sugestões.

