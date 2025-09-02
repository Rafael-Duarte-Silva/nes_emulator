#include "console.h"
#include "cpu.h"
#include <stdlib.h>

void init_console(console_t *console){
    console->CPU = NULL;
    console->cartrigde = NULL;
}