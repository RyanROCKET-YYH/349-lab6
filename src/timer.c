
/**
 * @file
 *
 * @brief
 *
 * @date
 *
 * @author
 */

#include <unistd.h>
#include <timer.h>
#include <rcc.h>
#include <nvic.h>
#include <gpio.h>

#define UNUSED __attribute__((unused))

struct tim2_5* const timer_base[] = {(void *)0x0,   // N/A - Don't fill out
                                     (void *)0x0,   // N/A - Don't fill out
                                     (void *)-1,    // TODO: fill out address for TIMER 2
                                     (void *)-1,    // TODO: fill out address for TIMER 3
                                     (void *)-1,    // TODO: fill out address for TIMER 4
                                     (void *)-1};   // TODO: fill out address for TIMER 5


/**
 *
 * @brief  Starts the timer
 *
 *  timer      - The timer
 *  prescaler  - Prescalar for clock
 *  Period     - Period of the timer interrupt
 */
void timer_init(UNUSED int timer, UNUSED uint32_t prescalar, UNUSED uint32_t period) {
  if (timer < 2 || timer > 5) return; // Check for valid timer
  struct tim2_5* tim = timer_base[timer];
  // 1. Enable the timer clock in RCC 
  struct rcc_reg_map *rcc = RCC_BASE;
  // 2. Clear the interrupt pending bit
  switch (timer)
  {
  case 2:
    rcc->apb1_enr |= TIM2_CLKEN;
    nvic_irq(TIM2_INT_NUM, IRQ_ENABLE);
    break;
  case 3:
    rcc->apb1_enr |= TIM3_CLKEN;
    nvic_irq(TIM3_INT_NUM, IRQ_ENABLE);
    break;
  case 4:
    rcc->apb1_enr |= TIM4_CLKEN;
    nvic_irq(TIM4_INT_NUM, IRQ_ENABLE);
    break;
  case 5:
    rcc->apb1_enr |= TIM5_CLKEN;
    nvic_irq(TIM5_INT_NUM, IRQ_ENABLE);
    break;
  default:
    break;
  }
  // 3. Set the prescalar value 
  tim->psc = prescalar - 1;
  // 4. Set the auto-reload value
  tim->arr = period - 1;
  // 5. Enable the timer and its interrupt
  tim->dier |= 1; // Update interrupt enable
  tim->cr1 |= 1; // Enable the timer
}

void timer_start_pwm(UNUSED int timer, UNUSED uint32_t UNUSED channel, UNUSED uint32_t prescalar, UNUSED uint32_t period, UNUSED uint32_t duty_cycle) {

}

void timer_set_duty_cycle(UNUSED int timer, UNUSED uint32_t channel, UNUSED uint32_t duty_cycle) {
    
}

/**
 *
 * @brief  Clears the timer interrupt bit
 *
 * @param timer      - The timer
*/
void timer_clear_interrupt_bit(UNUSED int timer) {
  if (timer < 2 || timer > 5) return; // Check for valid timer
  struct tim2_5* tim = timer_base[timer];
  // Clear the update interrupt flag
  tim->sr &= ~1;
}

/** @brief set the led state */
volatile uint8_t ledstate = 0;

/**
 * tim3_irq_handler():
 * @brief  Set time 3 interrupt requestion handler
*/
void tim3_irq_handler() {
  struct tim2_5* tim3 = timer_base[3];
  if (tim3->sr & TIM_SR_UIF) {
    if (ledstate) {
      gpio_clr(GPIO_A, 5);
    } else {
      gpio_set(GPIO_A, 5);
    }
    
    ledstate = !ledstate;
    timer_clear_interrupt_bit(3);
  }
}


/**
 *
 * @brief  Stops the timer
 *
 * @param timer      - The timer
*/
void timer_disable(UNUSED int timer) {
  if (timer < 2 || timer > 5) return; // Check for valid timer
  struct tim2_5* tim = timer_base[timer];
  // Disable the timer
  tim->cr1 &= ~1;
  struct rcc_reg_map *rcc = RCC_BASE;
  switch (timer)
  {
  case 2:
    rcc->apb1_enr &= ~TIM2_CLKEN;
    break;
  case 3:
    rcc->apb1_enr &= ~TIM3_CLKEN;
    break;
  case 4:
    rcc->apb1_enr &= ~TIM4_CLKEN;
    break;
  case 5:
    rcc->apb1_enr &= ~TIM5_CLKEN;
    break;
  default:
    break;
  }
}