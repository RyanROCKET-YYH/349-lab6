#ifndef _GPIO_PIN_YUHONG_H_
#define _GPIO_PIN_YUHONG_H_

/** keypad pins A0-A5, D2 */
#define COL1_PORT  0    // A2   PA_4
#define COL1_PIN   4
#define COL2_PORT  0    // A0   PA_0
#define COL2_PIN   0
#define COL3_PORT  2    // A4   PC_1
#define COL3_PIN   1

#define ROW1_PORT  0    // A1   PA_1
#define ROW1_PIN   1
#define ROW2_PORT  0    // D2   PA_10
#define ROW2_PIN   10
#define ROW3_PORT  2    // A5   PC_0
#define ROW3_PIN   0
#define ROW4_PORT  1    // A3   PB_0
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

#endif  // _GPIO_PIN_YUHONG_H_