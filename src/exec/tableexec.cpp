#include "executer.hpp"


typedef avm_memcell* (*get_table_el_funcs)(avm_memcell* index,avm_table* table);
typedef avm_memcell* (*erase_table_el_funcs)(avm_memcell* index,avm_table* table);

erase_table_el_funcs erase_el_functable[]={
    erase_num_indexed,
    erase_str_indexed, 
    erase_bool_indexed,
    erase_table_indexed,
    erase_usrf_indexed,
    erase_libf_indexed
};
get_table_el_funcs get_el_functable[]={
    get_num_indexed,
    get_str_indexed, 
    get_bool_indexed,
    get_table_indexed,
    get_usrf_indexed,
    get_libf_indexed
};

void execute_newtable(instruction* in){
   // cout << "newtable!\n";
  // cout << "****"<<in->arg1.type;
   avm_memcell *lv = avm_translate_operand(&in->arg1, NULL);

    assert(lv && (&avm_stack[AVM_STACKSIZE-1] >= lv && lv > &avm_stack[top] || lv == &retval));
    
    avm_memcellclear(lv);
    lv->type = table_m;
    lv->data.tableVal = avm_tablenew();

    avm_tableincrefcounter(lv->data.tableVal);
}

void execute_tablegetelem(instruction* in){
    //cout << "newtable get elem!\n";
    avm_memcell *lv = avm_translate_operand(&in->result, NULL);
    avm_memcell *t = avm_translate_operand(&in->arg1, NULL);
    avm_memcell *i = avm_translate_operand(&in->arg2, &ax);

    assert(lv && &avm_stack[AVM_STACKSIZE-1] >= lv && lv > &avm_stack[top] || lv == &retval);
    assert(t && &avm_stack[AVM_STACKSIZE-1] >= t && t > &avm_stack[top]);
    assert(i);

    avm_memcellclear(lv);
    lv->type = nil_m;

    if (t->type != table_m)
        avm_runtime_error("illegal use of non table element as table (line %d)\n", currLine);
    else{
        avm_memcell *content = avm_tablegetelem(t->data.tableVal, i);
        if (content){
            avm_assign(lv, content);
            //print_table(t->data.tableVal);
        }else{
		    lv->type=nil_m;
        }
    }
    
    return;
}

void execute_tablesetelem(instruction* in){
 // cout << "newtable set elem!\n";
    avm_memcell *t = avm_translate_operand(&in->result, NULL);
    avm_memcell *i = avm_translate_operand(&in->arg1, &ax);
    avm_memcell *c = avm_translate_operand(&in->arg2, &bx);

    assert(t && &avm_stack[AVM_STACKSIZE-1] >= t && t > &avm_stack[top]);
    assert(i && c);

    if (t->type != table_m)
        avm_runtime_error("illegal use of non table element as table");
        
    if(c->type==nil_m){
        erase_el_functable[i->type](i, t->data.tableVal);
    }else
        avm_tablesetelem(t->data.tableVal, i, c);
      //  print_table(t->data.tableVal);
    return;
}

avm_memcell* avm_tablegetelem(avm_table* table, avm_memcell* index){
    assert(table && index);
    //cout<<index->data.strVal<<endl;
    return  get_el_functable[index->type](index,table);
    
}

avm_memcell* make_cell_copy( avm_memcell* to_copy){
    avm_memcell* tmp = new avm_memcell;
    assert(tmp);
    tmp->type = to_copy->type;
    tmp->data.numVal = to_copy->data.numVal;
    tmp->data.strVal= to_copy->data.strVal;
    tmp->data.funcVal = to_copy->data.funcVal;
    tmp->data.libfuncVal = to_copy->data.libfuncVal;
    tmp->data.boolVal = to_copy->data.boolVal;
    if(to_copy->type == table_m){
        copyTable(tmp,to_copy);
    }
    return tmp;

}

