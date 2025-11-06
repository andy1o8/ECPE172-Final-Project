#include "fsm.h"
#include "lock.h"
#include "display.h"
#include "keypad.h"
#include "timer.h"
#include "driverlib/gpio.h"

uint8_t stored_password[4] = {1,2,3,4};   // default 1234
bool    password_is_set   = true;
uint8_t code_buffer[4]    = {0xFF,0xFF,0xFF,0xFF};
uint8_t digits_entered    = 0;
lock_state_t lock_state   = STATE_IDLE;

/* debounce */
static uint8_t  key_debounce      = 0xFF;
static uint32_t key_debounce_time = 0;

/* timing markers */
static uint32_t state_start_time = 0;

static void set_unlock(bool on) {
    GPIOPinWrite(LED_PORT, LED_PIN, on ? 0 : LED_PIN);
}

static bool reset_pressed(void) {
    return (GPIOPinRead(RESET_PORT, RESET_PIN) == 0);
}

void FSM_Init(void) {
    Display_Clear();
    set_unlock(false);
    state_start_time = Timer_GetMs();
}

void FSM_Run(void)
{
    uint32_t now = Timer_GetMs();

    /* ------------------- KEYPAD INPUT (only in IDLE) ------------------- */
    if (lock_state == STATE_IDLE && digits_entered < 4) {
        uint8_t k = Keypad_Scan();

        if (k != 0xFF) {
            if (k == key_debounce) {
                if ((now - key_debounce_time) >= KEY_DEBOUNCE_TICKS) {
                    code_buffer[digits_entered++] = k;
                    for (uint8_t i = digits_entered; i < 4; ++i)
                        code_buffer[i] = 0xFF;
                    if (digits_entered == 4)
                        lock_state = STATE_CHECK_CODE;
                    key_debounce = 0xFF;
                }
            } else {
                key_debounce = k;
                key_debounce_time = now;
            }
        } else {
            key_debounce = 0xFF;
        }
    }

    /* -------------------------- FSM -------------------------- */
    switch (lock_state) {

        case STATE_IDLE:
            break;

        case STATE_CHECK_CODE:
            if (!password_is_set) {
                /* first entry after reset = new password */
                for (int i = 0; i < 4; ++i) stored_password[i] = code_buffer[i];
                password_is_set = true;
                Display_Clear();
                lock_state = STATE_IDLE;
            } else if (__builtin_memcmp(code_buffer, stored_password, 4) == 0) {
                set_unlock(true);
                state_start_time = now;
                lock_state = STATE_UNLOCKED;
            } else {
                Display_Clear();
                state_start_time = now;
                lock_state = STATE_WRONG_CODE;
            }
            break;

        case STATE_WRONG_CODE:
            if ((now - state_start_time) >= WRONG_DELAY_MS) {
                lock_state = STATE_IDLE;
            }
            break;

        case STATE_UNLOCKED:
            if ((now - state_start_time) >= UNLOCK_TIME_MS) {
                set_unlock(false);
                if (reset_pressed()) {
                    password_is_set = false;   // next 4 digits set new pw
                }
                Display_Clear();
                lock_state = STATE_IDLE;
            }
            break;

        case STATE_RELOCK_CHECK:   // not used – merged into UNLOCKED
        default:
            lock_state = STATE_IDLE;
            break;
    }

    /* keep display alive */
    Display_Update();
}