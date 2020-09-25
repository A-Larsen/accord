#include <stdio.h>
#include <stdarg.h>

void 
my_print(const char * pString, ...){
    va_list args;
    va_start(args, pString);
    if (uart_mode == UART_ON){
        vprintf(pString, args);
    }
    va_end(args);
}
