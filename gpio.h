/*
 * gpio.h
 *
 *  Created on: July 29, 2018
 *      Author: khughes
 *
 * GPIO register definitions.  See Chapter 10 of the TM4C1294 datasheet 
 * for complete information.
 */

#ifndef _GPIO_H
#define _GPIO_H

#include <stdint.h>

// Peripheral base addresses.
#define GPIO_PORTN              ((volatile uint32_t *)0x40064000) // reset button
#define GPIO_PORTD              ((volatile uint32_t *)0x4005B000) // 7 segment display
#define GPIO_PORTE              ((volatile uint32_t *)0x4005C000) // 7 segment display control
#define GPIO_PORTJ              ((volatile uint32_t *)0x40060000) // 7 segment display 2 control
#define GPIO_PORTL              ((volatile uint32_t *)0x40062000) // 7 segment display 2
#define GPIO_PORTH              ((volatile uint32_t *)0x4005F000) // keypad
#define GPIO_PORTC              ((volatile uint32_t *)0x4005A000) // keypad

// Peripheral register offsets and special fields
enum {
#define   GPIO_PIN_0              (1 << 0) // pin 0
#define   GPIO_PIN_1              (1 << 1) // pin 1
#define   GPIO_PIN_2              (1 << 2) // pin 2
#define   GPIO_PIN_3              (1 << 3) // pin 3
#define   GPIO_PIN_4              (1 << 4) // pin 4
#define   GPIO_PIN_5              (1 << 5) // pin 5
#define   GPIO_PIN_6              (1 << 6) // pin 6
#define   GPIO_PIN_7              (1 << 7) // pin 7
#define   GPIO_ALLPINS            0b11111111 // pins 0 to 7
  GPIO_DIR  =   (0x400 >> 2),
  GPIO_PUR  =   (0x510 >> 2),
  GPIO_DEN  =   (0x51c >> 2),
  GPIO_LOCK =   (0x520 >> 2),
  GPIO_CR   =   (0x524 >> 2),
  GPIO_ODR  =   (0x50C >> 2),
};

#endif // _GPIO_H
