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
#include "semphr.h"
#include <adc.h>
#include <stdio.h>
#include <stdlib.h>
#include <uart.h>
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
#include <encoder.h>
#include <motor_driver.h>

#define YUHONG
#ifdef YUHONG
#include "gpio_pin_yuhong.h"
#elif defined YIYING
#include "gpio_pin_yiying.h"
#endif

/** @brief define passcode */
int g_passcode = 349;
volatile int g_dutycycle = 16;
/** @brief define highest motor speed */
#define MAX_MOTOR_SPEED 90
/** @brief define lowest motor speed */
#define MIN_MOTOR_SPEED 10

/** @brief servo's states */
#define DEGREE_0 0
#define DEGREE_90 1
#define DEGREE_180 2

/** @brief clear the lcd */
void lcd_clear_quick() {
    lcd_set_cursor(0,0);
    lcd_print("                ");
    lcd_set_cursor(1,0);
    lcd_print("                ");
    lcd_set_cursor(0,0);
}

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
        // wait for 500 millisec
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
        if (1){
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
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/** @brief target positions for each button actuation */
const uint32_t target_positions[3] = {100, 500, 900};
volatile uint32_t target_position = 100; // Initial target position

/**
 * @brief  handle external interrupt task
 *
*/
void vExtiTask(void* pvParameters) {
    (void)pvParameters;
    // button
    gpio_init(BUTTON1_PORT, BUTTON1_PIN, MODE_INPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_PULL_UP, ALT0);
    gpio_init(BUTTON2_PORT, BUTTON2_PIN, MODE_INPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_PULL_UP, ALT0);
    // motor init
    motor_init(MORTO_IN1_PORT, MORTO_IN2_PORT, MOTOR_EN_PORT, MORTO_IN1_PIN, MORTO_IN2_PIN, MOTOR_EN_PIN, PWM_TIMER, PWM_TIMER_CHANNEL, MOTOR_INIT_ALT);
    
    // enable exti
    enable_exti(BUTTON1_PORT, BUTTON1_PIN, RISING_EDGE);
    enable_exti(BUTTON2_PORT, BUTTON2_PIN, RISING_EDGE);

    uint32_t current_index = 0;   // index for the target position

    while (1) {
        // BACKWARD
        if(exti_flag_backward){
            exti_flag_backward = 0; // Clear the interrupt flag
            // Decrement index safely
            if (current_index == 0) {   // hardcode the index decreasement
                current_index = 2;
            } else {
                current_index--;
            }
            target_position = target_positions[current_index];
            printf("Target_position = %ld\n", target_position);
        }
        // FORWARD
        if (exti_flag_forward) {
            exti_flag_forward = 0; // Clear the interrupt flag
            if (current_index == 2) {   // hardcode the index increasment
                current_index = 0;
            } else {
                current_index++;
            }
            target_position = target_positions[current_index];
            printf("Target_position = %ld\n", target_position);
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay to debounce the button
    }
}

/**
 * @brief  handle encoder task
 *
*/
void vEncoderMonitorTask(void* pvParameters) {
    (void)pvParameters;
    uint32_t motor_pos = 0;
    while(1) {
        motor_pos = encoder_read();
        printf("Motor_position = %ld\n", motor_pos);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/** @brief PID parameters */
typedef struct {
    float P;
    float I;
    float D;

    float integrator;
    float prevError;
    SemaphoreHandle_t mutex;
} PIDParameters;

/** @brief init PID parameters */
volatile PIDParameters pidParams = {1.01f, 0.18f, 0.09f, 0.0f, 0.0f, NULL};

/** @brief pid update function with simple algorithm */
float UpdatePID(PIDParameters *pid, int error, float deltaTime) {
    float pTerm, iTerm, dTerm;
    xSemaphoreTake(pid->mutex, portMAX_DELAY);
    // proportional term calculation
    pTerm = pid->P * error;
    // Intergral term calculation 
    pid->integrator += error * deltaTime;
    iTerm = pid->I * pid->integrator;
    // derivative term
    dTerm = pid->D * (error - pid->prevError) / deltaTime;

    pid->prevError = error;
    xSemaphoreGive(pid->mutex);
    float output = pTerm + iTerm + dTerm;
    return output;
}

/** @brief helper function of floating point abs val */
float absoluteValue(float x) {
    return (x < 0) ? -x : x;
}

/**
 * @brief PID controller task
 *
*/
void vPIDInputTask(void* pvParameters) {
    (void)pvParameters;
    char* pids[] = {"P", "I", "D"};
    // float* pid_val[] = {(float*)&pidParams.P, (float*)&pidParams.I, (float*)&pidParams.D};
    float new_val[3] = {pidParams.P, pidParams.I, pidParams.D};
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
                new_val[i] = atof(input);
            }
        }

        xSemaphoreTake(pidParams.mutex, portMAX_DELAY);
        pidParams.P = new_val[0];
        pidParams.I = new_val[1];
        pidParams.D = new_val[2];
        pidParams.integrator = 0.0f;
        pidParams.prevError = 0.0f;
        xSemaphoreGive(pidParams.mutex);

        lcd_clear_quick();
        char summary1[32];
        char summary2[32];
        snprintf(summary1, sizeof(summary1), "PID: P-%.2f", pidParams.P);
        snprintf(summary2, sizeof(summary2), "I-%.2f  D-%.2f", pidParams.I, pidParams.D);
        lcd_print(summary1);
        lcd_set_cursor(1,0);
        lcd_print(summary2);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void motorControlTask(void* pvParameters) {
    (void)pvParameters;
    while (1) {
        uint32_t curr_pos = encoder_read();
        int32_t error = target_position - curr_pos;
        float pid_output = UpdatePID((PIDParameters *)&pidParams, error, 0.01f);   // delta time is 1ms
        printf("pid_out = %f\n", pid_output);
        MotorDirection direction = pid_output >= 0 ? FORWARD : BACKWARD;
        uint32_t motor_speed = absoluteValue(pid_output);

        if (motor_speed > MAX_MOTOR_SPEED) {
            motor_speed = MAX_MOTOR_SPEED;
        } else if (motor_speed < MIN_MOTOR_SPEED) {
            motor_speed = MIN_MOTOR_SPEED;
        }

        motor_set_dir(MORTO_IN1_PORT, MORTO_IN2_PORT, MORTO_IN1_PIN, MORTO_IN2_PIN, PWM_TIMER, PWM_TIMER_CHANNEL, motor_speed, direction);
    
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// /** @brief calculate the shortest path */
// int32_t findBestpath(uint32_t current_position, uint32_t target_position) {
//     int32_t forward_path = (target_position - current_position + TICKS_PER_REV) % TICKS_PER_REV;
//     int32_t backward_path = (current_position - target_position + TICKS_PER_REV) % TICKS_PER_REV;
//     return (forward_path <= backward_path) ? forward_path : -backward_path;
// }

/**
 * @brief  handle servo task
 *
*/
void vServoTask(void *pvParameters) {
    (void)pvParameters;

    // SERVO 1
    int8_t servo_channle = 1; // yuhong channel 1, yiling channel 0
    gpio_init(SERVO_PORT, SERVO_PIN, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT0);
    servo_enable(servo_channle, 1);
    servo_set(servo_channle, 0); // initialized to lock state
    int32_t servo_state = DEGREE_0;
    int32_t last_servo_state = DEGREE_0;
    // int32_t servo_degree = 0;
    for (;;) {
        // when degree = 0
        if(servo_state == DEGREE_0){
            // turn to 90
            servo_set(servo_channle, 90);
            // update state
            servo_state = DEGREE_90;
            last_servo_state = DEGREE_0;
        }
        // when degree = 90, last_servo_state = 0
        else if((servo_state == DEGREE_90) && (last_servo_state == DEGREE_0)){
            // turn to 180
            servo_set(servo_channle, 180);
            // update state
            servo_state = DEGREE_180;
            last_servo_state = DEGREE_90;
        }
        // when degree = 180
        else if(servo_state == DEGREE_180){
            // turn to 90
            servo_set(servo_channle, 90);
            // update state
            servo_state = DEGREE_90;
            last_servo_state = DEGREE_180;
        }
        // when degree = 90, last_servo_state = 180
        else if((servo_state == DEGREE_90) &&  (last_servo_state == DEGREE_180)){
            // turn to 0
            servo_set(servo_channle, 0);
            // update state
            servo_state = DEGREE_0;
            last_servo_state = DEGREE_90;
        }
    
        vTaskDelay(pdMS_TO_TICKS(1000));
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
    pidParams.mutex = xSemaphoreCreateMutex();
    
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
        vPIDInputTask,
        "PIDInputTask",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL);

    xTaskCreate(
        motorControlTask,
        "motorControl",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL);
    
    xTaskCreate(
        vEncoderMonitorTask, 
        "EnocderMonitor", 
        configMINIMAL_STACK_SIZE, 
        NULL, 
        tskIDLE_PRIORITY + 1, 
        NULL);

    xTaskCreate(
        vServoTask, 
        "Servo", 
        configMINIMAL_STACK_SIZE, 
        NULL, 
        tskIDLE_PRIORITY + 1, 
        NULL);

    vTaskStartScheduler();
    
    // Infinite loop
    for(;;) {}  
    return 0;
}