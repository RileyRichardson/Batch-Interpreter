#pragma once
int proccess_line(char* tok,scope* _scope);
int proccess_body(char* body,scope* _scope);
int if_bat(char* arg,scope* _scope){
    std::vector<char> expr;
    unsigned int i=0;
    while(arg[i]){
        if(arg[i]=='|')expr.push_back('|');
        else if(arg[i]=='&')expr.push_back('&');
        else if(arg[i]=='^')expr.push_back('^');
        else if(arg[i]=='(')expr.push_back('(');
        else if(arg[i]==')')expr.push_back(')');
        else if(arg[i]=='!'){expr.push_back('!');}
        else if(arg[i]=='~')expr.push_back('=');
        else if(arg[i]!=' '){
            if(arg[i]=='.')i++;
            char* end=endLine(arg+i+1);
            char hold=*end;
            *end='\0';
            int res=proccess_line(arg+i,_scope);
            if(res==ERROR||res==RETURN||res==SUCCESS)return ERROR;
            expr.push_back(res?'1':'0');
            *end=hold;
            arg=end;
            i=0;
            continue;
        }
        i++;
    }
    return expr_eval(&expr);
}
int run_macro(char* body,scope* _scope){
    char* copy=new char[strlen(body)+1];
    memcpy(copy,body,strlen(body)+1);
    scope s(_scope);
    s.in_macro=true;
    int res=proccess_body(copy,&s);
    if(res==RETURN){
        if(_scope->depth<=*_scope->return_to)return SUCCESS;
    }
    return res;
}
int check(char* arg,char* body,scope* _scope){
    int src_files=0;
    int comp_files=0;
    int len=strlen(arg);
    bool post_sc=false;
    for(int i=0;i<len;i++){
        if(arg[i]==','){
            arg[i]='\0';
            if(post_sc)comp_files++;
            else src_files++;
        }else if(arg[i]==';'){
            if(!post_sc){arg[i]='\0';src_files++;post_sc=true;continue;}
            std::cout<<"too may args for check: 2 expected\n";
            return ERROR;
        }
    }
    comp_files++;
    unsigned long long src_time=0;
    for(int i=0;i<src_files;i++){
        struct _stat srcbuf;
        char* next=arg+strlen(arg)+1;
        while(*arg!='"'&&*arg)arg++;
        if(!strLiteral(arg))return ERROR;
        int res=_stat(arg,&srcbuf);
        if(res){
            std::cout<<"could not read file: "<<arg<<"\n";
            return ERROR;
        }
        src_time=src_time<srcbuf.st_mtime?srcbuf.st_mtime:src_time;
        arg=next;
    }
    bool less=false;
    for(int i=0;i<comp_files;i++){
        struct _stat srcbuf;
        while(*arg!='"'&&*arg)arg++;
        char* end=strLiteral(++arg);
        if(!end)return ERROR;
        int res=_stat(arg,&srcbuf);
        if(res||src_time>srcbuf.st_mtime){
            less=true;
            break;
        }
        arg=end+1;
    }
    
    if(less){
        char* c=body;
        while(*c!='{'&&*c)c++;
        scope s(_scope);
        if(*c){
            proccess_body(++c,&s);
        }else if(c-body>0){
            proccess_line(body,&s);
        }
        return TRUE;
    }
    return FALSE;
}

