/**
 * @file exti.c
 *
 * @brief
 *
 * @date
 *
 * @author
 */

#include <exti.h>
#include <gpio.h>
#include <rcc.h>

#define UNUSED __attribute__((unused))


void enable_exti(UNUSED gpio_port port, UNUSED uint32_t channel, UNUSED uint32_t edge) {

}

void disable_exti(UNUSED uint32_t channel) {
   
}

void exti_clear_pending_bit(UNUSED uint32_t channel) {
    
}
