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

#define ENCODER_TPR 1200

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

volatile uint32_t enc_pos = 0;
volatile uint32_t last_state = S00;

void encoder_irq_handler() {
    uint32_t enc_a = gpio_read(ENC_A_PORT, ENC_A_PIN);
    uint32_t enc_b = gpio_read(ENC_B_PORT, ENC_B_PIN);
    encoder_state enc_state = (encoder_state)((enc_a << 1) | (enc_b));

    switch (last_state)
    {
    case S00:
        if (enc_state == S01) {
            enc_pos++;
        } else if (enc_state == S10) {
            enc_pos--;
        }
        break;
    case S01:
        if (enc_state == S11) {
            enc_pos++;
        } else if (enc_state == S00) {
            enc_pos--;
        }
        break;
    case S10:
        if (enc_state == S00) {
            enc_pos++;
        } else if (enc_state == S11) {
            enc_pos--;
        }
        break;
    case S11:
        if (enc_state == S10) {
            enc_pos++;
        } else if (enc_state == S01) {
            enc_pos--;
        }
        break;
    default:
        break;
    }

    last_state = enc_state;
    // Handle overflow
    if (enc_pos >= ENCODER_TPR) {
        enc_pos = 0; // Reset position after reaching the max
    } else if (enc_pos < 0) {
        enc_pos = ENCODER_TPR - 1; // Wrap around if the count goes negative
    }
}

uint32_t encoder_read() {
    return enc_pos;
}
