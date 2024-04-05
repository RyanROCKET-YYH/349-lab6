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


void motor_init(UNUSED gpio_port port_a, UNUSED gpio_port port_b, UNUSED gpio_port port_pwm, UNUSED uint32_t channel_a, UNUSED uint32_t channel_b, UNUSED uint32_t channel_pwm, UNUSED uint32_t timer, UNUSED uint32_t timer_channel, UNUSED uint32_t alt_timer) {

}


void motor_set_dir(UNUSED uint32_t duty_cycle, UNUSED uint32_t direction) {

}


uint8_t motor_position() {
    return -1;
}
