/**
 * @file main.c
 *
 * @brief
 *
 * @date
 *
 * @author
 */

#include <FreeRTOS.h>
#include <task.h>
#include <adc.h>
#include <stdio.h>
#include <stdlib.h>
#include <uart.h>
#include <atcmd.h>
#include <gpio.h>
#include <unistd.h>
#include <string.h>
#include <lcd_driver.h>
#include <keypad_driver.h>
#include <servo.h>
#include <stdbool.h>
#include <i2c.h>
#include <arm.h>
#include <timer.h>
#include <exti.h>
#include <atcmd.h>

/** @brief define parser */
atcmd_parser_t parser;
/** @brief define passcode */
int g_passcode = 349;
volatile int g_dutycycle = 16;

/**
 * @brief  handle the AT+Resume command
 * 
*/
uint8_t handleResume(void *args, const char *cmdArgs) {
    (void)args;
    (void)cmdArgs;
    isInCommandMode = false;
    printf("Exit command mode\n");
    return 1; // sucess
}

/**
 * @brief  handle the AT+Hello=<> command
 *
*/
uint8_t handleHello(void *args, const char *cmdArgs) {
    (void)args;
    if (cmdArgs != NULL) {
        printf("Hello, %s!\n", cmdArgs);
    } else {
        printf("Hello!\n"); //if <name> not provided
    }
    return 1; // success
}

/**
 * @brief  handle the AT+Passcpde? command
 *
*/
uint8_t handlePasscode(void *args, const char *cmdArgs) {
    (void)args;
    (void)cmdArgs;
    printf("Current passcode: %d\n", g_passcode);
    return 1; // success
}

/**
 * @brief  handle the AT+Passcpde=<> command
 *
*/
uint8_t handlePasscodeChange(void *args, const char *cmdArgs) {
    (void)args; 

    if (cmdArgs != NULL && strlen(cmdArgs) <= 12) {
        int new_passcode = atoi(cmdArgs);
        g_passcode = new_passcode;
        printf("Passcode changed successfully.\n");
        return 1; // Success
    } else if (cmdArgs == NULL) {
        printf("No passcode provided.\n");
    } else {
        printf("Invalid passcode (string up to 12 digits).\n");
    }
    return 0; // Fail
}

uint8_t handleDutyCycleChange(void* args, const char* cmdArgs) {
    (void)args;

    if (cmdArgs != NULL) {
        uint32_t new_duty = atoi(cmdArgs);
        if (new_duty <= 16) { // Define MAX_PWM_PERIOD according to your timer's ARR setting
            g_dutycycle = new_duty;
            printf("New Duty for PWM: %lu\n", new_duty);
            return 1;
        } else {
            printf("Duty cycle exceeds PWM period.\n");
        }
    } else {
        printf("No value provided.\n");
    }
    return 0;
}

/**
 * @brief  handle the commands:
 * AT+RESUME
 * AT+HELLO=<>
 * AT+PASSCODE?
 * AT+PASSCODE=<>
 * AT+DUTY=<>
*/
const atcmd_t commands[] = {
    {"RESUME", handleResume, NULL},
    {"HELLO", handleHello, NULL},
    {"PASSCODE?", handlePasscode, NULL},
    {"PASSCODE", handlePasscodeChange, NULL},
    {"DUTY", handleDutyCycleChange, NULL}
};

/**
 * @brief  handle the Blinky LED task
 *
*/
void vBlinkyTask(void *pvParameters) {
    (void)pvParameters;
   
    gpio_init(GPIO_A, 5, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT0);

    for(;;) {
        if (gpio_read(GPIO_A, 5)) {
            gpio_clr(GPIO_A, 5);
        } else {
            gpio_set(GPIO_A, 5);
        }
        // wait for 1000 millisec
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief  handle the UART echo task
 *
*/
static void vUARTEchoTask(void *pvParameters) {
    (void)pvParameters;
    char buffer[100];
    // char testCmd2[] = "AT+RESUME";

    for (;;) {
        // only work when command mode
        if (isInCommandMode){
            write(STDOUT_FILENO, "> ", 2);
            // Attempt to read data from UART
            memset(buffer, 0, sizeof(buffer)); // clear buffer
            ssize_t numBytesRead = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
            // check if data was read
            if (numBytesRead > 0) {
                buffer[numBytesRead] = '\0'; // Null-terminate the received string

                // Trim newline and carriage return from the end of the command
                for (int i = 0; i < numBytesRead; ++i) {
                    if (buffer[i] == '\n' || buffer[i] == '\r') {
                        buffer[i] = '\0';
                        break; // Stop at the first newline/carriage return character
                    }
                }
                // Now pass the trimmed and null-terminated command string to atcmd_parse
                atcmd_parse(&parser, buffer);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void vExtiTask(void* pvParameters) {
    (void)pvParameters;
    gpio_init(GPIO_C, 13, MODE_INPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_PULL_UP, ALT0);
    gpio_init(GPIO_A, 0, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT0);
    enable_exti(GPIO_C, 13, RISING_EDGE);
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void vHardPWM(void* pvParameters) {
    (void)pvParameters;
    gpio_init(GPIO_B, 4, MODE_ALT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT2);
    timer_init(3, 100, 16);
    timer_start_pwm(3, 1, 100, 16, g_dutycycle);
    for (;;) {
        timer_set_duty_cycle(3, 1, g_dutycycle);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main( void ) {
    uart_init(115200);
    keypad_init();
    i2c_master_init(80);
    atcmd_parser_init(&parser, commands, (sizeof(commands) / sizeof(commands[0])));
    // timer_init(3, 100, 16);
    // gpio_init(GPIO_B, 4, MODE_ALT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT2);
    
    // timer_start_pwm(3, 1, 100, 16, 2);
    // timer_set_duty_cycle(3,1, 16);
    
    xTaskCreate(
        vBlinkyTask,
        "BlinkyTask",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL);

    // Create the UART echo task
    xTaskCreate(
        vUARTEchoTask,
        "UARTEcho",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL); 

    xTaskCreate(
        vExtiTask,
        "EXTITask",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL); 
    
    xTaskCreate(
        vHardPWM,
        "HardPWM",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL); 

    vTaskStartScheduler();
    
    // Infinite loop
    for(;;) {}  
    return 0;
}