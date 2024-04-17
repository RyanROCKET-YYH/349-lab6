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

// Forward declaration of encoder_read
extern uint32_t encoder_read(void);



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


void motor_set_dir(gpio_port port_a, gpio_port port_b, uint32_t channel_a, uint32_t channel_b, uint32_t timer, uint32_t timer_channel, uint32_t duty_cycle, MotorDirection direction) {
    switch (direction) {
        case FORWARD:
            // Set IN1 = HIGH and IN2 = LOW for forward direction
            gpio_set(port_a, channel_a);   // MOTOR IN1
            gpio_clr(port_b, channel_b);    // MOTOR IN2
            timer_set_duty_cycle(timer, timer_channel, duty_cycle);
            break;

        case BACKWARD:
            // Set IN1 = LOW and IN2 = HIGH for backward direction
            gpio_clr(port_a, channel_a);    // MOTOR IN1
            gpio_set(port_b, channel_b);   // MOTOR IN2
            timer_set_duty_cycle(timer, timer_channel, duty_cycle);
            break;

        case STOP:
            // Set both IN1 and IN2 HIGH to stop the motor
            gpio_set(port_a, channel_a);   // MOTOR IN1
            gpio_set(port_b, channel_b);   // MOTOR IN2
            // timer_set_duty_cycle(timer, timer_channel, 0);
            break;

        case FREE:
            // // Set both IN1 and IN2 LOW
            // gpio_clr(port_a, channel_a);    // MOTOR IN1
            // gpio_clr(port_b, channel_b);    // MOTOR IN2
            timer_set_duty_cycle(timer, timer_channel, 0); // Set PWM to 0%
            break;
    }
}

uint8_t motor_position() {
    uint32_t motor_pos = encoder_read();
    return motor_pos;
}
