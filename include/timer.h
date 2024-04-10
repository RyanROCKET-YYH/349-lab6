#ifndef _TIMER_H_
#define _TIMER_H_

#include <unistd.h>
#include <gpio.h>

#define TIM_SR_UIF (1)
#define TIM2_INT_NUM    28
#define TIM3_INT_NUM    29
#define TIM4_INT_NUM    30
#define TIM5_INT_NUM    50

/** @brief tim2_5 */
struct tim2_5 {
  volatile uint32_t cr1; /**< 00 Control Register 1 */
  volatile uint32_t cr2; /**< 04 Control Register 2 */
  volatile uint32_t smcr; /**< 08 Slave Mode Control */
  volatile uint32_t dier; /**< 0C DMA/Interrupt Enable */
  volatile uint32_t sr; /**< 10 Status Register */
  volatile uint32_t egr; /**< 14 Event Generation */
  volatile uint32_t ccmr[2]; /**< 18-1C Capture/Compare Mode */
  volatile uint32_t ccer; /**< 20 Capture/Compare Enable */
  volatile uint32_t cnt; /**< 24 Counter Register */
  volatile uint32_t psc; /**< 28 Prescaler Register */
  volatile uint32_t arr; /**< 2C Auto-Reload Register */
  volatile uint32_t reserved_1; /**< 30 */
  volatile uint32_t ccr[4]; /**< 34-40 Capture/Compare */
  volatile uint32_t reserved_2; /**< 44 */
  volatile uint32_t dcr; /**< 48 DMA Control Register */
  volatile uint32_t dmar; /**< 4C DMA address for full transfer Register */
  volatile uint32_t or; /**< 50 Option Register */
};


void timer_start_pwm(int timer, uint32_t channel, uint32_t prescalar, uint32_t period, uint32_t duty_cycle);

void timer_set_duty_cycle(int timer, uint32_t channel, uint32_t duty_cycle);

void timer_init(int timer, uint32_t prescalar, uint32_t period);

void timer_disable(int timer);

void timer_clear_interrupt_bit(int timer);

#endif /* _TIMER_H_ */
