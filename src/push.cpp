#pragma once
bool push(char* line,scope* _scope){
    ridSpace(line);
    line++;
    char* folder=line;
    bool p=false;
    while(*folder!=']'&&*folder||p){
        if(*folder=='"'&&folder[-1]!='\\')p=!p;
        folder++;
    }
    if(!*folder){
        std::cout<<"no closing bracket\n";
        return false;
    }
    *folder='\0';
    folder++;
    char mode=*folder;
    if(*++folder!='>'){
        std::cout<<"expected a >\n";
        return false;
    }
    folder++;
    if(*folder!='"'){
        std::cout<<"expected a string literal for folder\n";
        return false;
    }
    folder++;
    char* end=folder+strlen(folder)-1;
    if(*end!='"'){
        std::cout<<"expected a string literal for folder(2)\n";
        return false;
    }
    *end='\0';
    char* next=line;
    while(next){
        char* tok=next;
        p=false;
        bool is_file=true;
        int b=0;
        
        while((*next!=','||p||b)&&*next){
            if(*next=='"'&&next[-1]!='\\')p=!p;
            else if(!p&&*next=='(')b++;
            else if(!p&&*next==')')b--;
            else if(!p&&!b&&*next==':')is_file=false;
            next++;
            if(b<0){
                std::cout<<"missing starting bracket\n";
                return false;
            }
        }
        if(!*next)next=nullptr;
        else{*next='\0';next++;}
        if(*tok==':')tok++;
        char* read;
        char* write;
        if(*tok=='('){
            write=++tok;
            char* e=++write;
            while(!(*e=='"'&&e[-1]!='\\')&&*e)e++;
            if(!*e){
                std::cout<<"error 1\n";
                return false;
            }
            *e='\0';
            e++;
            if(*e!=')'){
                std::cout<<"error 2\n";
                return false;
            }
            read=++e;
            if(*read==':')read++;
            if(*read!='"'){
                std::cout<<"error 3\n";
                return false;
            }
            read++;
            e=read;
            while(!(*e=='"'&&e[-1]!='\\')&&*e)e++;
            if(!*e){
                std::cout<<"error 1a\n";
                return false;
            }
            *e='\0';
        }else if(*tok=='"'){
            read=++tok;
            char* e=read;
            while(!(*e=='"'&&e[-1]!='\\')&&*e)e++;
            if(!*e){
                std::cout<<"error 5\n";
                return false;
            }
            *e='\0';
            e++;
            if(*e==':')e++;
            if(*e=='('){
                e++;
                if(*e!='"'){
                    std::cout<<"expected a string literal(3)\n";
                    return false;
                }
                write=++e;
                while(!(*e=='"'&&e[-1]!='\\')&&*e)e++;
                if(!*e){
                    std::cout<<"error 1c\n";
                    return false;
                }
                *e='\0';
            }else{
                if(!is_file){
                    std::cout<<"must have a filename to write to\n";
                    return false;
                }
                write=read;
            }

        }else{
            std::cout<<"huh?\n";
            return false;
        }
        char* contents;
        bool heap=false;
        unsigned long long size;
        if(is_file){
            std::ifstream _file;
            _file.open(read,std::ios_base::binary|std::ios_base::in);  
            if(_file){
                heap=true;
                _file.seekg(0,std::ios_base::end);
                size=(unsigned long long)_file.tellg();
                _file.seekg(0,std::ios_base::beg);
                contents=new char[size];
                _file.read(contents,size);
                _file.close();   
            }else{
                std::cout<<"could not read: "<<read<<'\n';
                _file.close();
                return false;
            }
        }else{
            escapeChars(read);
            contents=read;
            size=strlen(read);
        }
        std::ofstream file;
        unsigned long long f=strlen(folder)+1;
        char path[f+strlen(write)+1];
        strcpy(path,folder);
        if(f>1)path[f-1]='/';
        else f=0;
        strcpy(path+f,write);
        if(mode=='~'){
            file.open(path,std::ios_base::binary|std::ios_base::out|std::ios_base::app);
        }else if(mode=='-'){
            file.open(path,std::ios_base::binary|std::ios_base::out);
        }else{
            std::cout<<"unregocnized write mode\n";
            if(heap)delete[] contents;
            return false;
        }
        if(file){
            file.write(contents,size);
            std::cout<<write<<" written\n";
            file.close();  
        }else{
            std::cout<<"could not open: "<<path<<'\n';
        }
        
        if(heap)delete[] contents;
    }
    return true;
}