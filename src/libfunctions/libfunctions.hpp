#ifndef __LIBFUNCS_HPP__
#define __LIBFUNCS_HPP__

#include <string>
using namespace std;
typedef void (*library_func_t)(void);

void lib_print(void);
void lib_input(void);
void lib_objectmemberkeys(void);
void lib_objecttotalmembers(void);
void lib_objectcopy(void);
void lib_totalarguments(void);
void lib_argument(void);
void lib_typeof(void);
void lib_strtonum(void);
void lib_sqrt(void);
void lib_cos(void);
void lib_sin(void);

void avm_calllibfunc(string id);
library_func_t avm_getlibraryfunc(string id);
void register_libfunc(string id, library_func_t funcptr);
void execute_libFuncenter(void);
#endif