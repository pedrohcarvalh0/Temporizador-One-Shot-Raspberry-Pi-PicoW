# Projeto: Sequência de LEDs com Raspberry Pi Pico

Este projeto demonstra o uso de interrupções, temporizadores e callbacks na plataforma Raspberry Pi Pico para implementar o controle sequencial de LEDs por meio de um botão.

## Descrição Geral

Ao pressionar o botão conectado ao pino configurado como `BTN_PIN` (pino 5), o sistema acende os três LEDs (vermelho, azul e verde) simultaneamente. Em seguida, uma rotina de callback do temporizador é acionada a cada 3 segundos (3000 ms), desligando um LED por vez, seguindo a ordem definida. Durante o processo, o botão fica bloqueado, impedindo que uma nova sequência seja iniciada até que o último LED seja desligado.

## Requisitos Atendidos

1. **Acionamento dos LEDs**:  
   - Ao pressionar o botão, os três LEDs são ligados (nível alto).

2. **Sequência de Desligamento**:  
   - Após 3 segundos, a rotina de callback (`turn_off_callback`) é acionada, desligando um LED.
   - A cada 3 segundos subsequentes, outro LED é desligado, passando de três LEDs acesos para dois, depois para um e, por fim, todos desligados.

3. **Uso de Callbacks**:  
   - As mudanças de estado dos LEDs são implementadas em funções de callback do temporizador (usando `add_alarm_in_ms`), garantindo a execução após o intervalo de tempo definido.

4. **Bloqueio do Botão Durante a Sequência**:  
   - O botão só pode alterar o estado dos LEDs quando a sequência de desligamento tiver sido concluída.  
   - Durante a execução das rotinas de temporização, uma variável de bloqueio (`button_block`) impede que novas interrupções iniciem outra sequência.

## Componentes e Configurações

- **Pinos Utilizados**:
  - `RED_PIN_LED` (pino 13): LED vermelho.
  - `BLUE_PIN_LED` (pino 12): LED azul.
  - `GREEN_PIN_LED` (pino 11): LED verde.
  - `BTN_PIN` (pino 5): Botão de acionamento.

- **Bibliotecas Utilizadas**:
  - `pico/stdlib.h`: Funções básicas da SDK Pico.
  - `hardware/timer.h` e `hardware/clocks.h`: Funções para configuração e manipulação de temporizadores e relógios.

## Estrutura do Código

### 1. Inicialização dos Pinos

A função `setup()` é responsável por configurar:
- Os LEDs: Inicializados como saídas e iniciados com nível baixo (LEDs desligados).
- O Botão: Configurado como entrada com resistor de pull-up.

### 2. Tratamento da Interrupção do Botão

A interrupção é configurada para o pino do botão (`BTN_PIN`) na borda de descida (GPIO_IRQ_EDGE_FALL).  
A função `gpio_irq_handler` realiza as seguintes ações:
- Verifica o tempo decorrido (debounce de 300 ms) e se o botão não está bloqueado.
- Liga os três LEDs.
- Agenda a primeira chamada do callback `turn_off_callback` para ocorrer após 3000 ms.
- Define a variável `button_block` para `true`, bloqueando novas interrupções até que a sequência seja concluída.

### 3. Callback do Temporizador (`turn_off_callback`)

A função `turn_off_callback` é chamada a cada 3000 ms e:
- Desliga um LED por vez, seguindo a ordem definida no vetor `leds[]`.
- Incrementa o índice (`aux`) para passar ao próximo LED.
- Se ainda houver LEDs a serem desligados, agenda outra chamada do callback.
- Quando todos os LEDs forem desligados, libera o botão (define `button_block` como `false`), permitindo nova operação.

### 4. Loop Principal

No `main()`, após a inicialização e configuração da interrupção, o programa entra em um loop infinito (`while(true)`) que mantém a execução do sistema. O loop principal aguarda as interrupções e os callbacks do temporizador.

## Compilação e Carregamento

1. **Configuração do Ambiente**:  
   Certifique-se de ter configurado o ambiente de desenvolvimento conforme as instruções da [Raspberry Pi Pico SDK](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf).

2. **Compilação**:  
   Utilize o CMake e make para compilar o código:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
