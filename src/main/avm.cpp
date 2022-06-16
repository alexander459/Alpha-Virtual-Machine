#include "avm.hpp"
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <vector>
#include "../exec/executer.hpp"
#include "../libfunctions/libfunctions.hpp"


vector<string> stringArray;
vector<string> libfuncArray;
vector<double> numberArray;
vector<program_func> progfuncArray;
vector<instruction> instructions;

bool executionFinished;
unsigned codeSize;
unsigned pc;
int currLine;
unsigned totalActuals;

avm_memcell avm_stack[AVM_STACKSIZE];  //THE STACK
avm_memcell ax, bx, cx, retval;        //REGISTERS

unsigned top;
unsigned topsp;
int globmem;




execute_func_t executeFuncs[]={
    execute_assign,
    execute_arithmeticOp,
    execute_arithmeticOp,
    execute_arithmeticOp,
    execute_arithmeticOp,
    execute_arithmeticOp,
    NULL,           /* THERE IS NO MORE UMINUS */
    execute_jump,
    execute_jeq,
    execute_jne,
    execute_jle,
    execute_jge,
    execute_jlt,
    execute_jgt,
    execute_call,
    execute_pusharg,
    execute_funcenter,
    execute_funcexit,
    execute_newtable,
    execute_tablegetelem,
    execute_tablesetelem,
    execute_nop
};

