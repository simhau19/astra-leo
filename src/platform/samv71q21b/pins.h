#ifndef PINS_H
#define PINS_H

#include <hal_gpio.h>

// SAMV71 has 4 pin functions
#define GPIO_PIN_FUNCTION_A 0
#define GPIO_PIN_FUNCTION_B 1
#define GPIO_PIN_FUNCTION_C 2
#define GPIO_PIN_FUNCTION_D 3

#define PA3  GPIO(GPIO_PORTA, 3)
#define PA4  GPIO(GPIO_PORTA, 4)
#define PA21 GPIO(GPIO_PORTA, 21)
#define PB4  GPIO(GPIO_PORTB, 4)
#define PD0  GPIO(GPIO_PORTD, 0)
#define PD1  GPIO(GPIO_PORTD, 1)
#define PD2  GPIO(GPIO_PORTD, 2)
#define PD3  GPIO(GPIO_PORTD, 3)
#define PD4  GPIO(GPIO_PORTD, 4)
#define PD5  GPIO(GPIO_PORTD, 5)
#define PD6  GPIO(GPIO_PORTD, 6)
#define PD7  GPIO(GPIO_PORTD, 7)
#define PD8  GPIO(GPIO_PORTD, 8)
#define PD9  GPIO(GPIO_PORTD, 9)

#define PHY_RESET_PIN GPIO(GPIO_PORTC, 10)
#define LED0          GPIO(GPIO_PORTA, 23)
#define LED1          GPIO(GPIO_PORTC, 9)
#define SW0           GPIO(GPIO_PORTA, 9)
#define SW1           GPIO(GPIO_PORTB, 12)

#endif // PINS_H