int proccess_line(char* tok,scope* _scope){
    while(*tok==' ')tok++;
    unsigned long long nt=strlen(tok)+1;
    while(tok[nt-1]==' ')tok[--nt]='\0';
    if(*tok=='&'){
        if(_scope->in_macro)return ERROR;
        _scope->addMacro(tok+1);
        return SUCCESS;
    }else if(*tok=='$'){
        bool res=true;
        unsigned int i=1;
        char* var=tok;
        while(tok[i]&&tok[i]!='='&&tok[i]!=' ')i++;
        if(tok[i]=='='||tok[i]==' '){
            tok+=i+1;
            tok[-1]='\0';
            while(*tok==' ')tok++;
            bool _not=false;
            if(*tok=='!'){
                _not=true;
                tok++;
            }
            int p=proccess_line(tok,_scope);
            if(p==-1)return ERROR;
            res=(bool)p;
            if(_not)res=!res;
        }
        _scope->addVar(var,res);
        return (int)res;
    }else if(*tok=='#'){
        tok++;
        while(*tok!='"'&&*tok)tok++;
        if(!*tok)return ERROR;
        char* a=strLiteral(++tok);
        if(!a)return ERROR;
        a++;
        while(*a!='('&&*a)a++;
        if(*a){
            *a='\0';
            char* n=a;
            while(n){
                a=++n;
                while(*n!=','&&*n!=')'&&*n)n++;
                if(*n=='\0')return ERROR;
                if(*n==')'){
                    *n='\0';
                    n=nullptr;
                }else *n='\0';
                bool rstate=true;
                char* res=a;
                while(*res!='='&&*res)res++;
                if(*res){
                    *res='\0';
                    ++res;
                    while(*res==' ')res++;
                    int r=proccess_line(res,_scope); 
                    if(r==ERROR||r==SUCCESS||r==RETURN)return ERROR;
                    rstate=(bool)r;
                }
                if(_scope->varState(a)==-1){
                    _scope->addVar(a,rstate);
                }else{
                    _scope->flagOp(a,rstate);
                }
            }
        }
        char* file=readFile(tok);
        if(!file)return ERROR;
        ridComments(file);
        ridBigSpace(file);
        int res=proccess_body(file,_scope);
        delete[] file;
        return res;
    }else if(*tok=='['){
        bool res=push(tok,_scope);
        if(res)return SUCCESS;
        return ERROR;
    }else if(*tok=='!'){
        tok++;
        while(*tok==' '&&*tok)tok++;
        if(*tok=='\0')return false;
        _scope->flagOp(tok,0);
        return FALSE;
    }else if(*tok=='~'){
        tok++;
        while(*tok==' '&&*tok)tok++;
        if(*tok=='\0')return false;
        _scope->flagOp(tok,-1);
        return _scope->varState(tok);
    }else if(*tok=='-'){
        tok++;
        while(*tok==' '&&*tok)tok++;
        if(*tok=='\0')return FALSE;
        _scope->flagOp(tok,1);
        return TRUE;
    }else if(*tok=='*'){
        tok++;
        while(*tok==' ')tok++;
        unsigned long long len=strlen(tok);
        char cmd[len+10];
        memcpy(cmd,tok,len+1);
        if(!_scope->flags[ECHO]){
            memcpy(cmd+len,">NUL 2>&1",10);
        }else{
            std::cout<<cmd<<'\n';
        }
        int r=std::system(cmd);
        if(r==0)return TRUE;
        if(!_scope->flags[CBF])return FALSE;
        return ERROR;
    }else if(startsWithTok(tok,"check","(")){
        while(*tok!='(')tok++;
        tok++;
        unsigned int q=1;
        while(tok[q]!=')'&&tok[q]!='\0')q++;
        if(tok[q]=='\0')return ERROR;
        tok[q]='\0';
        return check(tok,tok+q+1,_scope);
    }else if(startsWithTok(tok,"out","(")){
        while(*tok!='(')tok++;
        tok++;
        while(*tok!='"'&&*tok)tok++;
        if(!*tok)return ERROR;
        tok++;
        strLiteral(tok);
        std::cout<<tok;
        return SUCCESS;
    }else if(startsWithTok(tok,"echo ask","(")){
        if(!_scope->flags[ECHO])return FALSE;
        while(*tok!='(')tok++;
        tok++;
        while(*tok!='"'&&*tok)tok++;
        if(*tok=='\0')return (int)ask("");
        char* start=++tok;
        tok++;
        while(*tok!='\0'&&!(*tok=='"'&&tok[-1]!='\\'))tok++;
        if(*tok=='\0')return ERROR;
        *tok='\0';
        tok++;
        while(*tok!=')'&&*tok)tok++;
        if(!*tok)return ERROR;
        tok++;
        escapeChars(start);
        bool awnser=ask(start);
        if(!awnser)return FALSE;
        while(*tok==' ')tok++;
        if(*tok){
            scope s(_scope);
            int res;
            if(*tok=='{'){
                tok++;
                res=proccess_body(tok,&s);
            }else{
                res=proccess_line(tok,&s);
            }
            if(res==ERROR)return ERROR;
            if(res==RETURN){
                if(_scope->depth>*_scope->return_to)return RETURN;
            }
        }
        return TRUE;
    }else if(startsWithTok(tok,"echo","(")){
        if(!_scope->flags[ECHO])return SUCCESS;
        while(*tok!='(')tok++;
        tok++;
        while(*tok!='"'&&*tok!='\0')tok++;
        if(*tok=='\0')return SUCCESS;
        tok++;
        char* start=tok;
        while(*tok!='\0'&&(*tok=='"'&&*(tok-1)!='\\'))tok++;
        if(*tok=='\0')return ERROR;
        *tok='\0';
        escapeChars(start);
        std::cout<<start;
        return SUCCESS;
    }else if(startsWithTok(tok,"ask","(")){
        while(*tok!='(')tok++;
        tok++;
        while(*tok!='"'&&*tok)tok++;
        char* end=strLiteral(++tok);
        if(!end)return ERROR;
        end++;
        while(*end!=')'&&*end)end++;
        if(!*end)return ERROR;
        end++;
        bool awnser=ask(tok);
        if(!awnser)return FALSE;
        while(*end==' ')end++;
        if(*end){
            scope s(_scope);
            int res;
            if(*end=='{'){
                end++;
                res=proccess_body(end,&s);
            }else{
                res=proccess_line(end,&s);
            }
            if(res==ERROR)return ERROR;
            if(res==RETURN){
                if(_scope->depth>*_scope->return_to)return RETURN;
            }
        }
        return TRUE;
    }else if(startsWithTok(tok,"if","(")){
        while(*tok!='(')tok++;
        tok++;
        unsigned int q=1;
        bool p=false;
        while((tok[q]!=')'||p)&&tok[q]){
            if(!p&&tok[q]=='"')p=true;
            else if(p&&tok[q]=='"'&&tok[q-1]!='"')p=false;
            q++;
        }
        if(tok[q]=='\0')return ERROR;
        tok[q]='\0';
        _scope->last_result=if_bat(tok,_scope);

        if(_scope->last_result==TRUE){
            char* body=tok+q+1;
            while(*body!='{'&&*body)body++;
            int res;
            if(*body=='\0'){
                body=tok+q+1;
                scope s(_scope);
                res=proccess_line(body,&s);
            }else{
                body++;
                scope s(_scope);
                res=proccess_body(body,&s);
            }
            if(res==RETURN){
                if(_scope->depth>*_scope->return_to){
                    _scope->last_result=RETURN;
                    return RETURN;
                }
            }else if(res==ERROR)return ERROR;
            return TRUE;
        }
        return _scope->last_result;
    }else if(startsWithTok(tok,"else if","(")){
        if(_scope->last_result==TRUE){
            return TRUE;
        }else if(_scope->last_result==FALSE){
            while(*tok!='(')tok++;
            tok++;
            unsigned int q=1;
            while(tok[q]!=')'&&tok[q]!='\0')q++;
            if(tok[q]=='\0')return ERROR;
            tok[q]='\0';
            _scope->last_result=if_bat(tok,_scope);
            if(_scope->last_result==TRUE){
                char* body=tok+q+1;
                while(*body!='{'&&*body!='\0')body++;
                bool res;
                if(*body=='\0'){
                    body=tok+q;
                    scope s(_scope);
                    res=proccess_line(body,&s);
                }else{
                    body++;
                    scope s(_scope);
                    res=proccess_body(body,&s);
                }
                if(res==RETURN){
                    if(_scope->depth>*_scope->return_to){
                        _scope->last_result=RETURN;
                        return RETURN;
                    }
                }else if(res==ERROR)return ERROR;
                return TRUE;
            }
            return _scope->last_result;
        }
        return ERROR;
    }else if(startsWithTok(tok,"else","{")||startsWith(tok,"else ")){
        if(_scope->last_result==TRUE){
            return SUCCESS;
        }else if(_scope->last_result==FALSE){
            char* body=tok;
            while(*tok!='{'&&*tok!='\0')tok++;
            int res;
            if(*tok=='\0'){
                while(*body!=' ')body++;
                body++;
                scope s(_scope);
                res=proccess_line(body,&s);
            }else{
                tok++;
                scope s(_scope);
                res=proccess_body(tok,&s);
            }
            if(res==RETURN){
                if(_scope->depth>*_scope->return_to){
                    _scope->last_result=RETURN;
                    return RETURN;
                }
            }else if(res==ERROR)return ERROR;
            _scope->last_result=SUCCESS;
            return SUCCESS;
        }
        return ERROR;
    }else if(startsWithTok(tok,"for","(")){
        while(*tok!='(')tok++;
        char* num_str=++tok;
        while(*tok!=')'&&*tok!='\0')tok++;
        if(*tok=='\0')return ERROR;
        *tok='\0';
        tok++;
        int loops=atoi(num_str);
        for(int i=0;i<loops;i++){
            scope s(_scope);
            bool error=!run_macro(tok,&s);
            if(error){
               std::cout<<"error";
               return ERROR;
            }
        }
        return SUCCESS;
    }else if(startsWithTok(tok,"args","(")){
        while(*tok!='(')tok++;
        tok++;
        char* n_flag=tok-1;
        while(n_flag){
            tok=++n_flag;
            while(*n_flag!=','&&*n_flag!=')'&&*n_flag)n_flag++;
            if(*n_flag=='\0')return ERROR;
            if(*n_flag==')'){
                *n_flag='\0';
                n_flag=nullptr;
            }else{
                *n_flag='\0';
            }
            if(_scope->varState(tok)==-1)_scope->addVar(tok,false); 
        }
        return SUCCESS;
    }else if(startsWithTok(tok,"control","(")){
        std::string input;
        std::cout<<">>";
        getline(std::cin,input);
        char* line=new char[input.length()+1];
        _scope->addMem(line);
        strcpy(line,input.c_str());
        return proccess_line(line,_scope);
    }else if(startsWithTok(tok,"exit","(")){
        while(*tok!='(')tok++;
        tok++;
        while(*tok==' ')tok++;
        if(*tok==')'){
            *_scope->return_to=0;
            return RETURN;  
        }
        char* end=tok;
        while(*end!=')'&&*end)end++;
        if(*end){
            *end='\0';
            int steps_back=atoi(tok);
            if(steps_back<0){
                steps_back=-steps_back;
                if(steps_back>=_scope->depth)steps_back=_scope->depth;
                long long rto=_scope->depth-steps_back;
                *_scope->return_to=rto;
                return RETURN;
            }
            if(steps_back>=_scope->depth)steps_back=_scope->depth-1;
            *_scope->return_to=steps_back+1;
            return RETURN;
        }
        return ERROR;
    }else if(startsWithTok(tok,"state","(")){
        while(*tok!='(')tok++;
        while(*tok!='"'&&*tok)tok++;
        tok++;
        char* rest_of_arg=strLiteral(tok)+1;
        if(!rest_of_arg)return ERROR;
        std::ifstream state_file;
        char* file_name=tok;
        state_file.open(tok,std::ios_base::in|std::ios_base::binary);
        if(!state_file)return ERROR;
        tok=rest_of_arg;
        while(*tok!=','&&*tok)tok++;
        if(!*tok)return ERROR;
        tok++;
        while(*tok!='"'&&*tok)tok++;
        tok++;
        rest_of_arg=strLiteral(tok)+1;
        char* s_name=tok;
        while(*rest_of_arg!=')'&&*rest_of_arg)rest_of_arg++;
        if(!*rest_of_arg)return ERROR;
        rest_of_arg++;
        while(*rest_of_arg==' ')rest_of_arg++;
        int size=size_of_file(&state_file);
        int len=nextline(&state_file,size);
        while(state_file){
            char data[len];
            int opos=state_file.tellg();
            char* line=&data[0];
            state_file.read(line,len);
            line[len-1]='\0';
            while(*line==' ')line++;
            char* iter=line;
            while(*iter!='='&&*iter)iter++;
            if(!*iter)return ERROR;
            char* val=iter+1;
            while(iter[-1]==' ')iter--;
            *iter='\0';
            if(!strcmp(line,s_name)){
                if(*rest_of_arg=='='){
                    int rest=size-len-opos+1;
                    char* end_file=new char[rest];
                    state_file.read(end_file,rest);
                    state_file.close();
                    rest_of_arg++;
                    while(*rest_of_arg==' ')rest_of_arg++;
                    int res=proccess_line(rest_of_arg,_scope);
                    if(res==ERROR||res==SUCCESS||res==RETURN)return ERROR;
                    std::ofstream ofile;
                    ofile.open(file_name,std::ios_base::binary|std::ios_base::out|std::ios_base::ate|std::ios_base::in);
                    ofile.seekp(opos);
                    ofile.write(s_name,strlen(s_name));
                    ofile.put('=');
                    if(res)ofile.write("true\n",5);
                    else ofile.write("false\n",6);
                    ofile.write(end_file,rest);
                    delete[] end_file;
                    while(ofile.tellp()<size)ofile.put(' ');
                    return res;
                }
                state_file.close();
                while(isspace(*val))val++;
                const char* t="true";
                for(int i=0;i<4;i++){
                    if(val[i]!=t[i]){return FALSE;}
                }
                return TRUE;
            }
            len=nextline(&state_file,size);
        }
        state_file.close();
        return ERROR;
    }else if(_scope->varState(tok)!=-1){
        return _scope->varState(tok);
    }else if(_scope->macro(tok)){
        scope s(_scope);
        _scope->last_result=run_macro(_scope->macro(tok),&s);
        if(_scope->last_result==RETURN){
            if(_scope->depth>=*_scope->return_to)return RETURN;
        }else if(_scope->last_result==ERROR)return ERROR;
        return SUCCESS;
    }else if(!strcmp(tok,"true")||!strcmp(tok,"on")){
        return TRUE;
    }else if(!strcmp(tok,"false")||!strcmp(tok,"off")){
        return FALSE;
    }
    char* t=tok;
    while(*t&&*t!=' '&&*t!='=')t++;
    char p=*t; 
    *t='\0';
    if((p==' '||p=='=')&&_scope->varState(tok)!=-1){
        if(p==' '){
            if(!strcmp(t+1,"on")||!strcmp(t+1,"true")){
                _scope->flagOp(tok,1);
                return TRUE;
            }else if(!strcmp(t+1,"off")||!strcmp(t+1,"false")){
                _scope->flagOp(tok,0);
                return FALSE;
            }else if(!strcmp(t+1,"tog")){
                return (int)_scope->flagOp(tok,-1);
            }else return ERROR;
        }else{
            t++;
            while(*t==' ')t++;
            bool n=false;
            if(*t=='!'){n=true;t++;}
            else if(*t=='.')t++;
            if(*t==' ')t++;
            int r=proccess_line(t,_scope);
            if(r==ERROR||r==SUCCESS)return ERROR;
            if(r==RETURN)return RETURN;
            r=r^n;
            return (int)_scope->flagOp(tok,r);
        }
        
    }
    *t=p;
    unsigned long long len=strlen(tok);
    char cmd[len+10];
    memcpy(cmd,tok,len+1);
    if(!_scope->flags[ECHO]){
        memcpy(cmd+len,">NUL 2>&1",10);
    }else std::cout<<cmd<<'\n';
    int r=std::system(cmd);
    if(r==0)return TRUE;
    if(!_scope->flags[CBF])return FALSE;
    return ERROR;

}
int proccess_body(char* body,scope* _scope){
    int tokc=tokenize(body);
    char* tok=body;
    for(unsigned int i=0;i<tokc;i++){
        unsigned long long nt=strlen(tok);
        char* next=tok+nt+1;
        while(*tok==' '){tok++;nt--;}
        while(tok[nt-1]==' ')tok[--nt]='\0';
        _scope->last_result=proccess_line(tok,_scope);
        if(_scope->last_result==ERROR){
            if(!continue_qm(_scope))
                return ERROR;
        }else if(_scope->last_result==RETURN){
            if(_scope->depth>*_scope->return_to)return RETURN;
            return SUCCESS;
        }
        tok=next;
    }
    return SUCCESS;
}