void avm_tablesetelem(avm_table* table, avm_memcell* index, avm_memcell* value){
    assert(table && index && value);
    avm_memcell* copy = make_cell_copy(value);
    if(get_el_functable[index->type](index,table)!=NULL){
        erase_el_functable[index->type](index,table);
    }
    
    switch (index->type)
    {
    case number_m:
    
        table->numIndexed.insert(pair<double,avm_memcell*>(index->data.numVal,copy));
         //cout<<"PRINTING"<< print_table(table);
        break;
    case string_m:
        
        table->strIndexed.insert(pair<string,avm_memcell*>(index->data.strVal,copy));
       // cout<<" TABLE AT PC :"<<pc<<endl;
      
        break;
    case userfunc_m:
        table->userfuncIndexed.insert(pair<unsigned,avm_memcell*>(index->data.numVal,copy));
        break;
    case libfunc_m:
        table->libFuncIndexed.insert(pair<string,avm_memcell*>(index->data.libfuncVal,copy));
        break;
    case table_m:
        table->tableIndexed.insert(pair<avm_table*,avm_memcell*>(index->data.tableVal,copy));
        break;
    case bool_m:
        table->boolIndexed.insert(pair<bool,avm_memcell*>(index->data.boolVal,copy)); 
        break;               
    default:
        assert(0);
        break;
    }
    table->total++;
  
    return;
}



void avm_tabledecrefcounter(avm_table* table){
    table->refCounter--;
    return;
}

void avm_tableincrefcounter(avm_table* table){
    table->refCounter++;
    return;
}

void execute_nop(instruction* in){
    //cout << "nop!\n";
}

avm_memcell* get_num_indexed(avm_memcell* index,avm_table* table){
    auto it = table->numIndexed.find(index->data.numVal);
    if(it==table->numIndexed.end()){
        return NULL;
    }
    return it->second;
}
avm_memcell* get_str_indexed(avm_memcell* index,avm_table* table){
    auto it = table->strIndexed.find(index->data.strVal);
     if(it==table->strIndexed.end()){
        return NULL;
    }
    return it->second;
}
avm_memcell* get_usrf_indexed(avm_memcell* index,avm_table* table){
    auto it = table->userfuncIndexed.find(index->data.funcVal);
     if(it==table->userfuncIndexed.end()){
        return NULL;
    }
    return it->second;
}
avm_memcell* get_libf_indexed(avm_memcell* index,avm_table* table){
    auto it = table->libFuncIndexed.find(index->data.libfuncVal);
     if(it==table->libFuncIndexed.end()){
        return NULL;
    }
    return it->second;
}

template <typename Map>
bool key_compare (Map const &lhs, Map const &rhs) {

    auto pred = [] (decltype(*lhs.begin()) a, decltype(a) b)
                   { return a.first == b.first; };

    return lhs.size() == rhs.size()
        && std::equal(lhs.begin(), lhs.end(), rhs.begin(), pred);
}



avm_memcell* get_table_indexed(avm_memcell* index,avm_table* table){
   
    for(auto map_it : table->tableIndexed){
        if(key_compare(index->data.tableVal->tableIndexed,map_it.first->tableIndexed)){
            return map_it.second;
        }
    }
     return NULL;
}
avm_memcell*  get_bool_indexed(avm_memcell* index,avm_table* table){
    auto it = table->boolIndexed.find(index->data.boolVal);
     if(it==table->boolIndexed.end()){
        return NULL;
    }
    return it->second; 
}


avm_memcell* erase_num_indexed(avm_memcell* index,avm_table* table){
     table->numIndexed.erase(index->data.numVal);
     return NULL;
}
avm_memcell* erase_str_indexed(avm_memcell* index,avm_table* table){
    table->strIndexed.erase(index->data.strVal);
    return NULL;
}
avm_memcell* erase_usrf_indexed(avm_memcell* index,avm_table* table){
    table->userfuncIndexed.erase(index->data.funcVal);
    return NULL;
}
avm_memcell* erase_libf_indexed(avm_memcell* index,avm_table* table){
   table->libFuncIndexed.erase(index->data.libfuncVal);
   return NULL;
}
avm_memcell* erase_table_indexed(avm_memcell* index,avm_table* table){
   table->tableIndexed.erase(index->data.tableVal);
   return NULL;
}
avm_memcell* erase_bool_indexed(avm_memcell* index,avm_table* table){
  table->boolIndexed.erase(index->data.boolVal);
  return NULL;
 
} 