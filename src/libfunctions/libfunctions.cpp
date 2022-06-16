#include "libfunctions.hpp"
#include "../main/avm.hpp"
#include "../exec/executer.hpp"
#include <string>
#include <map>
#include <cmath>
#include "../main/avm.hpp"

using namespace std;

map<string, library_func_t> libfunc_map;
bool is_double(string str);
bool is_int(string s);

library_func_t libFunctions[]={
    lib_print,
    lib_input,
    lib_objectmemberkeys,
    lib_objecttotalmembers,
    lib_objectcopy,
    lib_totalarguments,
    lib_argument,
    lib_typeof,
    lib_strtonum,
    lib_sqrt,
    lib_cos,
    lib_sin
};


void lib_print(void){
    execute_libFuncenter();
    unsigned n=avm_totalactuals();
    avm_memcell* tmp;
    string msg;
    for(unsigned i=0; i<n; i++){
        tmp=avm_getactual(i);
        assert(tmp);
        if(tmp->type==userfunc_m)
            cout << "user function: ";
        else if(tmp->type==libfunc_m)
            cout << "libraty function: ";
        msg=avm_tostring(tmp);
        cout << msg;
    }
    return;
}

void lib_input(void){
    execute_libFuncenter();
    avm_memcellclear(&retval);

    unsigned n=avm_totalactuals();
    if(n!=0)
        avm_runtime_error("No arguments expected for funtion \"input()\" but given %d (line %d)\n", n, currLine);


    string s;
    getline(cin, s);
    if(is_int(s) || is_double(s)){
        retval.type=number_m;
        retval.data.numVal=strtod(s.c_str(), NULL);
    }else{
        if(s=="true"){
            retval.type=bool_m;
            retval.data.boolVal=true;
        }else if(s=="false"){
            retval.type=bool_m;
            retval.data.boolVal=false;
        }else if(s=="nil"){
            retval.type=nil_m;
        }else{
            retval.type=string_m;
            retval.data.strVal=s;
        }
    }
    return;
}


void lib_objectmemberkeys(void){
    execute_libFuncenter();
    avm_memcellclear(&retval);

    unsigned n=avm_totalactuals();
    if(n!=1)
        avm_runtime_error("One argument expected for funtion \"objectmemberkeys(x)\" but given %d (line %d)\n", n, currLine);
    if(avm_getactual(0)->type!=table_m){
        avm_runtime_error("Argument of \"objectmemberkeys(x)\" must be of type table (line %d)\n", currLine);
    }
    avm_table* table=avm_getactual(0)->data.tableVal;

    map<string,avm_memcell*>::iterator str;
    map<double,avm_memcell*>::iterator num;
    map<unsigned,avm_memcell*>::iterator func;
    map<string,avm_memcell*>::iterator l_func;
    map<avm_table*,avm_memcell*>::iterator tab;
    map<bool,avm_memcell*>::iterator b_ool;
    
    retval.type=table_m;
    retval.data.tableVal=new avm_table;
    avm_memcell* tmp;

    int i=0;
    for (str = table->strIndexed.begin(); str != table->strIndexed.end(); ++str) {
        tmp=new avm_memcell;
        tmp->type=string_m;
        tmp->data.strVal=str->first;
        retval.data.tableVal->numIndexed.insert({i, tmp});
        i++;
    }

    for (num = table->numIndexed.begin(); num != table->numIndexed.end(); ++num) {
        tmp=new avm_memcell;
        tmp->type=number_m;
        tmp->data.numVal=num->first;
        retval.data.tableVal->numIndexed.insert({i, tmp});

        i++;
    }
    for (func = table->userfuncIndexed.begin(); func != table->userfuncIndexed.end(); ++func) {
        tmp=new avm_memcell;
        tmp->type=userfunc_m;
        tmp->data.funcVal=func->first;
        retval.data.tableVal->numIndexed.insert({i, tmp});
        i++;
    }
    for (l_func = table->libFuncIndexed.begin(); l_func != table->libFuncIndexed.end(); ++l_func) {
        tmp=new avm_memcell;
        tmp->type=libfunc_m;
        tmp->data.libfuncVal=l_func->first;
        retval.data.tableVal->numIndexed.insert({i, tmp});
        i++;
    }
    for (tab = table->tableIndexed.begin(); tab != table->tableIndexed.end(); ++tab) {
        tmp=new avm_memcell;
        tmp->type=table_m;
        tmp->data.tableVal=tab->first;
        retval.data.tableVal->numIndexed.insert({i, tmp});
        i++;
    }
    for (b_ool = table->boolIndexed.begin(); b_ool != table->boolIndexed.end(); ++b_ool) {
        tmp=new avm_memcell;
        tmp->type=bool_m;
        tmp->data.boolVal=b_ool->first;
        retval.data.tableVal->numIndexed.insert({i, tmp});
        i++;
    }
    return;
}

