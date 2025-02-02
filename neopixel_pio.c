#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2818b.pio.h"

#define LED_COUNT 25 // Matriz 5x5 de LEDs
#define LED_PIN 7 // GPIO para a matriz de LEDs WS2812
#define BUTTON_A 5 // Pino para o botão A
#define BUTTON_B 6 // Pino para o botão B
#define RED_LED_PIN 13 // Pino para o LED vermelho (separado)

volatile int number = 0; // Número exibido na matriz de LEDs
volatile bool button_a_pressed = false; // Flag para o botão A
volatile bool button_b_pressed = false; // Flag para o botão B

// Declaração dos manipuladores de interrupção
void button_a_irq_handler(uint gpio, uint32_t events);
void button_b_irq_handler(uint gpio, uint32_t events);

// Define a estrutura para o controle dos LEDs WS2812
struct pixel_t {
    uint8_t G, R, B;
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t;

npLED_t leds[LED_COUNT];
PIO np_pio;
uint sm;

// Matrizes para os dígitos de 0 a 9
const uint8_t digits[10][5][5] = {
    { {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0} }, // 0
    { {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 1, 1, 0, 0}, {0, 0, 1, 0, 0} }, // 1
    { {0, 1, 1, 1, 0}, {0, 1, 0, 0, 0}, {0, 1, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 1, 1, 1, 0} }, // 2
    { {0, 1, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 1, 1, 1, 0} }, // 3
    { {0, 1, 0, 0, 0}, {0, 0, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0} }, // 4
    { {0, 1, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 0, 0}, {0, 1, 1, 1, 0} }, // 5
    { {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 0, 0}, {0, 1, 1, 1, 0} }, // 6
    { {0, 1, 0, 0, 0}, {0, 0, 0, 1, 0}, {0, 1, 0, 0, 0}, {0, 0, 0, 1, 0}, {0, 1, 1, 1, 0} }, // 7
    { {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0} }, // 8
    { {0, 1, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0} }  // 9
};

// Função para configurar os botões com interrupções
void setup_buttons() {
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A); // Habilita o pull-up interno
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_a_irq_handler); // Interrupção para o botão A

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B); // Habilita o pull-up interno
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &button_b_irq_handler); // Interrupção para o botão B
}

// Função de interrupção para o botão A
void button_a_irq_handler(uint gpio, uint32_t events) {
    static uint32_t last_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if (current_time - last_time > 200) { // Debouncing de 200ms
        button_a_pressed = true;
        last_time = current_time;
    }
}

// Função de interrupção para o botão B
void button_b_irq_handler(uint gpio, uint32_t events) {
    static uint32_t last_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if (current_time - last_time > 200) { // Debouncing de 200ms
        button_b_pressed = true;
        last_time = current_time;
    }
}
// Inicializa os LEDs WS2812
void npInit(uint pin) {
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;

    sm = pio_claim_unused_sm(np_pio, false);
    if (sm < 0) {
        np_pio = pio1;
        sm = pio_claim_unused_sm(np_pio, true);
    }

    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

    for (uint i = 0; i < LED_COUNT; ++i) {
        leds[i].R = 0;
        leds[i].G = 0;
        leds[i].B = 0;
    }
}

// Atualiza o estado dos LEDs
void npWrite() {
    for (uint i = 0; i < LED_COUNT; ++i) {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100);
}

// Exibe um número na matriz de LEDs
void display_number(int num) {
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            if (digits[num][i][j]) {
                leds[i * 5 + j].R = 255;
                leds[i * 5 + j].G = 0;
                leds[i * 5 + j].B = 0;
            } else {
                leds[i * 5 + j].R = 0;
                leds[i * 5 + j].G = 0;
                leds[i * 5 + j].B = 0;
            }
        }
    }
    npWrite();
}

// Função para o LED vermelho piscar
void blink_red_led() {
    static uint32_t last_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    // Pisca 5 vezes por segundo (200ms de intervalo)
    if (current_time - last_time >= 200) {
        last_time = current_time;
        // Alterna entre acender e apagar o LED vermelho
        static bool led_on = false;
        if (led_on) {
            gpio_put(RED_LED_PIN, 0); // Apaga o LED
        } else {
            gpio_put(RED_LED_PIN, 1); // Acende o LED
        }
        led_on = !led_on;
    }
}

int main() {
    stdio_init_all();
    npInit(LED_PIN); // Inicializa a matriz de LEDs
    setup_buttons(); // Configura os botões com interrupções

    // Inicializa o pino para o LED vermelho (separado)
    gpio_init(RED_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);

    uint32_t last_button_a_time = 0; // Tempo de controle do botão A
    uint32_t last_button_b_time = 0; // Tempo de controle do botão B

    while (true) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());

        // Loop para o botão A: Incrementa enquanto pressionado
        if (gpio_get(BUTTON_A) == 0) { // Botão A pressionado (estado baixo)
            if (current_time - last_button_a_time > 200) { // Debounce de 200ms
                number = (number + 1) % 10; // Incrementa o número
                display_number(number); // Exibe o número
                last_button_a_time = current_time; // Atualiza o tempo de controle
            }
        }


        // Loop para o botão B: Decrementa enquanto pressionado
        if (gpio_get(BUTTON_B) == 0) { // Botão B pressionado (estado baixo)
            if (current_time - last_button_b_time > 200) { // Debounce de 200ms
                number = (number - 1 + 10) % 10; // Decrementa o número
                display_number(number); // Exibe o número
                last_button_b_time = current_time; // Atualiza o tempo de controle
            }
        }

        // Atualiza o LED vermelho piscando
        blink_red_led();

        sleep_ms(10); // Espera para reduzir a carga do processador
    }

    return 0;
}
