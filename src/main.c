/**
 * @file main.c
 *
 * @brief main function and some helper functions of lab 6
 *
 * @date 04/16/2024
 *
 * @author Yuhong Yao (yuhongy), Yiying Li (yiyingl4)
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
#include <encoder.h>
#include <motor_driver.h>

#define YUHONG
#ifdef YUHONG
#include "gpio_pin_yuhong.h"
#elif defined YIYING
#include "gpio_pin_yiying.h"
#endif


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

/**
 * @brief  handle the duty cycle change
 *
*/
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
   
    gpio_init(LEDG_PORT, LEDG_PIN, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT0);

    for(;;) {
        if (gpio_read(LEDG_PORT, LEDG_PIN)) {
            gpio_clr(LEDG_PORT, LEDG_PIN);
        } else {
            gpio_set(LEDG_PORT, LEDG_PIN);
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

/**
 * @brief  handle +++ command: enter the command mode
 *
*/
void escapeSequenceTask(void *pvParameters) {
    (void)pvParameters;
    char byte;
    while (1) {
        if (!isInCommandMode) {
            if (uart_get_byte(&byte) == 0) { 
                // Directly pass each byte to atcmd_detect_escape
                if (atcmd_detect_escape(NULL, byte)) {
                    isInCommandMode = 1;
                    printf("Entering Command Mode.\n");
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/**
 * @brief  handle external interrupt task
 *
*/
void vExtiTask(void* pvParameters) {
    (void)pvParameters;
    // button
    gpio_init(BUTTON1_PORT, BUTTON1_PIN, MODE_INPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_PULL_UP, ALT0);
    // LED
    gpio_init(GPIO_JP_PORT, GPIO_JP_PIN, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT0);
    // button enable exti
    enable_exti(BUTTON1_PORT, BUTTON1_PIN, FALLING_EDGE);
    while (1) {
        // the exti_flag is from exti
        if (exti_flag) {
            exti_flag = 0;
            // toggle LED
            if (gpio_read(GPIO_JP_PORT, GPIO_JP_PIN)) {
                gpio_clr(GPIO_JP_PORT, GPIO_JP_PIN);
            } else {
                gpio_set(GPIO_JP_PORT, GPIO_JP_PIN);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief  handle encoder task
 *
*/
void vEncoderMonitorTask(void* pvParameters) {
    (void)pvParameters;
    encoder_init();

    while(1) {
        uint32_t enc_read = encoder_read();
        printf("encoder_read = %ld\n", enc_read);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief  handle Hardware-driven PWM task
 *
*/
void vHardPWM(void* pvParameters) {
    (void)pvParameters;
    gpio_init(GPIO_B, 4, MODE_ALT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT2);
    timer_start_pwm(3, 1, 100, 16, g_dutycycle);
    for (;;) {
        timer_set_duty_cycle(3, 1, g_dutycycle);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/** @brief PID parameters */
typedef struct {
    float P;
    float I;
    float D;
} PIDParameters;

/** @brief init PID parameters */
volatile PIDParameters pidParams = {0.0f, 0.0f, 0.0f};

/** @brief clear the lcd */
void lcd_clear_quick() {
    lcd_set_cursor(0,0);
    lcd_print("                ");
    lcd_set_cursor(1,0);
    lcd_print("                ");
    lcd_set_cursor(0,0);
}

/**
 * @brief PID controller task
 *
*/
void vPIDTask(void* pvParameters) {
    (void)pvParameters;
    char* pids[] = {"P", "I", "D"};
    float* pid_val[] = {(float*)&pidParams.P, (float*)&pidParams.I, (float*)&pidParams.D};
    char input[16];
    int index = 0;

    lcd_driver_init();
    for (;;) {
        for (int i = 0; i < 3; i++) {
            index = 0;
            memset(input, 0, sizeof(input));
            lcd_clear_quick();
            lcd_print("Enter ");
            lcd_print(pids[i]);
            lcd_print(":");
            lcd_set_cursor(1,0);
            
            char key = '\0';
            while (key != '#') {
                key = keypad_read();
                if (key != '\0') {
                    if (((key >= '0' && key <= '9') || key == '*' || key == '#') && index < 15) {
                        if (key == '*') {
                            input[index++] = '.';
                            lcd_print("."); 
                        } else if (key >= '0' && key <= '9') {
                            input[index++] = key;
                            char toPrint[2] = {key, '\0'};
                            lcd_print(toPrint);
                        }
                    }
                    if (key == '#') {
                        break; // Exit the loop to process the input
                    }
                    key = '\0'; // Reset key to avoid infinite loop
                    vTaskDelay(pdMS_TO_TICKS(50)); // Debounce delay
                }
            }

            if (index > 0) {
                *pid_val[i] = atof(input);
            }
        }

        lcd_clear_quick();
        char summary1[32];
        char summary2[32];
        snprintf(summary1, sizeof(summary1), "PID: P-%.2f", pidParams.P);
        snprintf(summary2, sizeof(summary2), "I-%.2f  D-%.2f", pidParams.I, pidParams.D);
        lcd_print(summary1);
        lcd_set_cursor(1,0);
        lcd_print(summary2);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }

}

/**
 * @brief Motor control task
 *
*/
void vMotorTask(void* pvParameters){
    (void)pvParameters;
    // Initialize the motor
    // motor_init(gpio_port port_a, gpio_port port_b, gpio_port port_pwm, uint32_t channel_a, uint32_t channel_b, uint32_t channel_pwm, uint32_t timer, uint32_t timer_channel, uint32_t alt_timer)
    motor_init(MORTO_IN1_PORT, MORTO_IN2_PORT, MOTOR_EN_PORT, MORTO_IN1_PIN, MORTO_IN2_PIN, MOTOR_EN_PIN, 3, 1, ALT2);

    while (1) {
        // move forward
        // void motor_set_dir(gpio_port port_a, gpio_port port_b, uint32_t channel_a, uint32_t channel_b, uint32_t timer, uint32_t timer_channel, uint32_t duty_cycle, MotorDirection direction)
        motor_set_dir(MORTO_IN1_PORT, MORTO_IN2_PORT, MORTO_IN1_PIN, MORTO_IN2_PIN, 2, 1, 30, FORWARD);
        vTaskDelay(pdMS_TO_TICKS(100));

        // // move backward
        // motor_set_dir(MORTO_IN1_PORT, MORTO_IN2_PORT, MORTO_IN1_PIN, MORTO_IN2_PIN, 2, 1, 16, BACKWARD);
        // printf("Motor moving BACKWARD at 16 duty cycle\n");

        // // Stop the motor
        // motor_set_dir(MORTO_IN1_PORT, MORTO_IN2_PORT, MORTO_IN1_PIN, MORTO_IN2_PIN, 2, 1, 0, STOP);
        // printf("Motor STOPPED\n");

        // // Free the motor
        // motor_set_dir(MORTO_IN1_PORT, MORTO_IN2_PORT, MORTO_IN1_PIN, MORTO_IN2_PIN, 2, 1, 0, FREE);
        // printf("Motor is FREE\n");

        // // Print motor position
        // uint32_t pos = motor_position();
        // printf("Current Motor Position: %lu\n", pos);
        // vTaskDelay(pdMS_TO_TICKS(100));
    }
}


/**
 * @brief main funtion
 *
*/
int main( void ) {
    uart_init(115200);
    keypad_init();
    i2c_master_init(80);
    atcmd_parser_init(&parser, commands, (sizeof(commands) / sizeof(commands[0])));
    
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
        vPIDTask,
        "PIDTask",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL);
    
    xTaskCreate(
        escapeSequenceTask, 
        "ENTERCommand", 
        configMINIMAL_STACK_SIZE, 
        NULL, 
        tskIDLE_PRIORITY + 1, 
        NULL);
    
    // xTaskCreate(
    //     vHardPWM,
    //     "HardPWM",
    //     configMINIMAL_STACK_SIZE,
    //     NULL,
    //     tskIDLE_PRIORITY + 1,
    //     NULL); 

    // xTaskCreate(
    //     vEncoderMonitorTask, 
    //     "EnocderMonitor", 
    //     configMINIMAL_STACK_SIZE, 
    //     NULL, 
    //     tskIDLE_PRIORITY + 1, 
    //     NULL);

    xTaskCreate(
        vMotorTask, 
        "Motor", 
        configMINIMAL_STACK_SIZE, 
        NULL, 
        tskIDLE_PRIORITY + 1, 
        NULL);

    vTaskStartScheduler();
    
    // Infinite loop
    for(;;) {}  
    return 0;
}