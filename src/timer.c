/**
 * @file timer.c
 *
 * @brief timer related functions of lab 6
 *
 * @date 04/09/2024
 *
 * @author Yuhong Yao (yuhongy), Yiying Li (yiyingl4)
 */

#include <unistd.h>
#include <timer.h>
#include <rcc.h>
#include <nvic.h>
#include <gpio.h>

/** @brief define unused */
#define UNUSED __attribute__((unused))
/** @brief set PWM_MODE1 */
#define PWM_MODE1 (0b110)
/** @brief set TIM_CCMR_OC1PE */
#define TIM_CCMR_OC1PE (1 << 3)
/** @brief set TIM_CCMR_OC2PE */
#define TIM_CCMR_OC2PE (1 << 11)
/** @brief set TIM_CCER_CC1E */
#define TIM_CCER_CC1E (1 << 0)
/** @brief set TIM_CCER_CC1P */
#define TIM_CCER_CC1P (1 << 1)
/** @brief set TIM_CCER_CC2E */
#define TIM_CCER_CC2E (1 << 4)
/** @brief set TIM_CCER_CC2P */
#define TIM_CCER_CC2P (1 << 5)
/** @brief set TIM_CCER_CC3E */
#define TIM_CCER_CC3E (1 << 8)
/** @brief set TIM_CCER_CC3P */
#define TIM_CCER_CC3P (1 << 9)
/** @brief set TIM_CCER_CC4E */
#define TIM_CCER_CC4E (1 << 12)
/** @brief set TIM_CCER_CC4P */
#define TIM_CCER_CC4P (1 << 13)

/** @brief set PWM_MODE1 */
struct tim2_5* const timer_base[] = {(void *)0x0,    // N/A - Don't fill out
                                     (void *)0x0,    // N/A - Don't fill out
                                     (void *)0x40000000, // TIMER 2 Base Address
                                     (void *)0x40000400, // TIMER 3 Base Address
                                     (void *)0x40000800, // TIMER 4 Base Address
                                     (void *)0x40000C00};  // TIMER 5 Base Address

/**
 *
 * @brief  Starts the timer
 *
 *  timer      - The timer
 *  prescaler  - Prescalar for4clock
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

/**
 * @brief set timer for pwm
 *  - duty cycle: the number of ticks that you want the output to be set to HIGH. (Never be greater than period)
 *
*/
void timer_start_pwm(UNUSED int timer, UNUSED uint32_t UNUSED channel, UNUSED uint32_t prescalar, UNUSED uint32_t period, UNUSED uint32_t duty_cycle) {
  if (timer < 2 || timer > 5) return; // Check for valid timer
  struct tim2_5* tim = timer_base[timer];
  struct rcc_reg_map *rcc = RCC_BASE;
  switch (timer)
  {
  case 2:
    rcc->apb1_enr |= TIM2_CLKEN;
    break;
  case 3:
    rcc->apb1_enr |= TIM3_CLKEN;
    break;
  case 4:
    rcc->apb1_enr |= TIM4_CLKEN;
    break;
  case 5:
    rcc->apb1_enr |= TIM5_CLKEN;
    break;
  default:
    break;
  }
  switch (channel){
  case 1:
    tim->ccmr[0] &= ~(0b111 << 4);
    tim->ccmr[0] |= (PWM_MODE1 << 4);
    tim->ccmr[0] |= TIM_CCMR_OC1PE;
    tim->ccer |= TIM_CCER_CC1E;  // enable output compare
    tim->ccer &= ~(TIM_CCER_CC1P); // active high
    break;
  case 2: 
    tim->ccmr[0] &= ~(0b111 << 4);
    tim->ccmr[0] |= (PWM_MODE1 << 12);
    tim->ccmr[0] |= TIM_CCMR_OC2PE;
    tim->ccer |= TIM_CCER_CC2E;  // enable output compare
    tim->ccer &= ~(TIM_CCER_CC2P); // active high
    break;
  case 3:
    tim->ccmr[1] &= ~(0b111 << 4);
    tim->ccmr[1] |= (PWM_MODE1 << 4);
    tim->ccmr[1] |= TIM_CCMR_OC1PE;
    tim->ccer |= TIM_CCER_CC3E;  // enable output compare
    tim->ccer &= ~(TIM_CCER_CC3P); // active high
    break;
  case 4: 
    tim->ccmr[1] &= ~(0b111 << 4);
    tim->ccmr[1] |= (PWM_MODE1 << 12);
    tim->ccmr[1] |= TIM_CCMR_OC2PE;
    tim->ccer |= TIM_CCER_CC4E;  // enable output compare
    tim->ccer &= ~(TIM_CCER_CC4P);
    break;
  default:
    break;
  }
  
  tim->psc = prescalar - 1;
  tim->arr = period - 1;
  tim->ccr[channel - 1] = duty_cycle;

  tim->cr1 |= 1; // Enable the timer
}

/**
 * @brief dynamically set the duty cycle after you initialize the timer and its PWM mode
*/
void timer_set_duty_cycle(UNUSED int timer, UNUSED uint32_t channel, UNUSED uint32_t duty_cycle) {
  if (timer < 2 || timer > 5) return;
  struct tim2_5* tim = timer_base[timer];

  switch (channel) {
    case 1:
      tim->ccr[channel - 1] = duty_cycle;
      break;
    case 2:
      tim->ccr[channel - 1] = duty_cycle;
      break;
    case 3:
      tim->ccr[channel - 1] = duty_cycle;
      break;
    case 4:
      tim->ccr[channel - 1] = duty_cycle;
      break;
    default:
      break;
  }
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