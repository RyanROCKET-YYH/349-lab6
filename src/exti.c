/**
 * @file exti.c
 *
 * @brief EXTI (EXTernal Interrupts and events)
 *
 * @date 04/12/2024
 *
 * @author Yuhong Yao (yuhongy), Yiying Li (yiyingl4)
 */

#include <exti.h>
#include <rcc.h>
#include <nvic.h>
#include <stdio.h>
#include <encoder.h>
#include <arm.h>

/** @brief define unused */
#define UNUSED __attribute__((unused))


/** @brief define SYSCFG_BASE */
#define SYSCFG_BASE (struct syscfg_reg_map *) 0x40013800 
/** @brief define EXTI_BASE */
#define EXTI_BASE   (struct exti_reg_map *)   0x40013C00

/** @brief define RCC_SYSCFGEN */
#define RCC_SYSCFGEN (1 << 14)

/** @brief EXTI_PR0 */
#define EXTI_PR0    (1)
/** @brief EXTI_PR4 */
#define EXTI_PR4    (1 << 4)
/** @brief EXTI_PR5 */
#define EXTI_PR5    (1 << 5)
/** @brief EXTI_PR6 */
#define EXTI_PR6    (1 << 6)
/** @brief EXTI_PR7 */
#define EXTI_PR7    (1 << 7)
/** @brief EXTI_PR8 */
#define EXTI_PR8    (1 << 8)
/** @brief EXTI_PR9 */
#define EXTI_PR9    (1 << 9)

/** @brief EXTI0_INT_NUM */
#define EXTI0_INT_NUM (6)
/** @brief EXTI1_INT_NUM */
#define EXTI1_INT_NUM (7)
/** @brief EXTI2_INT_NUM */
#define EXTI2_INT_NUM (8)
/** @brief EXTI3_INT_NUM */
#define EXTI3_INT_NUM (9)
/** @brief EXTI4_INT_NUM */
#define EXTI4_INT_NUM (10)
/** @brief EXTI9_5_INT_NUM */
#define EXTI9_5_INT_NUM (23)
/** @brief EXTI15_10_INT_NUM */
#define EXTI15_10_INT_NUM (40)

/** @brief define exti flag for motor moving forward */
volatile uint8_t exti_flag_forward = 0;
/** @brief define exti flag for motor moving backward */
volatile uint8_t exti_flag_backward = 0;

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

/**
 * @brief  Enables external interrupts on a given GPIO port and channel, triggered by a condition specified by edge.
 * edge - RISING_EDGE, FALLING_EDGE, RISING_FALLING_EDGE(for button)
 *
*/
void enable_exti(UNUSED gpio_port port, UNUSED uint32_t channel, UNUSED uint32_t edge) {
    // syscfg enable rcc
    struct rcc_reg_map* rcc = RCC_BASE;
    rcc->apb2_enr |= RCC_SYSCFGEN;

    struct syscfg_reg_map* syscfg = SYSCFG_BASE;

    // generate exticr_x and exti_x
    uint32_t exticr_x = (channel / 4);
    uint32_t exti_x = (channel % 4) * 4;

    // enable the interrupt in the NVIC
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
    
    // set/clr the bits
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
    exti->imr |= (1 << channel); // mask 1
}

/**
 * @brief  Disables external interrupts on a given channel
 *
*/
void disable_exti(UNUSED uint32_t channel) {
    struct exti_reg_map* exti = EXTI_BASE;
    exti->imr &= ~(1 << channel);
}

/**
 * @brief  exti clear pending bit
 *
*/
void exti_clear_pending_bit(UNUSED uint32_t channel) {
    struct exti_reg_map* exti = EXTI_BASE;
    // cleared by programming it to ‘1’.
    exti->pr = (1 << channel);
}

/**
 * @brief  EXTI9_5 Interrupt Handler: used in boot.S
 *
*/
void EXTI9_5_IRQHandler(void) {
    struct exti_reg_map* exti = EXTI_BASE;

    // For YIYING's encoder
    // if (exti->pr & (EXTI_PR5 | EXTI_PR6)) { 
    //     encoder_irq_handler();
    //     exti_clear_pending_bit(5);
    //     exti_clear_pending_bit(6);
    // }

    if (exti->pr & EXTI_PR7) {      // forward button
        exti_flag_forward = 1;
        exti_clear_pending_bit(7);
    }

    if (exti->pr & EXTI_PR6) {      // backword button
        exti_flag_backward = 1;
        exti_clear_pending_bit(6);
    }

    // For YUHONG's encoder
    if (exti->pr & (EXTI_PR8 | EXTI_PR9)) {
        encoder_irq_handler();
        
        exti_clear_pending_bit(8);
        exti_clear_pending_bit(9);
    }

    nvic_clear_pending(EXTI9_5_INT_NUM);
}

/**
 * @brief  EXTI0 Interrupt Handler: used in boot.S
 *         FOR YIYING's forward button
*/
void EXTI0_IRQHandler(void) {
    // breakpoint();
    struct exti_reg_map* exti = EXTI_BASE;
    // For YIYING's LED toggle, boot.s: .word   EXTI0_IRQHandler                /* 22 IRQ6 EXTI0 */
    if (exti->pr & EXTI_PR0) { 
        exti_flag_forward = 1;
        exti_clear_pending_bit(0);
    }
    
    nvic_clear_pending(EXTI0_INT_NUM);
}

/**
 * @brief  EXTI4 Interrupt Handler: used in boot.S
 *         FOR YIYING's backward button
 *
*/
void EXTI4_IRQHandler(void){
    // breakpoint();
    struct exti_reg_map* exti = EXTI_BASE;
    // For YIYING's LED toggle, boot.s: .word   EXTI4_IRQHandler                /* 26 IRQ6 EXTI4 */
    if (exti->pr & EXTI_PR4){
        exti_flag_backward = 1;
        exti_clear_pending_bit(4);
    }
    // breakpoint();
    nvic_clear_pending(EXTI4_INT_NUM);
}