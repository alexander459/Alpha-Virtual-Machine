#include "executer.hpp"

call_op_funcs_t callOpFunctions[]={
    NULL,               /* SKIP ASSIGN */
    execute_add,
    execute_sub,
    execute_mul,
    execute_div,
    execute_mod
};

void execute_add(avm_memcell* arg1, avm_memcell* arg2, avm_memcell* result){
   // cout << "add!\n";
    assert(arg1 && arg2 && result);
    result->type=number_m;
    result->data.numVal=arg1->data.numVal+arg2->data.numVal;
    return;
}

void execute_sub(avm_memcell* arg1, avm_memcell* arg2, avm_memcell* result){
 //   cout << "sub!\n";
    assert(arg1 && arg2 && result);
    result->type=number_m;
    result->data.numVal=arg1->data.numVal-arg2->data.numVal;
    return;
}

void execute_mul(avm_memcell* arg1, avm_memcell* arg2, avm_memcell* result){
  //  cout << "mul!\n";
    assert(arg1 && arg2 && result);
    result->type=number_m;
    result->data.numVal=arg1->data.numVal*arg2->data.numVal;
    return;
}

void execute_div(avm_memcell* arg1, avm_memcell* arg2, avm_memcell* result){
    //cout << "div!\n";
    assert(arg1 && arg2 && result);
    result->type=number_m;
    result->data.numVal=arg1->data.numVal/arg2->data.numVal;
    return;
}

void execute_mod(avm_memcell* arg1, avm_memcell* arg2, avm_memcell* result){
   // cout << "mod!\n";
    assert(arg1 && arg2 && result);
    result->type=number_m;
    result->data.numVal=(unsigned)(arg1->data.numVal) % (unsigned)(arg2->data.numVal);
    return;
}

void execute_arithmeticOp(instruction* in){
    assert(in!=NULL);
    assert(in->opcode==add_v || in->opcode==sub_v || in->opcode==mul_v || in->opcode==div_v || in->opcode==mod_v);
    avm_memcell *result=avm_translate_operand(&in->result, NULL);
    avm_memcell *arg1=avm_translate_operand(&in->arg1, &ax);
    avm_memcell *arg2=avm_translate_operand(&in->arg2, &bx);
    assert(result && arg1 && arg2);
    assert(in->arg1.type!=reset && in->arg2.type!=reset && in->result.type!=reset);
    if(in->arg1.type==string_a || in->arg2.type==string_a || in->arg1.type==bool_a || in->arg2.type==bool_a)
        avm_runtime_error("Can not make arithmetic operations between non number types (line %d)\n", currLine);
    callOpFunctions[in->opcode](arg1, arg2, result);
    return;
}