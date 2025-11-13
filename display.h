#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

void Display_Init(void);
void Display_Update(void);
void Display_Clear(void);
void Display_SetDigit(uint8_t pos, uint8_t value);  // pos: 0-3, value: 0-9

#endif /* DISPLAY_H */