// timer.c
#include "timer.h"
#include "sysctl.h"   // Include if needed; assuming available
#include "lock.h"

void Timer_Init(void) {
    // Enable TIMER3 clock
    SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGCTIMER_R3;
    SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGCTIMER_R3;  // Delay
}

void Timer_StartOneShot(uint32_t ms) {
    uint32_t load = (uint64_t)ms * (SYS_CLOCK / 1000) - 1;  // Use uint64_t to avoid overflow

    GPTM_TIMER3[GPTM_CTL] = 0;                       // Disable
    GPTM_TIMER3[GPTM_CFG] = GPTM_CFG_32BITTIMER;
    GPTM_TIMER3[GPTM_TAMR] = GPTM_TAMR_TAMR_ONESHOT;
    GPTM_TIMER3[GPTM_TAILR] = load;
    GPTM_TIMER3[GPTM_TAPR] = 0;
    GPTM_TIMER3[GPTM_ICR] = GPTM_ICR_TATOCINT;      // Clear flag
    GPTM_TIMER3[GPTM_CTL] = GPTM_CTL_TAEN;          // Enable
}

bool Timer_Expired(void) {
    return (GPTM_TIMER3[GPTM_RIS] & GPTM_RIS_TATORIS) != 0;
}

void Timer_Clear(void) {
    GPTM_TIMER3[GPTM_ICR] = GPTM_ICR_TATOCINT;
}