void lib_objecttotalmembers(void){
    execute_libFuncenter();
    avm_memcellclear(&retval);

    unsigned n=avm_totalactuals();
    if(n!=1)
        avm_runtime_error("One argument expected for funtion \"objecttotalmembers(x)\" but given %d (line %d)\n", n, currLine);
    if(avm_getactual(0)->type!=table_m)
        avm_runtime_error("Argument of \"objecttotalmembers(x)\" must be of type table (line %d)\n", currLine);
    avm_table* table=avm_getactual(0)->data.tableVal;
    retval.type=number_m;
    retval.data.numVal=table->strIndexed.size() + table->numIndexed.size()+ table->userfuncIndexed.size() 
    + table->libFuncIndexed.size() + table->tableIndexed.size() + table->boolIndexed.size();
    return;
}

void lib_objectcopy(void){
    execute_libFuncenter();
    avm_memcellclear(&retval);

    unsigned n=avm_totalactuals();
    if(n!=1)
        avm_runtime_error("One argument expected for funtion \"objectcopy(x)\" but given %d (line %d)\n", n, currLine);
    
    if(avm_getactual(0)->type!=table_m)
        avm_runtime_error("Argument of \"objectcopy(x)\" must be of type table (line %d)\n", currLine);

    avm_memcell* arg2=avm_getactual(0);
    retval.type=table_m;
    retval.data.tableVal=new avm_table;
    
    retval.data.tableVal->strIndexed.insert(arg2->data.tableVal->strIndexed.begin(), arg2->data.tableVal->strIndexed.end());
    retval.data.tableVal->numIndexed.insert(arg2->data.tableVal->numIndexed.begin(), arg2->data.tableVal->numIndexed.end());
    retval.data.tableVal->userfuncIndexed.insert(arg2->data.tableVal->userfuncIndexed.begin(), arg2->data.tableVal->userfuncIndexed.end());
    retval.data.tableVal->libFuncIndexed.insert(arg2->data.tableVal->libFuncIndexed.begin(), arg2->data.tableVal->libFuncIndexed.end());
    retval.data.tableVal->boolIndexed.insert(arg2->data.tableVal->boolIndexed.begin(), arg2->data.tableVal->boolIndexed.end());
    retval.data.tableVal->tableIndexed.insert(arg2->data.tableVal->tableIndexed.begin(), arg2->data.tableVal->tableIndexed.end());
    return;
}


void lib_totalarguments(void){
    execute_libFuncenter();
    avm_memcellclear(&retval);

    unsigned prev_topsp=avm_get_envvalue(topsp+1);       /* GET THE PREV TOPSP */
    retval.type=number_m;
    if(prev_topsp==AVM_STACKSIZE-1)                      /* THE FUNCTION IS CALLED IN GLOBAL SCOPE */
        retval.type=nil_m;
    else
        retval.data.numVal=avm_get_envvalue(prev_topsp+4);
    return;
}



void lib_argument(void){
    execute_libFuncenter();
    avm_memcellclear(&retval);

    unsigned n=avm_totalactuals();
    unsigned index;
    unsigned prev_topsp=avm_get_envvalue(topsp+1);       /* GET THE PREV TOPSP */
    if(n!=1)
        avm_runtime_error("One arguments expected for funtion \"argument(x)\" but given %d (line %d)\n", n, currLine);
    avm_memcell *temp=avm_getactual(0);                  /* TAKE THE ARGUMENT */
    if(temp->type!=number_m)
        avm_runtime_error("Give argument is of type \"%s\" but \"argument(x)\" requires \
        argument of type \"number\" (line %d)\n", avm_typetostring(temp).c_str(), currLine);

    if(temp->data.numVal!=(unsigned)temp->data.numVal)
        avm_runtime_error("Invalid number \"%lf\" given as argument in function argument(x)\
         (line %d)\n", temp->data.numVal, currLine);

    index=temp->data.numVal;

    if(prev_topsp==AVM_STACKSIZE-1)                      /* THE FUNCTION IS CALLED IN GLOBAL SCOPE */
        retval.type=nil_m;
    else{
        temp=&avm_stack[prev_topsp + AVM_STACKENV_SIZE + 1 + index];
    }

    avm_assign(&retval, temp);
    return;
}

