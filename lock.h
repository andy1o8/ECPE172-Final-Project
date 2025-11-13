// lock.h (new file: shared definitions)
#ifndef LOCK_H
#define LOCK_H

#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"
#include "gptm.h"
#include "led7seg.h"

// System clock frequency (adjust if different; assuming PLL 120 MHz set elsewhere)
#define SYS_CLOCK 120000000UL

// Pins for reset button and unlock LED (assuming PORTN PIN0=reset active low, PIN1=LED active low)
#define RESET_PIN   GPIO_PIN_0
#define LED_PIN     GPIO_PIN_1

// Timing constants (ms)
#define UNLOCK_TIME_MS      5000
#define WRONG_DELAY_MS      1000
#define KEY_DEBOUNCE_MS     20

// Globals
extern uint8_t stored_password[4];
extern bool    password_is_set;
extern uint8_t code_buffer[4];
extern uint8_t digits_entered;

typedef enum {
    STATE_IDLE,
    STATE_CHECK_CODE,
    STATE_WRONG_CODE,
    STATE_UNLOCKED,
    STATE_RELOCK_CHECK
} lock_state_t;

extern lock_state_t lock_state;

#endif /* LOCK_H */