// init.c (new file)
#include "init.h"
#include "display.h"
#include "keypad.h"
#include "timer.h"
#include "sysctl.h"
#include "gpio.h"

void SysCtlClockSet_120MHz(void);

void Hardware_Init(void) {
    // Set system clock if needed (assuming osc.h sets it to 120 MHz; otherwise add PLL config)

    // Init peripherals
    initKeypad();      // Their keypad init
    Display_Init();    // Extended 4-digit
    Timer_Init();

    // Init PORTN for reset and LED
    SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTN;
    SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTN;  // Delay

    // Unlock if needed
    GPIO_PORTN[GPIO_LOCK] = 0x4c4f434b;
    GPIO_PORTN[GPIO_CR] = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_PORTN[GPIO_LOCK] = 0;

    // Reset: input, pull-up
    GPIO_PORTN[GPIO_DIR] &= ~RESET_PIN;
    GPIO_PORTN[GPIO_PUR] |= RESET_PIN;
    GPIO_PORTN[GPIO_DEN] |= RESET_PIN;

    // LED: output
    GPIO_PORTN[GPIO_DIR] |= LED_PIN;
    GPIO_PORTN[GPIO_DEN] |= LED_PIN;
    GPIO_PORTN[LED_PIN] = LED_PIN;  // Off (locked, high)
}
