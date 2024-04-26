#ifndef _GPIO_PIN_YIYING_H_
#define _GPIO_PIN_YIYING_H_

//TODO: CHANGE PINS
//TODO: STM32 might be broken
/** keypad pins A0-A5, D2 */
// #define COL1_PORT  0    // D7	PA_8
// #define COL1_PIN   8
// #define COL2_PORT  1    // D4	PB_5
// #define COL2_PIN   5
// #define COL3_PORT  2    // D9	PC_7
// #define COL3_PIN   7

// #define ROW1_PORT  1    // D6	PB_10
// #define ROW1_PIN   10
// #define ROW2_PORT  0    // D11	PA_7
// #define ROW2_PIN   7
// #define ROW3_PORT  1    // D10	PB_6
// #define ROW3_PIN   6
// #define ROW4_PORT  0    // D8	PA_9
// #define ROW4_PIN   9
#define COL1_PORT  0 // D7(PA8)
#define COL1_PIN   8
#define COL2_PORT  1 // D4(PB5) //TODO:
#define COL2_PIN   5
#define COL3_PORT  2 // D9(PC7)
#define COL3_PIN   7

#define ROW1_PORT  1 // D6(PB10)
#define ROW1_PIN   10
#define ROW2_PORT  0 // D11(PA7)
#define ROW2_PIN   7
#define ROW3_PORT  1 // D10(PB6)
#define ROW3_PIN   6
#define ROW4_PORT  0 // D8(PA9)
#define ROW4_PIN   9

#define SERVO_PORT       1  // A5(PC_0) TODO: //new: A3(PB0)
#define SERVO_PIN        0 
#define TIM_SERVO        3
#define TIM_channel_SERVO        3

#define GPIO_JP_PORT     0  // Green LED: PA_10(0, 10)
#define GPIO_JP_PIN      10

#define BUTTON1_PORT     2  // A3(PB0) // BUTTON for exti TODO: //new: A5(PC_0)
#define BUTTON1_PIN      0
#define BUTTON2_PORT     0  // A2(PA_4)
#define BUTTON2_PIN      4

// encoder
#define ENC_B_PORT       0  // D12 (PA_6)
#define ENC_B_PIN        6
#define ENC_A_PORT       0  // D13 (PA_5)
#define ENC_A_PIN        5

// motor
#define MOTOR_EN_PORT    1  // D3 (PB_3)
#define MOTOR_EN_PIN     3 
#define MORTO_IN1_PORT   0  // A0(PA_0)
#define MORTO_IN1_PIN    0
#define MORTO_IN2_PORT   0  // A1(PA_1)
#define MORTO_IN2_PIN    1
#define PWM_TIMER         2
#define PWM_TIMER_CHANNEL 2
#define MOTOR_INIT_ALT 1 // ALT1

#define LEDG_PORT        2  // A4(PC_1) //Red LED: (2_1)
#define LEDG_PIN         1

#endif  // _GPIO_PIN_YIYING_H_