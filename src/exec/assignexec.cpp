#include "executer.hpp"
void execute_assign(instruction* in){
    //cout << "assign!\n";
    avm_memcell* lv=avm_translate_operand(&in->result, NULL);
    avm_memcell* rv=avm_translate_operand(&in->arg1, &ax);
    assert(lv && rv);
    assert(lv && (&avm_stack[AVM_STACKSIZE-1] >= lv && lv > &avm_stack[top] || lv==&retval));

    if(!((&avm_stack[AVM_STACKSIZE-1] >= lv && lv > &avm_stack[top] || lv==&retval))){
        avm_runtime_error("Invalid stack accessing (line %d)\n", currLine);
    }
    avm_assign(lv, rv);
    return;
}

/* assigns the value of the address of "rv" in the address "lv" */
void avm_assign(avm_memcell* lv, avm_memcell* rv){
    if(lv==rv)
        return;
    if(lv->type==table_m && rv->type==table_m && lv->data.tableVal==rv->data.tableVal)
        return;
    if(rv->type==undef_m){
        avm_runtime_warning("Assigning from undefined content (line %d)\n", currLine);
        return;
    }
    avm_memcellclear(lv);
    memcellcopy(lv, rv);
    if(lv->type==table_m){
        avm_tableincrefcounter(lv->data.tableVal);
    }
    return;
}


copy_funcs copymemcellFunctions[]={
    copyNum,
    copyStr,
    copyBool,
    copyTable,
    copyUserfunc,
    copyLibfunc,
    copyNil,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

void memcellcopy(avm_memcell* arg1, avm_memcell* arg2){
    assert(arg1 && arg2);
    assert(arg2->type!=undef_m);
    assert(arg2->type!=next_addr_m && arg2->type!=old_top_m && arg2->type!=old_topsp && arg2->type!=num_of_actuals_m);
    (*copymemcellFunctions[arg1->type=arg2->type])(arg1, arg2);
    return;
}

void copyNum(avm_memcell* arg1, avm_memcell* arg2){
    assert(arg1 && arg2);
    assert(arg2->type==number_m);
    arg1->type=number_m;
    arg1->data.numVal=arg2->data.numVal;
    return;
}

void copyStr(avm_memcell* arg1, avm_memcell* arg2){
    assert(arg1 && arg2);
    assert(arg2->type==string_m);
    arg1->type=string_m;
    arg1->data.strVal=arg2->data.strVal;
    return;
}

void copyBool(avm_memcell* arg1, avm_memcell* arg2){
    assert(arg1 && arg2);
    assert(arg2->type==bool_m);
    arg1->type=bool_m;
    arg1->data.boolVal=arg2->data.boolVal;
    return;
}

void copyTable(avm_memcell* arg1, avm_memcell* arg2){
    assert(arg1 && arg2);
    assert(arg2->type==table_m);
    arg1->type=table_m;
    arg1->data.tableVal = arg2->data.tableVal;
    //print_table(arg2->data.tableVal);
    
    arg1->data.tableVal->strIndexed.insert(arg2->data.tableVal->strIndexed.begin(), arg2->data.tableVal->strIndexed.end());
    arg1->data.tableVal->numIndexed.insert(arg2->data.tableVal->numIndexed.begin(), arg2->data.tableVal->numIndexed.end());
    arg1->data.tableVal->userfuncIndexed.insert(arg2->data.tableVal->userfuncIndexed.begin(), arg2->data.tableVal->userfuncIndexed.end());
    arg1->data.tableVal->libFuncIndexed.insert(arg2->data.tableVal->libFuncIndexed.begin(), arg2->data.tableVal->libFuncIndexed.end());
    arg1->data.tableVal->boolIndexed.insert(arg2->data.tableVal->boolIndexed.begin(), arg2->data.tableVal->boolIndexed.end());
    arg1->data.tableVal->tableIndexed.insert(arg2->data.tableVal->tableIndexed.begin(), arg2->data.tableVal->tableIndexed.end());
    
}

void copyUserfunc(avm_memcell* arg1, avm_memcell* arg2){
    assert(arg1 && arg2);
    assert(arg2->type==userfunc_m);
    arg1->type=userfunc_m;
    arg1->data.funcVal=arg2->data.funcVal;
    return;
}

void copyLibfunc(avm_memcell* arg1, avm_memcell* arg2){
    assert(arg1 && arg2);
    assert(arg2->type==libfunc_m);
    arg1->type=libfunc_m;
    arg1->data.libfuncVal=arg2->data.libfuncVal;
    return;
}

void copyNil(avm_memcell* arg1, avm_memcell* arg2){
    assert(arg1 && arg2);
    assert(arg2->type==nil_m);
    arg1->type=nil_m;
}