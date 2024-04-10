/**
 * @file adc.c
 *
 * @brief the functions to implement adc initialization and read
 *
 * @date 04/04/2024
 *
 * @author Yuhong Yao (yuhongy), Yiying Li (yiyingl4)
 */

#include "FreeRTOS.h"
#include "task.h"
#include <FreeRTOSConfig.h>
#include <gpio.h>
#include <stdint.h>
#include <rcc.h>
#include <unistd.h>
#include <adc.h>
#include "semphr.h"

/** @brief The ADC register map. */
struct adc_reg_map {
	volatile uint32_t SR;      /**<  status register  */
    volatile uint32_t CR1;      /**<  Control Register 1 */
    volatile uint32_t CR2;      /**<  Control Register 2 */
    volatile uint32_t SMPR1;     /**<  sample time register 1 */
    volatile uint32_t SMPR2;     /**<  sample time register 2 */
	volatile uint32_t JOFR1;     /**< injected channel data offset register 1  */
	volatile uint32_t JOFR2;     /**<  injected channel data offset register 2 */
	volatile uint32_t JOFR3;     /**<  injected channel data offset register 3 */
	volatile uint32_t JOFR4;     /**<  injected channel data offset register 4 */
	volatile uint32_t HTR;     /**<  watchdog higher threshold register */
	volatile uint32_t LTR;     /**<  watchdog lower threshold register */
	volatile uint32_t SQR1;     /**<  regular sequence register 1 */
	volatile uint32_t SQR2;     /**<  regular sequence register 2 */
	volatile uint32_t SQR3;     /**<  regular sequence register 3 */
	volatile uint32_t JSQR;     /**<  injected sequence register */
	volatile uint32_t JDR1;     /**<  injected data register 1 */
	volatile uint32_t JDR2;     /**<  injected data register 2 */
	volatile uint32_t JDR3;     /**<  injected data register 3 */
	volatile uint32_t JDR4;     /**<  injected data register 4 */
	volatile uint32_t DR;     /**<  regular data register */
	volatile uint32_t CCR;     /**<  common control register */
};

/** @brief Base Address of ADC1 */
#define ADC1_BASE   (struct adc_reg_map *) 0x40012000

/** @brief ADON: A/DConverterON/OFF */
#define ADC1_CR2_ADON  (1 << 0)

/** @brief higher bit of RES */
#define ADC1_CR1_RES_HI  (1 << 25)

/** @brief lower bit of RES */
#define ADC1_CR1_RES_LO (1 << 24)

/** @brief Start conversion of regular channels */
#define ADC1_CR2_SWSTART  (1 << 30)

/** @brief Continuous conversion */
#define ADC1_CR2_CONT  (1 << 1)

/** @brief SQR3 SQ1[4:0] */
#define ADC1_SQR3_SQ1 (31 << 0)

/** @brief Regular channel end of conversion */
#define ADC1_SR_EOC (1 << 1)

/** @brief ADC regular sequence register 1 -- Regularchannelsequencelength */
#define ADC1_SQR1_L (0xF << 20)

SemaphoreHandle_t adc_mutex;

/**
 *
 * @brief  initialize the adc.
 *
 */
void adc_init(){
	// set rcc
	struct rcc_reg_map *rcc = RCC_BASE;
	rcc->apb2_enr |= ADC_CLKEN;

	// GPIO Pins(PA_0, A0)(ADC1/0)  light sensor
	gpio_init(GPIO_A, 0, MODE_ANALOG_INPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT0);
	// (PA_1 A1) (ADC1/1) temp sensor
	gpio_init(GPIO_A, 1, MODE_ANALOG_INPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT0);

	// set adc
	struct adc_reg_map *adc = ADC1_BASE;
	// set 10 bit resolution for the ADC
	adc->CR1 &= (~ADC1_CR1_RES_HI); // 0
	adc->CR1 |= ADC1_CR1_RES_LO; // 1
	
	adc_mutex = xSemaphoreCreateMutex();
	// single conversion mode
	adc->CR2 &= ~(ADC1_CR2_CONT); // enable single conversion mode
	// adc->SMPR2 &= ~(0x7 << 15);
	// adc->SMPR2 |= (0x4 << 15);

	// turn on adc power
	adc->CR2 |= ADC1_CR2_ADON;
	
	return;
}

/**
 *
 * @brief  Performing an ADC read.
 *	chan: channel
 */
uint16_t adc_read_chan(uint8_t chan){
	uint16_t adc_val = 0;
	if (xSemaphoreTake(adc_mutex, portMAX_DELAY) == pdTRUE) {
		taskENTER_CRITICAL();
		struct adc_reg_map *adc = ADC1_BASE;
		adc->SQR1 &= ~ADC1_SQR1_L;

		// adc->SQR3 &= ~ADC1_SQR3_SQ1;
		adc->SQR3 = chan;
		adc->CR2 |= ADC1_CR2_SWSTART;
		vTaskDelay(pdMS_TO_TICKS(15));

		while (!(adc->SR & ADC1_SR_EOC)) {};
		taskEXIT_CRITICAL();
		adc_val = adc->DR;
		xSemaphoreGive(adc_mutex);
	}
	return adc_val;
}
