#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;
volatile int btn_release = 0;
volatile uint64_t press_time = 0; 


void btn_callback(uint gpio, uint32_t events) {
    uint64_t now = time_us_64();
    if (events == 0x4) { // fall edge
        press_time = now;
 } else if (events == 0x8) { // rise edge
      uint64_t rise_time = time_us_64();
      if((rise_time - press_time) > 500000){
         btn_release = 1;
      }
    }
}

int main() {

    int led_aceso = 0;

    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    while (true) {
        if(btn_release){
            if(led_aceso){
                gpio_put(LED_PIN_R, 0);
                led_aceso = 0;
            }else {
                gpio_put(LED_PIN_R, 1);
                led_aceso = 1;
            }
            btn_release = 0;
        }
        sleep_ms(10);
    }
}
