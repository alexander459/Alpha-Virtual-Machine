#ifndef __AVM_HPP__
#define __AVM_HPP__


#include "../../structs.hpp"
#include "../../enums.hpp"
#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <stdarg.h>

#define AVM_MAX_INSTRUCTIONS 22
#define AVM_STACKSIZE 4096
#define AVM_WIPEOUT(m) memset(&(m), 0, sizeof(m));
#define AVM_STACKENV_SIZE 4
#define YELLOW "\33[33m"
#define RED "\033[31m"
#define WHITE "\x1B[37m"

#define N AVM_STACKSIZE

extern vector<string> stringArray;
extern vector<string> libfuncArray;
extern vector<double> numberArray;
extern vector<program_func> progfuncArray;
extern vector<instruction> instructions;
extern unsigned totalActuals;

typedef void (*execute_func_t)(instruction*);
typedef void (*memclear_func_t)(avm_memcell*);


extern bool executionFinished;
extern unsigned codeSize;
extern unsigned pc;
extern int currLine;

extern avm_memcell avm_stack[AVM_STACKSIZE];  //THE STACK
extern avm_memcell ax, bx, cx, retval;        //REGISTERS

extern unsigned top;
extern unsigned topsp;       /*points το the beginning of a function environment*/
extern int globmem;
extern int callmem;     /*top - number of func args*/
extern int datamem;     /*callmem - number of func locals -2*/

/* INITIALIZES THE VM (STACK, PC ETC...) */
void init_avm(void);

void execute_cycle(void);

void avm_runtime_error(string msg, ...);
void avm_runtime_warning(string msg, ...);

avm_table *avm_tablenew(void);

avm_table *avm_tabledestroy(avm_table *t);

avm_memcell *avm_tablegetelem(avm_memcell *key);

avm_memcell *avm_tablesetelem(avm_memcell *key, avm_memcell *value);


string const_getstring(unsigned index);
double const_getnumber(unsigned index);
unsigned getfuncaddress(unsigned index);
string libfuncs_getused(unsigned index);
string getfuncid(unsigned index);
program_func avm_getfuncinfo(unsigned index);

void avm_memcellclear(avm_memcell* m);
void memclear_table(avm_memcell* m);

unsigned read_magic_number(FILE* file);

void reset_instruction(instruction* in);

void avm_callsaveenvironment(void);
void avm_dec_top(void);
unsigned avm_get_envvalue(unsigned i);
unsigned avm_totalactuals(void);
avm_memcell *avm_getactual(unsigned i);
int read_globmem(FILE* file);
vector<string> read_strings(FILE* file);
vector<program_func> read_functions(FILE* file);
vector<double> read_nums(FILE* file);
vector<instruction> read_instructions(FILE* file);


void set_globmem(int n);

bool avm_stack_full(void);

avm_memcell avm_stack_top(void);

void avm_stack_init(void);


string print_vmargs(vmarg arg, bool bin);
string getOpcode(vmopcode op);
void print_all(void);

void print_stack(void);
string print_table(avm_table* table);
#endif