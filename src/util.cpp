unsigned long long ridSpace(char* src){
    unsigned long long len=strlen(src);
    unsigned long long c=0;
    bool parenth=false;
    while(c<len){
        if(src[c]=='"'&&!parenth){parenth=true;c++;continue;}
        if(parenth){
            if(src[c]=='"'&&src[c-1]!='\\')parenth=false;
            c++;
            continue;
        }
        if(isspace(src[c])){
            memmove(src+c,src+c+1,len-c+1);
            len--;
            continue;
        }
        c++;
    }
    return len;
}
unsigned long long ridBigSpace(char* src){
    unsigned long long len=strlen(src);
    unsigned long long c=0;
    for(int i=0;i<len;i++){
        if(isspace(src[i])){
            src[i]=' ';
        }
    }
    while(c<len){
        if(src[c]==' '){
            int s=0;
            while(src[c+s+1]==' ')s++;
            if(s>0){
                memmove(src+c,src+c+s,len-c-s+1);
                len-=s-1;
                c++;
                continue;
            }
        }
        c++;
    }
    return len;
}
unsigned long long ridComments(char* src){
    unsigned long long len=strlen(src);
    unsigned long long c=0;
    bool p=false;
    while(c<len){
        if(src[c]=='"'&&src[c-1]!='\\')p=!p;
        if(p){c++;continue;}
        if(src[c]=='/'){
            if(src[c+1]=='/'){
                unsigned long long s=c;
                while(src[c]!='\n'&&c<len)c++;
                memmove(src+s,src+c,len-c+1);
                len-=c-s;
                c=s;
            }else if(src[c+1]=='*'){
                unsigned long long s=c;
                c+=2;
                while(src[c]!='*'&&src[c+1]!='/'&&c<len-1)c++;
                memmove(src+s,src+c+2,len-c+1);
                len-=c-s;
                c=s;
            }
        }
        c++;
    }
    return len;
}
bool startsWith(const char* src_nt,const char* prefix){
    bool starts_with=true;
    int i=0;
    while(starts_with&&i<strlen(prefix)){
        starts_with=src_nt[i]==prefix[i];
        i++;
    }
    return starts_with;
}
bool startsWithTok(const char* src,const char* pre,const char* sep){
    unsigned int i=0;
    while(pre[i]!='\0'){
        if(*src=='\0'||*src!=pre[i])return false;
        i++;
        src++;
    }
    const char* f=src+i;
    while(*src!='\0'){
        if(*src==' '){src++;continue;}
        for(int k=0;k<strlen(sep);k++){
            if(*src==sep[k])return true;
        }
        return false;
    }
    return false;
}
bool nextchar(char* src,const char tok){
    while(isspace(*src))src++;
    return *src==tok;
}
int tokenize(char* file){
    unsigned long long file_len=strlen(file);
    int tokc=0;
    bool q=false;
    unsigned long long i=0;
    while(i<file_len){
        if(file[i]=='"'&&file[i-1]!='\\'){
            q=!q;
        }
        if(q){i++;continue;}
        if(file[i]=='('){
            while(file[i]!=')'&&i<file_len){
                i++;
                if(file[i]=='"'){
                    i++;
                    while(!(file[i]=='"'&&file[i]!='\\')&&i<file_len)i++;
                }
            }
            continue;
        }
        if(file[i]=='{'){
            int b=1;
            int c=1;
            while(b&&i+c<file_len){
                if(file[i+c]=='{')b++;
                else if(file[i+c]=='}')b--;
                c++;
            }
            i+=c;
            file[i-1]='\0';
            tokc++;
            continue;
        }else if(file[i]==';'){
            file[i]='\0';
            tokc++;
        }
        i++;
    }
    return tokc;
}
unsigned long long escapeChars(char* src){
    if(*src=='\0')return 0;
    char* begin=src;
    src++;
    while(*src){
        if(src[-1]=='\\'){
            char new_c='\\';
            switch (*src){
                case 'n':new_c='\n';break;
                case 't':new_c='\t';break;
                case '\\':new_c='\\';break;
                case '"':new_c='"';break;
                case 'v':new_c='\v';break;
                case 'b':new_c='\b';break;
                case 'r':new_c='\r';break;
                case 'f':new_c='\f';break;
                case 'a':new_c='\a';break;
                case '\'':new_c='\'';break;
                case '?':new_c='?';break;
            }
            src[-1]=new_c;
            strcpy(src,src+1);
        }
        src++;
    }
    return (unsigned long long)(src-begin-1);
}
bool is_logical_op(char c){
    return c=='|'||c=='&'||c=='^'||c=='('||c==')'||c=='!'||c=='~';
}
char* endLine(char* start){
    bool q=false;
    int p=0;
    int b=0;
    while(*start){
        if(!q&&*start=='"')q=true;
        else if(q&&*start=='"'&&start[-1]!='\\')q=!q;
        if(q){start++;continue;}
        if(*start=='(')p++;
        else if(*start==')')p--;
        if(p<0){return start;}
        if(p>0){start++;continue;}
        if(*start=='{')b++;
        else if(*start=='}')b--;
        if(b>0){start++;continue;}
        if(*start==';'||*start=='}')return start;
        if(is_logical_op(*start))return start;
        start++;
    }
    return start;
}
char* readFile(const char* path){
    std::ifstream file;
    file.open(path,std::ios_base::in);
    if(file){
        file.seekg(0,std::ios_base::end);
        unsigned long long size=file.tellg();
        file.seekg(0,std::ios_base::beg);
        char* contents=new char[size+1];
        contents[size]='\0';
        file.read(contents,size);
        file.close();
        return contents;
    }
    return nullptr;
}
bool expr_eval(std::vector<char>* expr){
    for(unsigned int i=0;i<expr->size();i++){
        if(expr->at(i)=='('){
            unsigned int i2=i;
            int p=1;
            while(p){
                i2++;
                if(expr->at(i2)=='(')p++;
                else if(expr->at(i2)==')')p--;
            }
            i2--;
            std::vector<char> sub_expr(expr->begin()+i+1,expr->begin()+i2);
            bool res=expr_eval(&sub_expr);
            expr->erase(expr->begin()+i,expr->begin()+i2+1);
            expr->insert(expr->begin()+i,res?'1':'0');
        }
    }
    
    for(unsigned int i=0;i<expr->size();i++){
        if(expr->at(i)=='!'){
            if(expr->at(i+1)=='1'){
                expr->at(i)='0';
            }else{
                expr->at(i)='1';
            }
            expr->erase(expr->begin()+i+1);
        }
    }
    while(expr->size()>2){
        bool res=false;
        switch(expr->at(1)){
            case '|':{
                res=expr->at(0)=='1'||expr->at(2)=='1';
            }case '&':{
                res=expr->at(0)=='1'&&expr->at(2)=='1';
            }case '^':{
                res=(expr->at(0)=='1')^(expr->at(2)=='1');
            }case '=':{
                res=expr->at(0)==expr->at(2);
            }
        }
        expr->erase(expr->begin(),expr->begin()+3);
        expr->insert(expr->begin(),res?'1':'0');
    }
    return expr->at(0)=='1';
}   
bool ask(const char* message){
    int tries=0;
    while(tries<3){
        std::cout<<message;
        std::string a;
        getline(std::cin,a);
        if(a[0]=='n'||a[0]=='N'){
            return false;
        }else if(a[0]!='y'&&a[0]!='Y'){
            tries++;
            continue;
        }
        return true;
    }
    std::cout<<"3 strikes equals a no\n";
    return false;
}

