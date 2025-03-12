#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r = 0;
volatile int flag_g = 0;

volatile int g_timer_1 = 0;
volatile int g_timer_2 = 0;





bool timer_1_callback(repeating_timer_t *rt) {
    g_timer_1 = 1;
    return true; // keep repeating
}

bool timer_2_callback(repeating_timer_t *rt) {
    g_timer_2 = 1;
    return true; // keep repeating
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_R)
            flag_r = 1;
        else if (gpio == BTN_PIN_G)
            flag_g = 1;
    }
}

int main() {

    int timer_active_1= 0;
    int timer_active_2 = 0;

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    int timer_1_hz = 500;
    repeating_timer_t timer_1;
    int timer_2_hz = 250;
    repeating_timer_t timer_2;


    while (true) {

        if (flag_r) {
            flag_r = 0;

            if (timer_active_1){
                cancel_repeating_timer(&timer_1);
                timer_active_1 = 0;
                gpio_put(LED_PIN_R, 0);
            } else {
                 add_repeating_timer_ms(timer_1_hz, timer_1_callback, NULL, &timer_1);
                 timer_active_1 = 1;
            }
        }

        if (flag_g) {
            flag_g = 0;

            if (timer_active_2) {
                cancel_repeating_timer(&timer_2); // Para o timer
                timer_active_2 = 0;
                gpio_put(LED_PIN_G, 0);
            } else {
                add_repeating_timer_ms(timer_2_hz, timer_2_callback, NULL, &timer_2);
                timer_active_2 = 1 ;
            }
        }


        if(g_timer_1){
            g_timer_1 = 0;
            gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
        }
        if(g_timer_2){
            g_timer_2 = 0;
            gpio_put(LED_PIN_G, !gpio_get(LED_PIN_G));
        }
    }
}
