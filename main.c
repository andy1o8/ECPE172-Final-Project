// main.c (new file)
#include "init.h"
#include "fsm.h"

int main(void) {
    Hardware_Init();
    FSM_Init();

    while (1) {
        FSM_Run();
    }
}