memclear_func_t memclearFunctions[]={
    NULL,
    NULL,
    NULL,
    memclear_table,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

avm_table *avm_tablenew(void){
    avm_table* table = new avm_table;
    table->total= 0;
    return table;
}

void avm_memcellclear(avm_memcell* m){
    if(m->type!=undef_m){
        memclear_func_t f=memclearFunctions[m->type];
        if(f!=NULL)
            (*f)(m);
        m->type=undef_m;
    }
    return;
}

void memclear_table(avm_memcell* m){
    assert(m->data.tableVal);
    //avm_tabledecrefcounter(m->data.tableVal);
    return;
}

void execute_cycle(void){
    if(executionFinished)
        return;
    else if(pc==codeSize){
        executionFinished=true;
        return;
    }else{
        assert(pc<codeSize);
        instruction* currInstr=&instructions[pc];
        assert(currInstr->opcode>=0 && currInstr->opcode<=AVM_MAX_INSTRUCTIONS);
        if(currInstr->srcLine!=0)
            currLine=currInstr->srcLine;
        unsigned oldPC=pc;
        //cout << "pc: " << pc << ", instruction: " << getOpcode(currInstr->opcode) << endl;
        (*executeFuncs[currInstr->opcode])(currInstr);
        //print_stack();
        if(pc==oldPC)
            pc++;
    }
    return;
}

void avm_dec_top(void){
    if (!top){
        avm_runtime_error("Stack Overflow (line %d)\n", currLine);
        executionFinished = 1;
    }
    else{
        --top;
    }
    return;
}

void avm_push_envvalue(unsigned val){
    /* TODO REMOVE IT IS FOR DEBUGGING */
    //avm_stack[top].type = number_m;
    avm_stack[top].data.numVal = val;
    avm_dec_top();
}

void avm_callsaveenvironment(void){
    avm_stack[top].type = num_of_actuals_m;
    avm_push_envvalue(totalActuals);        /* FIRST PUSH THE NUMBER OF LOCALS */
    avm_stack[top].type = next_addr_m;
    avm_push_envvalue(pc+1);                /* SECOND PUSH THE NEXT INSTRUCTION ADDRESS TO CONTINUE THE EXEC */
    avm_stack[top].type = old_top_m;
    avm_push_envvalue(top+totalActuals+2);  /* THIRD STORE THE OLD TOP */
    avm_stack[top].type = old_topsp;
    avm_push_envvalue(topsp);               /* LAST SAVE THE TOPSP */
    return;
}


unsigned avm_get_envvalue(unsigned i){
   // cout << "$$$$$$$$$$$$$$typeis " + avm_typetostring(&avm_stack[i]) << endl << endl;
    assert(avm_stack[i].type==number_m || avm_stack[i].type==old_top_m || avm_stack[i].type==old_topsp || avm_stack[i].type==num_of_actuals_m || avm_stack[i].type==next_addr_m);
    unsigned val=(unsigned)avm_stack[i].data.numVal;
    assert(avm_stack[i].data.numVal==((double)val));
    return val;
}

unsigned avm_totalactuals(void){
    return avm_get_envvalue(topsp+4);   /* NUMBER OF ACTUALS HAS OFFSET +4 FROM TOPSP */
}

avm_memcell *avm_getactual(unsigned i){
    assert(i<avm_totalactuals());
    return &avm_stack[topsp + AVM_STACKENV_SIZE + 1 + i];
}

double const_getnumber(unsigned index){
    assert(index<numberArray.size());
    return numberArray[index];
}
string const_getstring(unsigned index){
    assert(index<stringArray.size());
    return stringArray[index];
}
unsigned getfuncaddress(unsigned index){
    assert(index<progfuncArray.size());
    return progfuncArray[index].address;
}
string getfuncid(unsigned index){
    assert(index<progfuncArray.size());
    return progfuncArray[index].id;
}
program_func avm_getfuncinfo(unsigned index){
    assert(index<progfuncArray.size());
    return progfuncArray[index];
}
string libfuncs_getused(unsigned index){
    assert(index<libfuncArray.size());
    return libfuncArray[index];
}

void avm_runtime_error(string msg, ...){
  va_list args;
  string warn="[AVM] Runtime Error: ";
  warn+=msg;
  va_start(args, warn);
  vprintf(RED, args);
  vprintf(warn.c_str(), args);
  vprintf(WHITE, args);
  va_end(args);
  executionFinished=true;
  exit(0);
}

void avm_runtime_warning(string msg, ...){
  va_list args;
  string warn="[AVM] Warning: ";
  warn+=msg;
  va_start(args, warn);
  vprintf(YELLOW, args);
  vprintf(warn.c_str(), args);
  vprintf(WHITE, args);
  va_end(args);
}


unsigned read_magic_number(FILE* file){
    unsigned num;
    fread(&num, sizeof(num), 1, file);
    return num;
}

void set_globmem(int n){
    globmem=AVM_STACKSIZE-1-n;
    top=globmem;
    topsp=AVM_STACKSIZE-1;
    return;
}

bool avm_stack_full(void){
    return top==AVM_STACKSIZE;
}

/* CHECK IF EMPTY BEFORE CALL! */
avm_memcell avm_stack_top(void){
    return avm_stack[top];
}

void avm_stack_init(void){
    for(unsigned i=0; i<AVM_STACKSIZE; i++){
        AVM_WIPEOUT(avm_stack[i]);
        avm_stack[i].type=undef_m;
    }
    return;
}

/* INITIALIZES THE VM (STACK, PC ETC...) */
void init_avm(void){
    avm_stack_init();
    executionFinished=false;
    pc=0;
    //top=AVM_STACKSIZE-1;
    //topsp=top;
    totalActuals=0;
    codeSize=instructions.size();
    register_libfunc("print", lib_print);
    register_libfunc("input", lib_input);
    register_libfunc("objectmemberkeys", lib_objectmemberkeys);
    register_libfunc("objecttotalmembers", lib_objecttotalmembers);
    register_libfunc("objectcopy", lib_objectcopy);
    register_libfunc("totalarguments", lib_totalarguments);
    register_libfunc("argument", lib_argument);
    register_libfunc("typeof", lib_typeof);
    register_libfunc("strtonum", lib_strtonum);
    register_libfunc("sqrt", lib_sqrt);
    register_libfunc("cos", lib_cos);
    register_libfunc("sin", lib_sin);
    return;
}




void reset_instruction(instruction* in){
    in->arg1.type=reset;
    in->arg1.val=0;
    in->arg2.type=reset;
    in->arg2.val=0;
    in->result.type=reset;
    in->result.val=0;
    return;
}

int read_globmem(FILE* file){
    int num;
    fread(&num, sizeof(num), 1, file);
    return num;
}

/*reads the strings array from bin file*/
vector<string> read_strings(FILE* file){
    vector<string> array;
    unsigned array_size;
    unsigned i;
    int str_len;
    char* buff;
    fread(&array_size, sizeof(array_size), 1, file);
    for(i=0; i<array_size; i++){
        fread(&str_len, sizeof(str_len), 1, file);
        buff=(char *)malloc(str_len+1);

        fread(buff, str_len, 1, file);
        buff[str_len] = '\0';
        string tmp_s(buff);
        array.push_back(tmp_s);
        free(buff);
    }
    return array;
}

/*reads the functions array from bin file*/
vector<program_func> read_functions(FILE* file){
    vector<program_func> array;
    unsigned array_size;
    unsigned i;
    int str_len;
    char* buff;
    fread(&array_size, sizeof(array_size), 1, file);
    for(i=0; i<array_size; i++){
        program_func func;
        fread(&str_len, sizeof(str_len), 1, file);
        buff=(char *)malloc(str_len+1);
        fread(buff, str_len, 1, file);
        buff[str_len] = '\0';
        string tmp_s(buff);
        func.id=tmp_s;

        fread(&func.address, sizeof(func.address), 1, file);
        fread(&func.totalLocals, sizeof(func.totalLocals), 1, file);


        array.push_back(func);
        free(buff);
    }
    return array;
}

/*reads the number array from bin file*/
vector<double> read_nums(FILE* file){
    vector<double> array;
    unsigned array_size;
    unsigned i;
    double temp_number;
    fread(&array_size, sizeof(array_size), 1, file);
    for(i=0; i<array_size; i++){
        fread(&temp_number, sizeof(temp_number), 1, file);
        array.push_back(temp_number);
    }
    return array;
}

/*reads the instructions array from bin file*/
vector<instruction> read_instructions(FILE* file){
    vector<instruction> array;
    unsigned array_size;
    unsigned i;
    fread(&array_size, sizeof(array_size), 1, file);
    for(i=0; i<array_size; i++){
        instruction in;
        reset_instruction(&in);
        fread(&in.opcode, sizeof(in.opcode), 1, file);

        fread(&in.result.type, sizeof(in.result.type), 1, file);
        fread(&in.result.val, sizeof(in.result.val), 1, file);

        fread(&in.arg1.type, sizeof(in.arg1.type), 1, file);
        fread(&in.arg1.val, sizeof(in.arg1.val), 1, file);

        fread(&in.arg2.type, sizeof(in.arg2.type), 1, file);
        fread(&in.arg2.val, sizeof(in.arg2.val), 1, file);

        fread(&in.srcLine, sizeof(in.srcLine), 1, file);

        array.push_back(in);
    }
    return array;
}

int main(int argc, char* argv[]){

    FILE* file;
    file=fopen(argv[1], "rb");
    unsigned magic_number;
    assert(file!=NULL);

    magic_number=read_magic_number(file);
    if(magic_number!=420420420){
        string s(argv[1]);
        avm_runtime_error("File \"%s\" is not an Alpha file!\n", s);
    }
   // cout << "Num is " << magic_number << endl;

    set_globmem(read_globmem(file));
    //cout << "glob " << globmem << endl;
    stringArray=read_strings(file);
    
    /* READ THE FUNCS */
    progfuncArray=read_functions(file);
    
    /* READ THE LIB FUNCS */
    libfuncArray=read_strings(file);

    /* READ THE NUMBERS */
    numberArray=read_nums(file);
    
    /* READ INSTRUCTIONS */
    instructions=read_instructions(file);
    
    init_avm();

    fclose(file);
    //print_all();

    int i=0;
    while(executionFinished==false){
       // cout << i << ": pc is " << pc << " ::: ";
        execute_cycle();
        i++;
    }
    cout << endl;
    return 0;
}



/*############################################################################################
                                PRINTS FOR DEBUG
#############################################################################################*/

/*prints vm arguments*/
string print_vmargs(vmarg arg, bool bin){
    string s;
    if(bin)
        s="-";
    else
        s=" --> ";

    switch(arg.type){
        case reset:
            return "RESET";
        case label_a:
            return "label_a" + s + to_string(arg.val);
        case global_a:
            return "global_a" + s + to_string(arg.val);
        case formal_a:
            return "formal_a" + s + to_string(arg.val);
        case local_a:
            return "local_a" + s + to_string(arg.val);
        case number_a:
            return "number_a" + s + to_string(arg.val);
        case string_a:
            return "string_a" + s + to_string(arg.val);
        case bool_a:
            return "bool_a" + s + to_string(arg.val);
        case nil_a:
            return "nil_a";
        case userfunc_a:
            return "userfunc_a" + s + to_string(arg.val);
        case libfunc_a:
            return "libfunc_a" + s + to_string(arg.val);
        case retval_a:
            return "returnval_a" + s + to_string(arg.val);
        default:
            cout << "\nType is : " << arg.type << endl;
            assert(0);
    }
}

/*prints the opcode*/
string getOpcode(vmopcode op){
    switch(op){
        case assign_v:
            return "assign_v";
        case add_v:
            return "add_v";
        case sub_v:
            return "sub_v";
        case mul_v:
            return "mul_v";
        case div_v:
            return "div_v";
        case mod_v:
            return "mod_v";
        case uminus_v:
            return "uminus_v";
        case jeq_v:
            return "jeq_v";
        case jne_v:
            return "jne_v";
        case jle_v:
            return "jle_v";
        case jge_v:
            return "jge_v";
        case jlt_v:
            return "jlt_v";
        case jgt_v:
            return "jgt_v";
        case call_v:
            return "call_v";
        case pusharg_v:
            return "pusharg_v";
        case funcenter_v:
            return "funcenter_v";
        case funcexit_v:
            return "funcexit_v";
        case newtable_v:
            return "newtable_v";
        case tablegetelem_v:
            return "tablegetelem_v";
        case tablesetelem_v:
            return "tablesetelem_v";
        case jump_v:
            return "jump_v";
        case nop_v:
            return "nop_v";
            break;
        default:
            assert(0);
    }
    assert(0);
}



/*prints all arrays (for debugging)*/
void print_all(void){
    unsigned i;
    cout << "globs\n";
    cout << globmem << endl;

    cout << "Strings\n";
    for(i=0; i<stringArray.size(); i++)
        cout << stringArray[i] << endl;

    cout << "funcs\n";
    for(i=0; i<progfuncArray.size(); i++)
        cout << "ID:" << progfuncArray[i].id << ", ADDR:" << progfuncArray[i].address
        <<  ", LOCALS:" << progfuncArray[i].totalLocals << endl;

    cout << "libf\n";
    for(i=0; i<libfuncArray.size(); i++)
        cout << libfuncArray[i] << endl;

    cout << "nums\n";
    for(i=0; i<numberArray.size(); i++)
        cout << numberArray[i] << endl;

    cout << "instr\n";
    for(i=0; i<instructions.size(); i++){
        cout << "OpCode: " << getOpcode(instructions[i].opcode);
        if(instructions[i].result.type!=reset){
            cout << ", RESULT: " + print_vmargs(instructions[i].result, false);
        }
        if(instructions[i].arg1.type!=reset){
            cout << ", ARG1: " + print_vmargs(instructions[i].arg1, false);
        }
        if(instructions[i].arg2.type!=reset){
            cout << ", ARG2: " + print_vmargs(instructions[i].arg2, false);
        }
        if(instructions[i].result.type!=retval_a)
           cout << " (line:" << instructions[i].srcLine << ")\n";
        else
            cout << endl;
    }
    return;
}




void print_stack(void){
    int i;
    cout << "-------------------------------------\n";
    for(i=top+1; i<=AVM_STACKSIZE-1; i++){
        cout << "[MEM : " << i << "] ";
        cout << avm_typetostring(&avm_stack[i]) << " : ";
        cout << avm_tostring(&avm_stack[i]) << endl;
    }
    cout << "-------------------------------------\n";
    return;
}

string print_table(avm_table* table){
    map<string,avm_memcell*>::iterator str;
    map<double,avm_memcell*>::iterator num;
    map<unsigned,avm_memcell*>::iterator func;
    map<string,avm_memcell*>::iterator l_func;
    map<avm_table*,avm_memcell*>::iterator tab;
    map<bool,avm_memcell*>::iterator b_ool;

    for (str = table->strIndexed.begin(); str != table->strIndexed.end(); ++str) {
        cout << "{KEY:" << str->first << ", VALUE:" << avm_tostring(str->second)<<"}"
             << " ";
    }
    for (num = table->numIndexed.begin(); num != table->numIndexed.end(); ++num) {
        cout << "{KEY:" << num->first << ", VALUE:" << avm_tostring(num->second)<<"}"
             << " ";
    }
    for (func = table->userfuncIndexed.begin(); func != table->userfuncIndexed.end(); ++func) {
        cout << "{KEY:" << func->first << ", VALUE:" << avm_tostring(func->second)<<"}"
             << " ";
    }
    for (l_func = table->libFuncIndexed.begin(); l_func != table->libFuncIndexed.end(); ++l_func) {
        cout << "{KEY:" << l_func->first << ", VALUE:" << avm_tostring(l_func->second)<<"}"
             << " ";
    }
    for (tab = table->tableIndexed.begin(); tab != table->tableIndexed.end(); ++tab) {
        if(tab->first==table)

        cout << "{KEY:" << print_table(tab->first) << ", VALUE:" << avm_tostring(tab->second)<<"}"
             << " ";
    }
    for (b_ool = table->boolIndexed.begin(); b_ool != table->boolIndexed.end(); ++b_ool) {
         cout << "{KEY:" << b_ool->first << ", VALUE:" << avm_tostring(b_ool->second)<<"}"
             <<" ";
    }
    return " ";

}