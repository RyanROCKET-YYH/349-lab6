/**
 * @file encoder.c
 *
 * @brief
 *
 * @date
 *
 * @author
 */

#include <encoder.h>
#include <exti.h>
#include <gpio.h>
#include <unistd.h>
#include <nvic.h>
#include <arm.h>

#define YUHONG
#ifdef YUHONG
#include "gpio_pin_yuhong.h"
#elif defined YIYING
#include "gpio_pin_yiying.h"
#endif

typedef enum {S00 = 0x0, S10 = 0x2, S11 = 0x3, S01 = 0x1} encoder_state;


void encoder_init() {
    gpio_init(ENC_A_PORT, ENC_A_PIN, MODE_INPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_PULL_UP, ALT0);
    gpio_init(ENC_B_PORT, ENC_B_PIN, MODE_INPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_PULL_UP, ALT0);

    enable_exti(ENC_A_PORT, ENC_A_PIN, RISING_FALLING_EDGE);
    enable_exti(ENC_B_PORT, ENC_B_PIN, RISING_FALLING_EDGE);
}

void encoder_stop() {
    disable_exti(ENC_A_PIN);
    disable_exti(ENC_B_PIN);
}

uint32_t encoder_read() {
    
}