bool continue_qm(scope* _scope){
    if(_scope->flags[AOE]){
        std::cout<<"continue?(y/n):";
        int trys=0;
        TRY_AGAIN:
        char a;
        std::cin>>a;
        if(a=='n'||a=='N'){
            return false;
        }else if(a!='y'&&a!='Y'){
            trys++;
            if(trys==3){
                std::cout<<"3 strikes equals a no\n";
                return false;
            }
            std::cout<<"(y/n):";
            goto TRY_AGAIN;
        }
        return true;
    }
    return _scope->flags[COE];
}
char* strLiteral(char* str){
    char* begin=str;
    while(*str!='"'||str[-1]=='\\'&&*str)str++;
    if(!*str)return nullptr;
    *str='\0';
    escapeChars(begin);
    return str;
}
int size_of_file(std::ifstream* file){
    int opos=file->tellg();
    file->seekg(0,std::ios_base::end);
    int size=file->tellg();
    file->seekg(opos);
    return size;
}
int size_of_file(std::ofstream* file){
    int opos=file->tellg();
    file->seekp(0,std::ios_base::end);
    int size=file->tellp();
    file->seekp(opos);
    return size;
}
int nextline(std::ifstream* file,int size=-1,char deli='\n'){
    if(size<0){
        size=size_of_file(file);
    }
    char c=0;
    int len=0;
    int opos=file->tellg();
    while(c!=delim&&opos+len<size){
        file->get(c);
        len++;
    }
    file->seekg(opos);
    return len+(opos+len==size);
}