
#ifndef VM_H_
#define VM_H_

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_STACK_SIZE      1000
#define DEFAULT_CALL_STACK_SIZE 100
#define DEFAULT_NUM_LOCALS      10

typedef enum {
    NOOP    = 0,
    IADD    = 1,   // int add
    ISUB    = 2,
    IMUL    = 3,
    ILT     = 4,   // int less than
    IEQ     = 5,   // int equal
    BR      = 6,   // branch
    BRT     = 7,   // branch if true
    BRF     = 8,   // branch if true
    ICONST  = 9,   // push constant integer
    LOAD    = 10,  // load from local context
    GLOAD   = 11,  // load from global memory
    STORE   = 12,  // store in local context
    GSTORE  = 13,  // store in global memory
    PRINT   = 14,  // print stack top
    POP     = 15,  // throw away top of stack
    CALL    = 16,  // call function at address with nargs,nlocals
    RET     = 17,  // return value from function
    HALT    = 18
} VM_CODE;

typedef struct {
    int returnip;
    int locals[DEFAULT_NUM_LOCALS];
} Context;

typedef struct {
    int *code;
    int code_size;

    // global variable space
    int *globals;
    int nglobals;

    // Operand stack, grows upwards
    int stack[DEFAULT_STACK_SIZE];
    Context call_stack[DEFAULT_CALL_STACK_SIZE];
} VM;

VM *vm_create(int *code, int code_size, int nglobals);
void vm_init(VM *vm, int *code, int code_size, int nglobals);
void vm_exec(VM *vm, int startip, bool trace);
void vm_print_instr(int *code, int ip);
void vm_print_stack(int *stack, int count);
void vm_print_data(int *globals, int count);

#ifdef __cplusplus
}
#endif

#endif
