#ifndef _YIHE_BOARD_H_
#define _YIHE_BOARD_H_

//key
#define ROW0  GPIO_PB5
#define ROW1  GPIO_PB6
#define ROW2  GPIO_PC0
#define ROW3  GPIO_PA0
#define ROW4  GPIO_PB4

#define COL0  GPIO_PB7
#define COL1  GPIO_PC1
#define COL2  GPIO_PA1
#define COL3  GPIO_PC4

#define MAX_GPIO_KEYS 18

//dc
#define DCDC_CE	GPIO_PB1

//led
#define LED1  GPIO_PC5
#define LED2  GPIO_PC6
#define LED3  GPIO_PD2
#define LED4  GPIO_PD3
#define LED5  GPIO_PD4
#define LED6  GPIO_PD7

#define HAL_LED_ALL   (HAL_LED_1 | HAL_LED_2 | HAL_LED_3 | HAL_LED_4 | HAL_LED_5 | HAL_LED_6)
#define MAX_LEDS 6

#define MAX_GPIO_LEDS 6
#define MAX_KEYS      (MAX_GPIO_KEYS)

//adc
#define ADC_INPUT_PIN  GPIO_PB4
#endif
