struct scope{
    std::vector<char*>* macros;
    bool* flags;
    std::vector<char*>* vars;
    std::vector<bool>* var_states;
    std::vector<char*> allocated_memory;
    unsigned int alloc_c=0;
    long long* return_to;
    bool in_macro=false;
    unsigned int var_c=0;
    unsigned int pre_var_c=0;
    unsigned int depth=1;
    int last_result=SUCCESS;

    void addMem(char* mem){
        allocated_memory.push_back(mem);
    }
    bool flagOp(char* flag,int op){
        if(!strcmp("echo",flag)){
            if(op==-1)flags[ECHO]=!flags[ECHO];
            else if(op)flags[ECHO]=true;
            else flags[ECHO]=false;
            return flags[ECHO];
        }
        if(!strcmp("lbl",flag)){
            if(op==-1)flags[LBL]=!flags[LBL];
            else if(op)flags[LBL]=true;
            else flags[LBL]=false;
            return flags[LBL];
        }
        if(!strcmp("fbf",flag)){
            if(op==-1)flags[FBF]=!flags[FBF];
            else if(op)flags[FBF]=true;
            else flags[FBF]=false;
            return flags[FBF];
        }
        if(!strcmp("aoe",flag)){
            if(op==-1)flags[AOE]=!flags[AOE];
            else if(op)flags[AOE]=true;
            else flags[AOE]=false;
            return flags[AOE];
        }
        if(!strcmp("coe",flag)){
            if(op==-1)flags[COE]=!flags[COE];
            else if(op)flags[COE]=true;
            else flags[COE]=false;
            return flags[COE];
        }
        if(!strcmp("afl",flag)){
            if(op==-1)flags[AFL]=!flags[AFL];
            else if(op)flags[AFL]=true;
            else flags[AFL]=false;
            return flags[AFL];
        }
        for(unsigned long long i=0;i<var_c;i++){
            if(!strcmp(flag,vars->at(i))){
                if(op==-1)var_states->at(i)=!var_states->at(i);
                else if(op)var_states->at(i)=true;
                else var_states->at(i)=false;
                return var_states->at(i);
            }
        }
        return false;
    }
    bool addVar(char* var,bool state){
        if(*var=='$')var++;
        while(*var==' ')var++;
        unsigned long long s=strlen(var);
        while(var[s-1]==' ')var[--s]='\0';
        if(!*var)return false;
        if(!strcmp("echo",var))return false;
        if(!strcmp("lbl",var))return false;
        if(!strcmp("fbf",var))return false;
        if(!strcmp("aoe",var))return false;
        if(!strcmp("coe",var))return false;
        if(!strcmp("afl",var))return false;
        for(unsigned int i=0;i<var_c;i++)if(!strcmp(var,vars->at(i)))return false;
        var_c++;
        vars->push_back(var);
        var_states->push_back(state);
        return true;
    }
    int varState(char* flag){
        if(!strcmp("echo",flag))return flags[ECHO];
        if(!strcmp("lbl",flag))return flags[LBL];
        if(!strcmp("fbf",flag))return flags[FBF];
        if(!strcmp("aoe",flag))return flags[AOE];
        if(!strcmp("coe",flag))return flags[COE];
        if(!strcmp("afl",flag))return flags[AFL];
        for(unsigned long long i=0;i<var_c;i++){
            if(!strcmp(flag,vars->at(i))){
                return var_states->at(i);
            }
        }
        return -1;
    }
    bool addMacro(char* macro){
        macros->push_back(macro);
        return true;
    }
    char* macro(char* tok){
        for(unsigned int i=0;i<macros->size();i++){
            bool match=true;
            unsigned long long k=0;
            while(tok[k]&&match){
                if(!macros->at(i)[k]){match=false;break;}
                match=tok[k]==macros->at(i)[k];
                k++;
            }
            if(!match)continue;
            char* body=macros->at(i)+strlen(tok);
            if(*++body==' '){
                while(*body==' ')body++;
                return body;
            }
        }
        return nullptr;
    }
    scope(scope* s){
        var_c=s->var_c;
        pre_var_c=var_c;
        vars=s->vars;
        var_states=s->var_states;
        in_macro=s->in_macro;
        return_to=s->return_to;
        (*return_to)++;
        flags=s->flags;
        macros=s->macros;
        depth=s->depth+1;
    }
    scope(std::vector<char*>* var_ptr,std::vector<bool>* bool_ptr,std::vector<char*>* macro_ptr,bool* _flags,long long* mr){
        flags=_flags;
        return_to=mr;
        *return_to=0;
        in_macro=false;
        vars=var_ptr;
        var_states=bool_ptr;
        macros=macro_ptr;
    }
    ~scope(){
        vars->erase(vars->begin()+pre_var_c,vars->end());
        var_states->erase(var_states->begin()+pre_var_c,var_states->end());
        for(char* c:allocated_memory){
            delete[] c;
        }
    }
};