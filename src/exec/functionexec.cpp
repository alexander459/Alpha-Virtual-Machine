#include "executer.hpp"
#include "../libfunctions/libfunctions.hpp"

void execute_call(instruction *in){
    //cout << "call!\n";
    avm_memcell *func = avm_translate_operand(&in->arg1, &ax);
    assert(func);
	avm_callsaveenvironment();

    switch (func->type){
    	case userfunc_m:
        	pc=getfuncaddress(func->data.funcVal);
        	assert(pc < codeSize);
        	assert(instructions[pc].opcode == funcenter_v);
        	break;
        case string_m:
        	avm_calllibfunc(func->data.strVal);
        	break;
        case libfunc_m:
        	avm_calllibfunc((func->data).libfuncVal);
        	break;
        case table_m:
        	//avm_functorCall(func);
        	break;
    	default:
         	string s = avm_tostring(func);
        	avm_runtime_error("Cannot use \"%s\" as a function (line %d)\n", s.c_str(), currLine);
        	executionFinished=1;
   }
}

void execute_pusharg(instruction* in){
  //  cout << "pusharg!\n";
    avm_memcell *arg = avm_translate_operand(&in->arg1, &ax);
    assert(arg);

    avm_assign(&avm_stack[top], arg);       /* PLACE THE ARGUMENT INT THE STACK */
    totalActuals++;
    avm_dec_top();              /* AN ARGUMENT PUSHED, GIVE MORE SPACE TO THE STACK */
    return;
}

void execute_funcenter(instruction* in){
   // cout << "func enter!\n";
    avm_memcell *func = avm_translate_operand(&in->result, &ax);
    assert(func);
    assert(pc == getfuncaddress(func->data.funcVal));

    totalActuals = 0;
    program_func funcInfo = avm_getfuncinfo(in->result.val);

    topsp=top;
    top=top-funcInfo.totalLocals;
    return;
}

void execute_funcexit(instruction*){
   // cout << "func exit!\n";
    unsigned oldTop = top;
    top = avm_get_envvalue(topsp+2);            /* OLD TOP IS TWO CELLS BELOW THE CURRENT TOPSP */
    pc = avm_get_envvalue(topsp+3);             /* OLD TOPSP IS THREE CELLS BELOW THE CURRENT TOPSP */
    topsp=avm_get_envvalue(topsp+1);            /* OLD TOPSP IS ONE CELL BELOW THE CURRENT TOPSP */

    oldTop++;
    while(oldTop <= top){
        avm_memcellclear(&avm_stack[oldTop]);
        oldTop++;
    }
}