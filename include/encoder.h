#ifndef _ENCODER_H_
#define _ENCODER_H_

#include <unistd.h>
#include <gpio.h>

#define TICKS_PER_REV 1200

extern volatile uint32_t enc_pos;
extern volatile uint32_t last_state;
/*
 * Initialize the encoder
 * This only supports one encoder at a time
 */
void encoder_init();

/*
 * Stop the encoder
 * This only supports one encoder at a time
 */
void encoder_stop();

/*
 * Handle the IRQ for the encoder
 * Calculate the position.
 */
void encoder_irq_handler();

/*
 * Returns the current position of the encoder
 */
uint32_t encoder_read();

/**
* @brief Register a callback function for the encoder ISR
*
* @param callback Pointer to the callback function
*/
void svc_reg_encoder_callback(void (*callback)(uint32_t, uint32_t));

#endif /* _ENCODER_H_ */
