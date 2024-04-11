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
#include <nvic.h>

#include <arm.h>

#define UNUSED __attribute__((unused))

#define RCC_SYSCFGEN (1 << 14)

#define SYSCFG_BASE (struct syscfg_reg_map *) 0x40013800 
#define EXTI_BASE   (struct exti_reg_map *)   0x40013C00 
#define EXTI_PR13   (1 << 13)

#define EXTI0_INT_NUM (6)
#define EXTI1_INT_NUM (7)
#define EXTI2_INT_NUM (8)
#define EXTI3_INT_NUM (9)
#define EXTI4_INT_NUM (10)
#define EXTI9_5_INT_NUM (23)
#define EXTI15_10_INT_NUM (40)

volatile uint8_t exti_flag = 0;

/** @brief syscfg */
struct syscfg_reg_map {
    volatile uint32_t memrmp; /**< 00 memory remap register */
    volatile uint32_t pmc; /**< 04 peripheral mode configuration register */
    volatile uint32_t exticr[4]; /**< 08-14 external interrupt configuration register */
    volatile uint32_t cmpcr; /**< 20 compensation cell control register */
};

/** @brief exti*/
struct exti_reg_map {
    volatile uint32_t imr; /**< 00 interrupt mask register */
    volatile uint32_t emr; /**< 04 event mask register */
    volatile uint32_t rtsr; /**< 08 rising trigger selection register */
    volatile uint32_t ftsr; /**< 0C falling trigger selection register */
    volatile uint32_t swier; /**< 10 software interrupt event register */
    volatile uint32_t pr; /**< 14 pending register */
};

void enable_exti(UNUSED gpio_port port, UNUSED uint32_t channel, UNUSED uint32_t edge) {
    // syscfg enable rcc
    struct rcc_reg_map* rcc = RCC_BASE;
    rcc->apb2_enr |= RCC_SYSCFGEN;
    // nvic_irq(40, IRQ_ENABLE);

    struct syscfg_reg_map* syscfg = SYSCFG_BASE;
    syscfg->exticr[3] &= ~(0xF << 4);
    syscfg->exticr[3] |= (0b0010 << 4);
    uint32_t exticr_x = (channel / 4);
    uint32_t exti_x = (channel % 4) * 4;
    switch (channel) {
        case 0:
            nvic_irq(EXTI0_INT_NUM, IRQ_ENABLE);
            break;
        case 1:
            nvic_irq(EXTI1_INT_NUM, IRQ_ENABLE);
            break;
        case 2:
            nvic_irq(EXTI2_INT_NUM, IRQ_ENABLE);
            break;
        case 3:
            nvic_irq(EXTI3_INT_NUM, IRQ_ENABLE);
            break;
        case 4:
            nvic_irq(EXTI4_INT_NUM, IRQ_ENABLE);
            break;
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            nvic_irq(EXTI9_5_INT_NUM, IRQ_ENABLE);
            break;
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            nvic_irq(EXTI15_10_INT_NUM, IRQ_ENABLE);
            break;
        default:
            break;
    }

    syscfg->exticr[exticr_x] &= ~(0xF << exti_x);
    syscfg->exticr[exticr_x] |= (port << exti_x);

    struct exti_reg_map* exti = EXTI_BASE;

    if (edge == RISING_EDGE || edge == RISING_FALLING_EDGE) {
        exti->rtsr |= (1 << channel);
    } else {
        exti->rtsr &= ~(1 << channel);
    }
    
    if (edge == FALLING_EDGE || edge == RISING_FALLING_EDGE) {
        exti->ftsr |= (1 << channel); 
    } else {
        exti->ftsr &= ~(1 << channel);
    }
    exti->imr |= ~(1 << channel);
}

void disable_exti(UNUSED uint32_t channel) {
   struct exti_reg_map* exti = EXTI_BASE;
   exti->imr &= ~(1 << channel);
}

void exti_clear_pending_bit(UNUSED uint32_t channel) {
    struct exti_reg_map* exti = EXTI_BASE;
    // cleared by programming it to ‘1’.
    exti->pr = (1 << channel);
}

void EXTI13_IRQHandler(void) {
    struct exti_reg_map* exti = EXTI_BASE;
    breakpoint();
    if (exti->pr & EXTI_PR13) { 
        exti_flag = 1;
        exti_clear_pending_bit(13);
        nvic_clear_pending(EXTI15_10_INT_NUM);
    }
}
