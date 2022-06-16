#include "conversions.hpp"
#include "../main/avm.hpp"


tobool_func_t toboolFuncs[]={
    number_tobool,
    string_tobool,
    bool_tobool,
    table_tobool,
    userfunc_tobool,
    libfunc_tobool,
    nil_tobool,
    undef_tobool
};



string avm_tostring(avm_memcell* cell){
    assert(cell!=NULL);
    switch(cell->type){
        /* TODO REMOVE WHEN FINISH DEBUGGING */
        case old_top_m:
        case old_topsp:
        case num_of_actuals_m:
        case next_addr_m:
        case number_m: return to_string(cell->data.numVal);
        case string_m: return cell->data.strVal;
        case bool_m:
            if(cell->data.boolVal==1)
                return "true";
            else
                return "false";
        case userfunc_m: return "userfunc "+getfuncid(cell->data.funcVal);
        case libfunc_m: return "libfunc " + cell->data.libfuncVal;
        case table_m:   return  print_table(cell->data.tableVal);
        case nil_m: return "nil";
        case undef_m: return "undefined";
        
        default: assert(0);
    }
}

string avm_typetostring(avm_memcell* cell){
    assert(cell!=NULL);
    switch(cell->type){
        case number_m: return "number";
        case string_m: return "string";
        case bool_m: return "bool";
        case userfunc_m: return "user func";
        case libfunc_m: return "libfunc";
        case table_m: return "table";
        case nil_m: return "nil";
        case undef_m: return "undefined";
        case old_top_m: return "old top";
        case old_topsp: return "old topsp";
        case num_of_actuals_m: return "number of actuals";
        case next_addr_m: return "return address";
        default: assert(0);
    }
}

bool number_tobool(avm_memcell *m){return m->data.numVal != 0;}
bool string_tobool(avm_memcell *m){return m->data.strVal.c_str()[0] != 0;}
bool bool_tobool(avm_memcell *m){return m->data.boolVal;}
bool userfunc_tobool(avm_memcell *m){return 1;}
bool libfunc_tobool(avm_memcell *m){return 1;}
bool nil_tobool(avm_memcell *m){return false;}

bool table_tobool(avm_memcell *m){
    return 1;
}

bool undef_tobool(avm_memcell *m){
    assert(0);
    return 0;
}

bool avm_tobool(avm_memcell *m){
    assert(m->type >= 0 && m->type < undef_m);
    return (*toboolFuncs[m->type])(m);
}


/*returns the address of the stack (or register) to store the data
and stores the data in this address if necessary*/
avm_memcell* avm_translate_operand(vmarg* arg, avm_memcell* reg){
    switch(arg->type){
        case global_a: return &avm_stack[AVM_STACKSIZE-1-arg->val];
        case local_a: return &avm_stack[topsp-arg->val];
        case formal_a: return &avm_stack[topsp + AVM_STACKENV_SIZE + 1 + arg->val];
        case retval_a: return &retval;
        case number_a:
            reg->type=number_m;                                 /* STORE THE NUM IN THE MEMORY AND RETURN THE CELL */
            reg->data.numVal=const_getnumber(arg->val);
            return reg;
        case string_a:
            reg->type=string_m;                                 /* STORE THE STRING IN THE MEMORY AND RETURN THE CELL */
            reg->data.strVal=const_getstring(arg->val);
            return reg;
        case bool_a:
            reg->type=bool_m;                                   /* STORE THE BOOL IN THE MEMORY AND RETURN THE CELL */
            reg->data.boolVal=arg->val;
            return reg;
        case nil_a:
            reg->type=nil_m;                                    /* STORE THE NIL IN THE MEMORY AND RETURN THE CELL */
            return reg;
        case userfunc_a:
            reg->type=userfunc_m;                               /* STORE THE FUNC ADDR IN THE MEMORY AND RETURN THE CELL */
            reg->data.funcVal=arg->val;
            return reg;
        case libfunc_a:
            reg->type=libfunc_m;                                /* STORE THE FUNC NAME IN THE MEMORY AND RETURN THE CELL */
            reg->data.libfuncVal=libfuncs_getused(arg->val);
            return reg;
        default: assert(0);
    }
}