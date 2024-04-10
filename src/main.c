/**
 * @file main.c
 *
 * @brief
 *
 * @date
 *
 * @author
 */

#include <FreeRTOS.h>
#include <task.h>
#include <adc.h>
#include <stdio.h>
#include <stdlib.h>
#include <uart.h>
#include <atcmd.h>
#include <gpio.h>
#include <unistd.h>
#include <string.h>
#include <lcd_driver.h>
#include <keypad_driver.h>
#include <servo.h>
#include <stdbool.h>
#include <i2c.h>
#include <arm.h>
#include <timer.h>

// void vTimerTask(void *pvParameters) {
//     (void)pvParameters;

//     // PB 4 PWM pins alternate function 2 for tim3 channel 1
//     gpio_init(GPIO_B, 4, MODE_ALT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT2);
// }
int main( void ) {
    uart_init(115200);
    adc_init();
    keypad_init();
    i2c_master_init(80);
    // timer_init(3, 100, 16);
    // gpio_init(GPIO_B, 4, MODE_ALT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT2);
    
    // timer_start_pwm(3, 1, 100, 16, 8);
    gpio_init(GPIO_A, 10, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT0);
    // SERVO 2 (A1)
    // gpio_init(GPIO_A, 1, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT0);
    servo_enable(0, 1);
    for (;;) {}
    return 0;
}