/*
 * display.c
 *
 * 4-digit 7-segment display driver using:
 *   - PORTD: Segments for first 2-digit display
 *   - PORTE: Digit select for first display (PE0 = digit 0, PE1 = digit 1)
 *   - PORTK: Segments for second 2-digit display
 *   - PORTJ: Digit select for second display (PJ0 = digit 2, PJ1 = digit 3)
 *
 * Multiplexed refresh, ~1ms per digit, ~250 Hz per digit.
 */

#include "display.h"
#include "lock.h"
#include "gpio.h"
#include "sysctl.h"

// External segment patterns from led7seg.c
extern uint8_t displayData[];

// Buffer for 4 digits: 0-9 = display digit, 10 = blank
uint8_t code_buffer[4] = {10, 10, 10, 10};
uint8_t digits_entered = 0;

// Current digit being displayed (0-3)
static uint8_t current_digit = 0;

// Simple busy-wait delay (~1ms at 120 MHz)
static void short_delay(void) {
    volatile uint32_t cnt = 40000;  // ~1ms (calibrated for 120 MHz)
    while (cnt--) {}
}

void Display_Init(void) {
    // Enable clocks for PORTD, PORTE, PORTK, PORTJ
    SYSCTL[SYSCTL_RCGCGPIO] |= (SYSCTL_RCGCGPIO_PORTD |
                                SYSCTL_RCGCGPIO_PORTE |
                                SYSCTL_RCGCGPIO_PORTK |
                                SYSCTL_RCGCGPIO_PORTJ);
    // Second write for delay (required by TM4C)
    SYSCTL[SYSCTL_RCGCGPIO] |= (SYSCTL_RCGCGPIO_PORTD |
                                SYSCTL_RCGCGPIO_PORTE |
                                SYSCTL_RCGCGPIO_PORTK |
                                SYSCTL_RCGCGPIO_PORTJ);

    // Unlock PORTD and PORTK (required for PD7, PK7, etc.)
    GPIO_PORTD[GPIO_LOCK] = 0x4C4F434B;  // "UNLOCK"
    GPIO_PORTD[GPIO_CR]   = GPIO_ALLPINS;
    GPIO_PORTD[GPIO_LOCK] = 0;

    GPIO_PORTK[GPIO_LOCK] = 0x4C4F434B;
    GPIO_PORTK[GPIO_CR]   = GPIO_ALLPINS;
    GPIO_PORTK[GPIO_LOCK] = 0;

    // Configure segment pins as outputs
    GPIO_PORTD[GPIO_DIR] |= GPIO_ALLPINS;   // PD0-PD7
    GPIO_PORTK[GPIO_DIR] |= GPIO_ALLPINS;   // PK0-PK7

    // Configure digit select pins as outputs
    GPIO_PORTE[GPIO_DIR] |= (GPIO_PIN_0 | GPIO_PIN_1);  // PE0, PE1
    GPIO_PORTJ[GPIO_DIR] |= (GPIO_PIN_0 | GPIO_PIN_1);  // PJ0, PJ1

    // Enable digital function
    GPIO_PORTD[GPIO_DEN] |= GPIO_ALLPINS;
    GPIO_PORTK[GPIO_DEN] |= GPIO_ALLPINS;
    GPIO_PORTE[GPIO_DEN] |= (GPIO_PIN_0 | GPIO_PIN_1);
    GPIO_PORTJ[GPIO_DEN] |= (GPIO_PIN_0 | GPIO_PIN_1);

    // Initialize all off
    GPIO_PORTD[GPIO_ALLPINS] = 0;
    GPIO_PORTK[GPIO_ALLPINS] = 0;
    GPIO_PORTE[GPIO_PIN_0 | GPIO_PIN_1] = 0;
    GPIO_PORTJ[GPIO_PIN_0 | GPIO_PIN_1] = 0;

    // Clear display
    Display_Clear();
}

void Display_Update(void) {
    // Turn off all digit selects
    GPIO_PORTE[GPIO_PIN_0 | GPIO_PIN_1] = 0;
    GPIO_PORTJ[GPIO_PIN_0 | GPIO_PIN_1] = 0;

    // Get value to display
    uint8_t val = code_buffer[current_digit];
    uint8_t pattern = (val <= 9) ? displayData[val] : 0x00;  // 10+ = blank

    if (current_digit < 2) {
        // First display: PORTD (segments), PORTE (select)
        GPIO_PORTD[GPIO_ALLPINS] = pattern;
        uint8_t select_pin = (current_digit == 0) ? GPIO_PIN_0 : GPIO_PIN_1;
        GPIO_PORTE[select_pin] = select_pin;  // High to enable
    } else {
        // Second display: PORTK (segments), PORTJ (select)
        GPIO_PORTK[GPIO_ALLPINS] = pattern;
        uint8_t select_pin = ((current_digit - 2) == 0) ? GPIO_PIN_0 : GPIO_PIN_1;
        GPIO_PORTJ[select_pin] = select_pin;  // High to enable
    }

    short_delay();  // ~1ms on time

    // Turn off current digit
    if (current_digit < 2) {
        uint8_t select_pin = (current_digit == 0) ? GPIO_PIN_0 : GPIO_PIN_1;
        GPIO_PORTE[select_pin] = 0;
    } else {
        uint8_t select_pin = ((current_digit - 2) == 0) ? GPIO_PIN_0 : GPIO_PIN_1;
        GPIO_PORTJ[select_pin] = 0;
    }

    // Next digit
    current_digit = (current_digit + 1) % 4;
}

void Display_Clear(void) {
    uint8_t i = 0;
    for (i = 0; i < 4; ++i) {
        code_buffer[i] = 10;  // Blank
    }
    digits_entered = 0;
}

void Display_SetDigit(uint8_t pos, uint8_t value) {
    if (pos < 4 && value <= 9) {
        code_buffer[pos] = value;
    }
}
