#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "vm.h"

#define TRACE_BEGIN    if (trace) vm_print_instr(vm->code, ip-1)
#define TRACE_END    	if (trace) vm_print_stack(vm->stack, sp);
#define RAW_DISPATCH	goto *dispatch[vm->code[ip++]]
#define DISPATCH    	TRACE_END; RAW_DISPATCH

typedef struct {
	char name[8];
	int nargs;
} VM_INSTRUCTION;

static VM_INSTRUCTION vm_instructions[] = {
	{"noop",   0},    // 0
	{"iadd",   0},    // 1
	{"isub",   0},    // 2
	{"imul",   0},    // 3
	{"ilt",    0},    // 4
	{"ieq",    0},    // 5
	{"br",     1},    // 7
	{"brt",    1},    // 8
	{"brf",    1},    // 9
	{"iconst", 1},    // 10
	{"load",   1},
	{"gload",  1},
	{"store",  1},
	{"gstore", 1},
	{"print",  0},
	{"pop",    0},
	{"call",   3},
	{"ret",    0},
	{"halt",   0}
};

static void vm_context_init(Context *ctx, int ip, int nlocals);

void vm_exec(VM *vm, int startip, bool trace) {
	// registers
	int ip;         // instruction pointer register
	int sp;         // stack pointer register
	int callsp;     // call stack pointer register

	int a = 0;
	int b = 0;
	int addr = 0;
	int offset = 0;

	ip = startip;
	sp = -1;
	callsp = -1;

	// See this for another example
	// http://eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables
	static void *dispatch[] = {
		&&interp_NOOP,
		&&interp_IADD,
		&&interp_ISUB,
		&&interp_IMUL,
		&&interp_ILT,
		&&interp_IEQ,
		&&interp_BR,
		&&interp_BRT,
		&&interp_BRF,
		&&interp_ICONST,
		&&interp_LOAD,
		&&interp_GLOAD,
		&&interp_STORE,
		&&interp_GSTORE,
		&&interp_PRINT,
		&&interp_POP,
		&&interp_CALL,
		&&interp_RET,
		&&interp_HALT
	};

	RAW_DISPATCH;                  // jump to first instruction interp code

interp_NOOP:
	TRACE_BEGIN;
	DISPATCH;

interp_IADD:
	TRACE_BEGIN;
	b = vm->stack[sp--];           // 2nd opnd at top of stack
	a = vm->stack[sp--];           // 1st opnd 1 below top
	vm->stack[++sp] = a + b;       // push result
	DISPATCH;

interp_ISUB:
	TRACE_BEGIN;
	b = vm->stack[sp--];
	a = vm->stack[sp--];
	vm->stack[++sp] = a - b;
	DISPATCH;

interp_IMUL:
	TRACE_BEGIN;
	b = vm->stack[sp--];
	a = vm->stack[sp--];
	vm->stack[++sp] = a * b;
	DISPATCH;

interp_ILT:
	TRACE_BEGIN;
	b = vm->stack[sp--];
	a = vm->stack[sp--];
	vm->stack[++sp] = (a < b) ? true : false;
	DISPATCH;

interp_IEQ:
	TRACE_BEGIN;
	b = vm->stack[sp--];
	a = vm->stack[sp--];
	vm->stack[++sp] = (a == b) ? true : false;
	DISPATCH;

interp_BR:
	TRACE_BEGIN;
	ip = vm->code[ip];
	DISPATCH;

interp_BRT:
	TRACE_BEGIN;
	addr = vm->code[ip++];
	if (vm->stack[sp--] == true) ip = addr;
	DISPATCH;

interp_BRF:
	TRACE_BEGIN;
	addr = vm->code[ip++];
	if (vm->stack[sp--] == false) ip = addr;
	DISPATCH;

interp_ICONST:
	TRACE_BEGIN;
	vm->stack[++sp] = vm->code[ip++];  // push operand
	DISPATCH;

interp_LOAD: // load local or arg; 1st local is fp+1, args are fp-3, fp-4, fp-5, ...
	TRACE_BEGIN;
	offset = vm->code[ip++];
	vm->stack[++sp] = vm->call_stack[callsp].locals[offset];
	DISPATCH;

interp_GLOAD: // load from global memory
	TRACE_BEGIN;
	addr = vm->code[ip++];
	vm->stack[++sp] = vm->globals[addr];
	DISPATCH;

interp_STORE:
	TRACE_BEGIN;
	offset = vm->code[ip++];
	vm->call_stack[callsp].locals[offset] = vm->stack[sp--];
	DISPATCH;

interp_GSTORE:
	TRACE_BEGIN;
	addr = vm->code[ip++];
	vm->globals[addr] = vm->stack[sp--];
	DISPATCH;

interp_PRINT:
	TRACE_BEGIN;
	printf("%d\n", vm->stack[sp--]);
	DISPATCH;

interp_POP:
	TRACE_BEGIN;
	--sp;
	DISPATCH;

interp_CALL:
	TRACE_BEGIN;
	// expects all args on stack
	addr = vm->code[ip++];            // index of target function
	int nargs = vm->code[ip++];    // how many args got pushed
	int nlocals = vm->code[ip++];    // how many locals to allocate
	++callsp; // bump stack pointer to reveal space for this call
	vm_context_init(&vm->call_stack[callsp], ip, nargs + nlocals);
	// copy args into new context
	for (int i = 0; i < nargs; i++) {
		vm->call_stack[callsp].locals[i] = vm->stack[sp - i];
	}
	sp -= nargs;
	ip = addr;        // jump to function
	DISPATCH;

interp_RET:
	TRACE_BEGIN;
	ip = vm->call_stack[callsp].returnip;
	callsp--; // pop context
	DISPATCH;

interp_HALT :
	TRACE_BEGIN;
	TRACE_END;
	return;
}

void vm_init(VM *vm, int *code, int code_size, int nglobals) {
	vm->code = code;
	vm->code_size = code_size;
	vm->globals = calloc(nglobals, sizeof(int));
	vm->nglobals = nglobals;
}

void vm_free(VM *vm) {
	free(vm->globals);
	free(vm);
}

VM *vm_create(int *code, int code_size, int nglobals) {
	VM *vm = calloc(1, sizeof(VM));
	vm_init(vm, code, code_size, nglobals);
	return vm;
}

static void vm_context_init(Context *ctx, int ip, int nlocals) {
	if (nlocals > DEFAULT_NUM_LOCALS) {
		fprintf(stderr, "too many locals requested: %d\n", nlocals);
	}
	ctx->returnip = ip;
}

void vm_print_instr(int *code, int ip) {
	int opcode = code[ip];
	VM_INSTRUCTION *inst = &vm_instructions[opcode];
	switch (inst->nargs) {
		case 0:
			printf("%04d:  %-20s", ip, inst->name);
			break;
		case 1:
			printf("%04d:  %-10s%-10d", ip, inst->name, code[ip + 1]);
			break;
		case 2:
			printf("%04d:  %-10s%d,%10d", ip, inst->name, code[ip + 1], code[ip + 2]);
			break;
		case 3:
			printf("%04d:  %-10s%d,%d,%-6d", ip, inst->name, code[ip + 1], code[ip + 2], code[ip + 3]);
			break;
	}
}

void vm_print_stack(int *stack, int count) {
	printf("stack=[");
	for (int i = 0; i <= count; i++) {
		printf(" %d", stack[i]);
	}
	printf(" ]\n");
}

void vm_print_data(int *globals, int count) {
	printf("Data memory:\n");
	for (int i = 0; i < count; i++) {
		printf("%04d: %d\n", i, globals[i]);
	}
}