void lib_typeof(void){
    execute_libFuncenter();
    avm_memcellclear(&retval);

    unsigned n=avm_totalactuals();
    if(n!=1)
        avm_runtime_error("One arguments expected for funtion \"typeof(x)\" but given %d (line %d)\n", n, currLine);
    retval.type=string_m;
    retval.data.strVal=avm_typetostring(avm_getactual(0));
    return;
}

void lib_strtonum(void){
    execute_libFuncenter();
    avm_memcellclear(&retval);

    unsigned n=avm_totalactuals();
    if(n!=1)
        avm_runtime_error("One arguments expected for funtion \"strtonum(x)\" but given %d (line %d)\n", n, currLine);
    avm_memcell* cell=avm_getactual(0);
    if(cell->type!=string_m)
        avm_runtime_error("Give argument is of type \"%s\" but \"strnum(x)\" requires \
        argument of type \"string\" (line %d)\n", avm_typetostring(cell).c_str(), currLine);

    if(is_int(cell->data.strVal) || is_double(cell->data.strVal)){
        retval.type=number_m;
        retval.data.numVal=strtod(cell->data.strVal.c_str(), NULL);
    }
    return;
}

void lib_sqrt(void){
    execute_libFuncenter();
    avm_memcellclear(&retval);

    unsigned n=avm_totalactuals();
    if(n!=1)
        avm_runtime_error("One arguments expected for funtion \"sqrt(x)\" but given %d (line %d)\n", n, currLine);
    retval.type=number_m;
    avm_memcell* cell=avm_getactual(0);
    if(cell->type!=number_m){
        retval.data.numVal=0;
    }else{
        if(cell->data.numVal<0)
            retval.type=nil_m;
        else
            retval.data.numVal=sqrt(cell->data.numVal);
    }
    return;
}

void lib_cos(void){
    execute_libFuncenter();
    avm_memcellclear(&retval);

    unsigned n=avm_totalactuals();
    if(n!=1)
        avm_runtime_error("One arguments expected for funtion \"cos(x)\" but given %d (line %d)\n", n, currLine);
    retval.type=number_m;
    avm_memcell* cell=avm_getactual(0);
    if(cell->type!=number_m)
        avm_runtime_error("Give argument is of type \"%s\" but \"cos(x)\" requires \
        argument of type \"number\" (line %d)\n", avm_typetostring(cell).c_str(), currLine);
    retval.data.numVal=cos((cell->data.numVal)*(M_PI/180));
    return;
}

void lib_sin(void){
    execute_libFuncenter();
    avm_memcellclear(&retval);

    unsigned n=avm_totalactuals();
    if(n!=1)
        avm_runtime_error("One arguments expected for funtion \"sin(x)\" but given %d (line %d)\n", n, currLine);
    retval.type=number_m;
    avm_memcell* cell=avm_getactual(0);
    if(cell->type!=number_m)
        avm_runtime_error("Give argument is of type \"%s\" but \"sin(x)\" requires \
        argument of type \"number\" (line %d)\n", avm_typetostring(cell).c_str(), currLine);
    retval.data.numVal=sin((cell->data.numVal)*(M_PI/180));
    return;
}



library_func_t avm_getlibraryfunc(string id){
    auto it=libfunc_map.find(id);
    if(it==libfunc_map.end()){
        executionFinished=true;
        avm_runtime_error("unsupported lib func \"%s\" called in line %d\n", id.c_str(), currLine);
    }
    return it->second;
}

void avm_calllibfunc(string id){
    library_func_t f=avm_getlibraryfunc(id);
    topsp=top;
    totalActuals=0;
    (*f)();
    if(!executionFinished)
        execute_funcexit(NULL);
    return;
}

void register_libfunc(string id, library_func_t funcptr){
    libfunc_map.insert({id, funcptr});
    return;
}

void execute_libFuncenter(void){
    totalActuals=0;
    topsp=top;
    /* TOP REMAINS TOP */
    return;
}


bool is_int(string str){
    int i;
    for(i=0; str[i]!=0; i++)
        if(str[i]<'0' || str[i]>'9')
            return false;
    return true;
}

bool is_double(string str){
    int i;
    bool foundDot=false;
    if(str[0]=='.' || str[str.length()-1]=='.')
        return false;
    for(i=0; str[i]!=0; i++){
        if(str[i]=='.'){
            if(foundDot==true)
                return false;
            foundDot=true;
        }else if(str[i]<'0' || str[i]>'9')
            return false;
    }
    return true;
}