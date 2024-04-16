/**
 * @file
 *
 * @brief
 *
 * @date
 *
 * @author
 */

#include <motor_driver.h>
#include <unistd.h>
#include <gpio.h>
#include <timer.h>
#include <encoder.h>

#define UNUSED __attribute__((unused))

/** @brief encoder's states */
typedef enum {
    MOTOR_FREE = 0,
    MOTOR_FORWARD = 1,
    MOTOR_BACKWARD = 2,
    MOTOR_STOP = 3
} MotorDirection;

void motor_init(UNUSED gpio_port port_a, UNUSED gpio_port port_b, UNUSED gpio_port port_pwm, UNUSED uint32_t channel_a, UNUSED uint32_t channel_b, UNUSED uint32_t channel_pwm, UNUSED uint32_t timer, UNUSED uint32_t timer_channel, UNUSED uint32_t alt_timer) {
    // Initialize GPIO pins 
    gpio_init(port_a, channel_a, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_HIGH, PUPD_NONE, ALT0);// IN1
    gpio_init(port_b, channel_b, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_HIGH, PUPD_NONE, ALT0); // IN2
    gpio_init(port_pwm, channel_pwm, MODE_ALT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_HIGH, PUPD_NONE, alt_timer); // EN

    uint32_t main_clock_speed = 16 /* The main clock speed: 16 MHz */;
    uint32_t pwm_frequency = 1; // 1 MHz PWM frequency
    uint32_t prescaler = (main_clock_speed / pwm_frequency) - 1;

    // Set the PWM period to 4000
    uint32_t pwm_period = 4000;

    // Initialize encoder
    encoder_init();

    // Configure timer for PWM
    timer_init(timer, prescaler, pwm_period);
    
    // Start PWM with a 0% duty cycle
    timer_start_pwm(timer, timer_channel, prescaler, pwm_period, 0);
}


void motor_set_dir(UNUSED uint32_t duty_cycle, UNUSED uint32_t direction) {

}


uint8_t motor_position() {
    return -1;
}
