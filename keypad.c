/*
 * keypad.c
 *
 *  Created on:  July 21, 2019
 *  Modified on: Aug 4, 2020
 *      Author: khughes
 *
 */

#include <stdint.h>
#include <stdbool.h>

// SYSCTL and GPIO definitions.
#include "ppb.h"
#include "sysctl.h"
#include "gpio.h"

// Prototypes for peripherals.
#include "osc.h"

// Initialize the specified row and column ports
void initKeypad(void) {
  // Enable port clocks
  SYSCTL[SYSCTL_RCGCGPIO] |= (SYSCTL_RCGCGPIO_PORTH | SYSCTL_RCGCGPIO_PORTC);
  SYSCTL[SYSCTL_RCGCGPIO] |= (SYSCTL_RCGCGPIO_PORTH | SYSCTL_RCGCGPIO_PORTC);

  // Configure row pins as outputs, disable pull-ups, and enable open-drain
  GPIO_PORTC[GPIO_DIR] |= (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
  GPIO_PORTC[GPIO_PUR] &= ~(GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
  GPIO_PORTC[GPIO_ODR] |= (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
  GPIO_PORTC[GPIO_DEN] |= (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

  // Configure column pins as inputs, enable pull-ups, and disable open-drain
  GPIO_PORTH[GPIO_DIR] &= ~(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
  GPIO_PORTH[GPIO_PUR] |= (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
  GPIO_PORTH[GPIO_ODR] &= ~(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
  GPIO_PORTH[GPIO_DEN] |= (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
}

// Check for a key in a row.  If a key is pressed, the procedure returns true and
// the row and column are set.  If a key is not pressed, the procedure returns
// false and row and columns don't matter.
bool getKey( uint8_t *col, uint8_t *row ) {
  static uint8_t currentRow = 0;
  static bool keyWasPressed = false;

  if (!keyWasPressed) {
    GPIO_PORTC[GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4] |= (GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4);
    GPIO_PORTC[(GPIO_PIN_7 >> currentRow)] = 0;
    *row = currentRow;
  }

  // Determine column number
  for (uint8_t i = 0; i < 4; i++) {
    if (!(GPIO_PORTH[GPIO_PIN_0 << i])) {
      *col = i;
      keyWasPressed = true;
      return true;
    }
  }

  keyWasPressed = false;
  currentRow = (currentRow + 1) & 0x03; // Cycle through 0-3
  return false;
}
