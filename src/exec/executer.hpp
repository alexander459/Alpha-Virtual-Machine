#ifndef __EXECUTER_HPP__
#define __EXECUTER_HPP__
#include "../../structs.hpp"
#include <iostream>
#include "../conversions/conversions.hpp"
#include "../main/avm.hpp"
#include <assert.h>
using namespace std;


void execute_assign(instruction* in);
void avm_assign(avm_memcell* lv, avm_memcell* rv);
void memcellcopy(avm_memcell* arg1, avm_memcell* arg2);
void copyNum(avm_memcell* arg1, avm_memcell* arg2);
void copyStr(avm_memcell* arg1, avm_memcell* arg2);
void copyBool(avm_memcell* arg1, avm_memcell* arg2);
void copyTable(avm_memcell* arg1, avm_memcell* arg2);
void copyUserfunc(avm_memcell* arg1, avm_memcell* arg2);
void copyLibfunc(avm_memcell* arg1, avm_memcell* arg2);
void copyNil(avm_memcell* arg1, avm_memcell* arg2);
typedef void (*copy_funcs)(avm_memcell* arg1, avm_memcell* arg2);


void execute_add(avm_memcell* arg1, avm_memcell* arg2, avm_memcell* result);
void execute_sub(avm_memcell* arg1, avm_memcell* arg2, avm_memcell* result);
void execute_mul(avm_memcell* arg1, avm_memcell* arg2, avm_memcell* result);
void execute_div(avm_memcell* arg1, avm_memcell* arg2, avm_memcell* result);
void execute_mod(avm_memcell* arg1, avm_memcell* arg2, avm_memcell* result);
void execute_uminus(avm_memcell* arg1, avm_memcell* arg2, avm_memcell* result);
void execute_arithmeticOp(instruction* in);
typedef void (*call_op_funcs_t)(avm_memcell* arg1, avm_memcell* arg2, avm_memcell* result);

void execute_jump(instruction* in);
void execute_jeq(instruction* in);
void execute_jne(instruction* in);
void execute_jle(instruction* in);
void execute_jge(instruction* in);
void execute_jlt(instruction* in);
void execute_jgt(instruction* in);

void execute_call(instruction* in);
void execute_pusharg(instruction* in);
void execute_jge(instruction* in);
void execute_funcenter(instruction* in);
void execute_funcexit(instruction*);

void execute_newtable(instruction* in);
void execute_tablegetelem(instruction* in);
void execute_tablesetelem(instruction* in);
avm_memcell* avm_tablegetelem(avm_table* table, avm_memcell* index);
void avm_tablesetelem(avm_table* table, avm_memcell* index, avm_memcell* value);
void avm_tableincrefcounter(avm_table* table);
void execute_nop(instruction* in);

typedef bool (*comp_func_t)(avm_memcell*, avm_memcell*, vmopcode op);
bool compareNumbers(avm_memcell* arg1, avm_memcell* arg2, vmopcode op);
bool compareStrings(avm_memcell* arg1, avm_memcell* arg2, vmopcode op);
bool compareTables(avm_memcell* arg1, avm_memcell* arg2, vmopcode op);
bool compareUserFunc(avm_memcell* arg1, avm_memcell* arg2, vmopcode op);
bool compareLibFunc(avm_memcell* arg1, avm_memcell* arg2, vmopcode op);

void callEqualityOperation(instruction* in, vmopcode op);
void callComparisonOperation(instruction* in, vmopcode op);

/* ##########TABLE_GET###########*/

avm_memcell* get_num_indexed(avm_memcell* index,avm_table* table);
avm_memcell* get_str_indexed(avm_memcell* index,avm_table* table);
avm_memcell* get_usrf_indexed(avm_memcell* index,avm_table* table);
avm_memcell* get_libf_indexed(avm_memcell* index,avm_table* table);
avm_memcell* get_table_indexed(avm_memcell* index,avm_table* table);
avm_memcell*  get_bool_indexed(avm_memcell* index,avm_table* table);

avm_memcell* erase_num_indexed(avm_memcell* index,avm_table* table);
avm_memcell* erase_str_indexed(avm_memcell* index,avm_table* table);
avm_memcell* erase_usrf_indexed(avm_memcell* index,avm_table* table);
avm_memcell* erase_libf_indexed(avm_memcell* index,avm_table* table);
avm_memcell* erase_table_indexed(avm_memcell* index,avm_table* table);
avm_memcell* erase_bool_indexed(avm_memcell* index,avm_table* table);  
    
    
#endif