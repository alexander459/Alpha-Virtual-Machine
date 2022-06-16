#include "executer.hpp"
#include <string.h>

comp_func_t compareFunctions[]={
    compareNumbers,
    compareStrings,
    NULL,
    compareTables,
    compareUserFunc,
    compareLibFunc,
    NULL,
    NULL
};




void execute_jump(instruction* in){
   // cout << "jump!\n";
    assert(in->result.type==label_a);
    pc=in->result.val;
    return;
}

void execute_jeq(instruction* in){
   // cout << "jump equal!\n";
    assert(in);
    assert(in->result.type==label_a);
    assert(in->arg1.type!=reset);
    assert(in->arg2.type!=reset);
    assert(in->opcode==jeq_v);
    callEqualityOperation(in, jeq_v);
    return;
}

void execute_jne(instruction* in){
    //cout << "jump not equal!\n";
    assert(in);
    assert(in->result.type==label_a);
    assert(in->arg1.type!=reset);
    assert(in->arg2.type!=reset);
    assert(in->opcode==jne_v);
    callEqualityOperation(in, jne_v);
    return;
}

void execute_jle(instruction* in){
   // cout << "jump less equal!\n";
    assert(in);
    assert(in->result.type==label_a);
    assert(in->arg1.type!=reset);
    assert(in->arg2.type!=reset);
    assert(in->opcode==jle_v);
    callComparisonOperation(in, jle_v);
    return;
}

void execute_jge(instruction* in){
 //  cout << "jump greater equal!\n";
    assert(in);
    assert(in->result.type==label_a);
    assert(in->arg1.type!=reset);
    assert(in->arg2.type!=reset);
    assert(in->opcode==jge_v);
    callComparisonOperation(in, jge_v);
    return;
}

void execute_jlt(instruction* in){
   // cout << "jump less!\n";
    assert(in);
    assert(in->result.type==label_a);
    assert(in->arg1.type!=reset);
    assert(in->arg2.type!=reset);
    assert(in->opcode==jlt_v);
    callComparisonOperation(in, jlt_v);
    return;
}

void execute_jgt(instruction* in){
   // cout << "jump greaer!\n";
    assert(in);
    assert(in->result.type==label_a);
    assert(in->arg1.type!=reset);
    assert(in->arg2.type!=reset);
    assert(in->opcode==jgt_v);
    callComparisonOperation(in, jgt_v);
    return;
}


void callEqualityOperation(instruction* in, vmopcode op){
    assert(in);
    assert(in->opcode==jeq_v || in->opcode==jne_v);
    assert(in->result.type==label_a);
    assert(in->arg1.type!=reset);
    assert(in->arg2.type!=reset);
    bool result=false;

    avm_memcell *arg1=avm_translate_operand(&in->arg1, &ax);
    avm_memcell *arg2=avm_translate_operand(&in->arg2, &bx);

    if(arg1->type==undef_m || arg2->type==undef_m)                  /* ONE IS UNDEFINED SO ERROR */
        avm_runtime_error("Equality comparison with undefined (line %d)\n", currLine);
    else if (arg1->type == nil_m || arg2->type == nil_m)            /* ONE IS NIL SO CHECK IF BOTH NIL */
        result=(arg1->type == nil_m && arg2->type == nil_m);
    else if (arg1->type == bool_m || arg2->type == bool_m)          /* ONE IS BOOL SO CONVERT TO BOOL */
        result=(avm_tobool(arg1) == avm_tobool(arg2));
    else if (arg1->type != arg2->type)                              /* DIFFERENT TYPES SO ERROR */
        avm_runtime_error("Equality comparison between different types (line %d)\n", currLine);
    else{
        assert(arg1->type!=undef_m && arg1->type!=nil_m && arg1->type!=bool_m);
        result=compareFunctions[arg1->type](arg1, arg2, op);
    }

    if (!executionFinished && result==true)
        pc=in->result.val;                                          /* SET THE PC TO JUMP */
    return;
}

void callComparisonOperation(instruction* in, vmopcode op){
    assert(in);
    assert(in->opcode==jle_v || in->opcode==jge_v || in->opcode==jlt_v || in->opcode==jgt_v);
    assert(in->result.type==label_a);
    assert(in->arg1.type!=reset);
    assert(in->arg2.type!=reset);
    bool result=false;

    avm_memcell *arg1=avm_translate_operand(&in->arg1, &ax);
    avm_memcell *arg2=avm_translate_operand(&in->arg2, &bx);

    if(arg1->type==undef_m || arg2->type==undef_m)                  /* ONE IS UNDEFINED SO ERROR */
        avm_runtime_error("Comparison with undefined (line %d)\n", currLine);
    else if (arg1->type != number_m || arg2->type != number_m)      /* ONLY NUMBERS ALLOWED */
        avm_runtime_error(">, >=, <, <= operators allowed only with numbers (line %d)\n", currLine);
    else{
        assert(arg1->type==number_m && arg2->type==number_m);
        result=compareFunctions[arg1->type](arg1, arg2, op);
    }

    if (!executionFinished && result==true)
        pc=in->result.val;                                          /* SET THE PC TO JUMP */
    return;
}

bool compareNumbers(avm_memcell* arg1, avm_memcell* arg2, vmopcode op){
    switch(op){
        case jeq_v: return arg1->data.numVal == arg2->data.numVal;
        case jne_v: return arg1->data.numVal != arg2->data.numVal;
        case jge_v: return arg1->data.numVal >= arg2->data.numVal;
        case jgt_v: return arg1->data.numVal > arg2->data.numVal;
        case jle_v: return arg1->data.numVal <= arg2->data.numVal;
        case jlt_v: return arg1->data.numVal < arg2->data.numVal;
        default: assert(0);
    }
    assert(0);
}

bool compareStrings(avm_memcell* arg1, avm_memcell* arg2, vmopcode op){
    assert(op==jeq_v || op==jne_v);
    switch(op){
        case jeq_v: return arg1->data.strVal == arg2->data.strVal;
        case jne_v: return arg1->data.strVal != arg2->data.strVal;
        default: assert(0);
    }
    assert(0);
}

bool compareTables(avm_memcell* arg1, avm_memcell* arg2, vmopcode op){
    assert(arg1 && arg2);
    assert(arg1->type==table_m && arg2->type==table_m);
    if(op!=jeq_v && op!=jne_v)
        avm_runtime_error(">, >=, <, <= operators allowed only with numbers (line %d)\n", currLine);
    return arg1->data.tableVal==arg2->data.tableVal;
}

bool compareUserFunc(avm_memcell* arg1, avm_memcell* arg2, vmopcode op){
    assert(op==jeq_v || op==jne_v);

    program_func f1=avm_getfuncinfo(arg1->data.funcVal);
    program_func f2=avm_getfuncinfo(arg2->data.funcVal);
    switch(op){
        case jeq_v: return ((f1.address==f2.address) && (f1.id==f2.id) && (f1.totalLocals==f2.totalLocals));
        case jne_v: return ((f1.address!=f2.address) && (f1.id!=f2.id) && (f1.totalLocals!=f2.totalLocals));
        default: assert(0);
    }
    assert(0);
}

bool compareLibFunc(avm_memcell* arg1, avm_memcell* arg2, vmopcode op){
    assert(op==jeq_v || op==jne_v);
    switch(op){
        case jeq_v: return arg1->data.libfuncVal == arg2->data.libfuncVal;
        case jne_v: return arg1->data.libfuncVal != arg2->data.libfuncVal;
        default: assert(0);
    }
    assert(0);
}