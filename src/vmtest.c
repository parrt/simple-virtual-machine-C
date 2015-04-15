
#include <stdio.h>
#include <time.h>
#include "vm.h"

int hello[] = {
    ICONST, 1234,
    PRINT,
    HALT
};

int loop[] = {
// .GLOBALS 2; N, I
// N = 10                      ADDRESS
        ICONST, 10,            // 0
        GSTORE, 0,             // 2
// I = 0
        ICONST, 0,             // 4
        GSTORE, 1,             // 6
// WHILE I<N:
// START (8):
        GLOAD, 1,              // 8
        GLOAD, 0,              // 10
        ILT,                   // 12
        BRF, 24,               // 13
//     I = I + 1
        GLOAD, 1,              // 15
        ICONST, 1,             // 17
        IADD,                  // 19
        GSTORE, 1,             // 20
        BR, 8,                 // 22
// DONE (24):
// PRINT "LOOPED "+N+" TIMES."
        HALT                   // 24
};

int main(int argc, char *argv[])
{
    int t1 = 0;
    int t2 = 0;
    vm_exec(hello, sizeof(hello), 0, 0, 0);
    t1 = (clock() / (CLOCKS_PER_SEC / 1000));
    vm_exec(loop, sizeof(loop), 0, 2, 0);
    t2 = (clock() / (CLOCKS_PER_SEC / 1000));
    printf("duration = %d ms\n", (t2 - t1));
    return 0;
}

