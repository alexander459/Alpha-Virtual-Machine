#ifndef CONVERSIONS_H
#define CONVERSIONS_H

#include "../../structs.hpp"
#include "../../enums.hpp"
#include <assert.h>
#include <iostream>
#include <stdio.h>


avm_memcell *avm_translate_operand(vmarg *arg, avm_memcell *reg);


typedef bool (*tobool_func_t)(avm_memcell*);
bool number_tobool(avm_memcell *m);
bool string_tobool(avm_memcell *m);
bool bool_tobool(avm_memcell *m);
bool table_tobool(avm_memcell *m);
bool userfunc_tobool(avm_memcell *m);
bool libfunc_tobool(avm_memcell *m);
bool nil_tobool(avm_memcell *m);
bool undef_tobool(avm_memcell *m);

bool avm_tobool(avm_memcell *m);

string avm_tostring(avm_memcell* cell);
string avm_typetostring(avm_memcell* cell);

#endif