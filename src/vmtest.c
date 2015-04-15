#include <stdio.h>
#include <stdbool.h>
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

const int FACTORIAL_ADDRESS = 0;
int factorial[] = {
//.def factorial: ARGS=1, LOCALS=0	ADDRESS
//	IF N < 2 RETURN 1
	LOAD, 0,                // 0
	ICONST, 2,              // 2
	ILT,                    // 4
	BRF, 10,                // 5
	ICONST, 1,              // 7
	RET,                    // 9
//CONT:
//	RETURN N * FACT(N-1)
	LOAD, 0,                // 10
	LOAD, 0,                // 12
	ICONST, 1,              // 14
	ISUB,                   // 16
	CALL, FACTORIAL_ADDRESS, 1, 0,    // 17
	IMUL,                   // 21
	RET,                    // 22
//.DEF MAIN: ARGS=0, LOCALS=0
// PRINT FACT(1)
	ICONST, 5,              // 23    <-- MAIN METHOD!
	CALL, FACTORIAL_ADDRESS, 1, 0,    // 25
	PRINT,                  // 29
	HALT                    // 30
};

static int f[] = {
	//								ADDRESS
	//.def main() { print f(10); }
	ICONST, 10,             // 0
	CALL, 8, 1, 1,          // 2
	PRINT,                  // 6
	HALT,                   // 7
	//.def f(x): ARGS=1, LOCALS=1
	//  a = x;
	LOAD, 0,                // 8	<-- start of f
	STORE, 1,
	// return 2*a
	LOAD, 1,
	ICONST, 2,
	IMUL,
	RET
};

int main(int argc, char *argv[]) {
	VM *vm = vm_create(hello, sizeof(hello), 0);
	vm_exec(vm, 0, false);
	vm_free(vm);

//    int t1 = (clock() / (CLOCKS_PER_SEC / 1000));
	vm = vm_create(loop, sizeof(loop), 2);
	vm_exec(vm, 0, false);
	vm_print_data(vm->globals, vm->nglobals);
	vm_free(vm);

//    int t2 = (clock() / (CLOCKS_PER_SEC / 1000));

	vm = vm_create(factorial, sizeof(factorial), 0);
	vm_exec(vm, 23, false);
	vm_free(vm);

	vm = vm_create(f, sizeof(f), 0);
	vm_exec(vm, 0, false);
	vm_free(vm);

//    printf("duration = %d ms\n", (t2 - t1));

	return 0;
}

