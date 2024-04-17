#ifndef _GPIO_PIN_YIYING_H_
#define _GPIO_PIN_YIYING_H_

//TODO: CHANGE PINS
//TODO: STM32 might be broken
/** keypad pins A0-A5, D2 */
#define COL1_PORT  0    // D7	PA_8
#define COL1_PIN   8
#define COL2_PORT  1    // D4	PB_5
#define COL2_PIN   5
#define COL3_PORT  2    // D9	PC_7
#define COL3_PIN   7

#define ROW1_PORT  1    // D6	PB_10
#define ROW1_PIN   10
#define ROW2_PORT  0    // D11	PA_7
#define ROW2_PIN   7
#define ROW3_PORT  1    // D10	PB_6
#define ROW3_PIN   6
#define ROW4_PORT  1    // D8	PA_9
#define ROW4_PIN   0

#define SERVO_PORT       1  // D4
#define SERVO_PIN        5 
#define GPIO_JP_PORT     1  // D6
#define GPIO_JP_PIN      10
#define MOTOR_EN_PORT    1  // D5
#define MORTO_EN_PIN     4 
#define ENC_B_PORT       0  // D7
#define ENC_B_PIN        8
#define ENC_A_PORT       0  // D8
#define ENC_A_PIN        9
#define BUTTON1_PORT     2  // D9
#define BUTTON1_PIN      7
#define BUTTON2_PORT     1  // D10
#define BUTTON2_PIN      6
#define MORTO_IN1_PORT   0  // D11
#define MORTO_IN1_PIN    7
#define MORTO_IN2_PORT   0  // D12
#define MORTO_IN2_PIN    6
#define LEDG_PORT        0  // D13
#define LEDG_PIN         5

#endif  // _GPIO_PIN_YIYING_H_