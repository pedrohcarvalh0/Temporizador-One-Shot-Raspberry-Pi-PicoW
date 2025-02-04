#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

// Definições dos pinos
#define RED_PIN_LED   13
#define BLUE_PIN_LED  12
#define GREEN_PIN_LED 11
#define BTN_PIN       5

// Variáveis globais
static volatile uint32_t last_time = 0;
static volatile uint8_t aux = 0;
static volatile bool button_block = false;  // Impede que o botão acione os LEDs enquanto estão ativos
static volatile uint8_t leds[] = { RED_PIN_LED, BLUE_PIN_LED, GREEN_PIN_LED }; // Ordem de desligamento dos LEDs

// Protótipos das funções
static void gpio_irq_handler(uint gpio, uint32_t events);
int64_t turn_off_callback(alarm_id_t id, void *user_data);

int64_t turn_off_callback(alarm_id_t id, void *user_data) {
    // Desliga o LED atual e passa para o próximo
    gpio_put(leds[aux], 0);
    aux++;

    if (aux < 3) {
        // Agenda o desligamento do próximo LED após 3000 ms
        add_alarm_in_ms(3000, turn_off_callback, NULL, false);
    } else {
        // Todos os LEDs foram desligados; libera o botão
        button_block = false;
    }

    return 0;
}

void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    // Verifica debounce (300 ms) e se o botão não está bloqueado
    if ((current_time - last_time > 300000) && !button_block) {
        last_time = current_time;
        button_block = true;  // Bloqueia o botão enquanto os LEDs estiverem ativos

        // Acende os LEDs
        gpio_put(GREEN_PIN_LED, 1);
        gpio_put(BLUE_PIN_LED, 1);
        gpio_put(RED_PIN_LED, 1);

        aux = 0;
        // Agenda a primeira chamada para desligar os LEDs
        add_alarm_in_ms(3000, turn_off_callback, NULL, false);
    }
}

void setup() {
    // Inicializa os LEDs
    gpio_init(GREEN_PIN_LED);
    gpio_set_dir(GREEN_PIN_LED, GPIO_OUT);
    gpio_put(GREEN_PIN_LED, 0);

    gpio_init(BLUE_PIN_LED);
    gpio_set_dir(BLUE_PIN_LED, GPIO_OUT);
    gpio_put(BLUE_PIN_LED, 0);

    gpio_init(RED_PIN_LED);
    gpio_set_dir(RED_PIN_LED, GPIO_OUT);
    gpio_put(RED_PIN_LED, 0);

    // Inicializa o botão com resistor de pull-up
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);
}

int main() {
    stdio_init_all();
    setup();

    // Configura a interrupção para o botão (queda de nível)
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    while (true) {
        sleep_ms(100);
    }
}
