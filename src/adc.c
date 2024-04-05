/**
 * @file adc.c
 *
 * @brief
 *
 * @date
 *
 * @author
 */

#include <gpio.h>
#include <stdint.h>
#include <rcc.h>
#include <unistd.h>
#include <adc.h>

#define UNUSED __attribute__((unused))

void adc_init(){
	return;
}

uint16_t adc_read_chan(UNUSED uint8_t chan){
	return -1;
}