#include "fsm.h"
#include "lock.h"
#include "display.h"
#include "keypad.h"   // Their keypad
#include "timer.h"
#include "gpio.h"
#include <string.h>   // For memcmp, memcpy

// Assuming keymap from keypad.h is chars '0'-'9' etc.
extern const uint8_t keymap[4][4];

uint8_t stored_password[4] = {1,2,3,4};   // default 1234
bool    password_is_set   = true;
uint8_t code_buffer[4]    = {10,10,10,10};  // 10=blank
uint8_t digits_entered    = 0;
lock_state_t lock_state   = STATE_IDLE;

/* debounce */
static uint8_t pending_key = 0xFF;
static bool debouncing = false;

static void set_unlock(bool on) {
    GPIO_PORTN[LED_PIN] = on ? 0 : LED_PIN;  // Active low
}

static bool reset_pressed(void) {
    return (GPIO_PORTN[RESET_PIN] == 0);
}

void FSM_Init(void) {
    Display_Clear();
    set_unlock(false);
}

void FSM_Run(void) {
    /* ------------------- KEYPAD INPUT (only in IDLE) ------------------- */
    if (lock_state == STATE_IDLE && digits_entered < 4) {
        uint8_t col, row;
        if (debouncing) {
            if (Timer_Expired()) {
                Timer_Clear();
                // Check if still pressed
                uint8_t c, r;
                if (getKey(&c, &r)) {
                    uint8_t k = keymap[r][c];
                    if (k >= '0' && k <= '9' && (k - '0') == pending_key) {
                        // Accept
                        code_buffer[digits_entered++] = pending_key;
                        for (uint8_t i = digits_entered; i < 4; ++i)
                            code_buffer[i] = 10;  // Blank
                        if (digits_entered == 4)
                            lock_state = STATE_CHECK_CODE;
                    }
                }
                debouncing = false;
                pending_key = 0xFF;
            }
        } else {
            if (getKey(&col, &row)) {
                uint8_t k = keymap[row][col];
                if (k >= '0' && k <= '9') {
                    pending_key = k - '0';
                    debouncing = true;
                    Timer_StartOneShot(KEY_DEBOUNCE_MS);
                }
            }
        }
    }

    /* -------------------------- FSM -------------------------- */
    switch (lock_state) {

        case STATE_IDLE:
            break;

        case STATE_CHECK_CODE:
            if (!password_is_set) {
                for (int i = 0; i < 4; ++i) stored_password[i] = code_buffer[i];
                password_is_set = true;
                Display_Clear();
                lock_state = STATE_IDLE;
            } else if (memcmp(code_buffer, stored_password, 4) == 0) {
                set_unlock(true);
                Timer_StartOneShot(UNLOCK_TIME_MS);
                lock_state = STATE_UNLOCKED;
            } else {
                Display_Clear();
                Timer_StartOneShot(WRONG_DELAY_MS);
                lock_state = STATE_WRONG_CODE;
            }
            break;

        case STATE_WRONG_CODE:
            if (Timer_Expired()) {
                Timer_Clear();
                lock_state = STATE_IDLE;
            }
            break;

        case STATE_UNLOCKED:
            if (Timer_Expired()) {
                Timer_Clear();
                set_unlock(false);
                if (reset_pressed()) {
                    password_is_set = false;   // next 4 digits set new pw
                }
                Display_Clear();
                lock_state = STATE_IDLE;
            }
            break;

        case STATE_RELOCK_CHECK:   // Merged
        default:
            lock_state = STATE_IDLE;
            break;
    }

    /* Refresh display */
    Display_Update();
}