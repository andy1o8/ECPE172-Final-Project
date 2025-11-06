/*
 * keypad.h
 *
 *  Created on:  July 21, 2019
 *  Modified on: Oct 2, 2019 
 *  Modified on: Nov 18, 2020 
 *      Author: khughes
 */

#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>
#include <stdbool.h>

// Define the three keypad lookups here.
// To declare yours, go into the properties for the lab04 project and under Build->ARM Compiler->Predefined Symbols,
//    add the macro name in "Pre-define NAME".


const uint8_t keymap[4][4] = {
#ifdef KEYPAD_PHONE
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
#endif
};

void initKeypad( void );
bool getKey( uint8_t *col, uint8_t *row );

#endif //  KEYPAD_H
