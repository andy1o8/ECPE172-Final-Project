// timer.h (new file: timer helpers using GPTM_TIMER3)
#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void Timer_Init(void);
void Timer_StartOneShot(uint32_t ms);  // Start one-shot timer for ms
bool Timer_Expired(void);             // Check if timeout occurred (polled)
void Timer_Clear(void);               // Clear the flag

#endif /* TIMER_H */