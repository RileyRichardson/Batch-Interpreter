#include "bat.h"
#define PTR(a) (&a[0])

int main(int argc,char** args){
    bool flags[MAX_FLAGS]={0};
    flags[ECHO]=true;
    std::vector<int> bat_files;
    std::vector<char*> bv; 
    std::vector<char*> m;
    std::vector<bool> bb;
    long long rt=0;
    scope _scope(&bv,&bb,&m,flags,&rt);
    if(argc==1){
        while(true){
            std::string input;
            std::cout<<">>";
            getline(std::cin,input);
            unsigned int size=input.length();
            if(size==0)continue;
            char line[size+1];
            strcpy(PTR(line),input.c_str());
            int res=proccess_line(PTR(line),&_scope);
            if(res==ERROR){
                std::cout<<"an error has occurred\n";
                if(!continue_qm(&_scope))return ERROR;
                else continue;
            }
            else if(res==RETURN)return 0;
        }
        return 0;
    }
    for(int i=1;i<argc;i++){
        if(!strcmp(args[i],"-lbl")){   
            flags[LBL]=true;
        }else if(!strcmp(args[i],"-fbf")){
            flags[FBF]=true;
        }else if(!strcmp(args[i],"-aoe")){
            flags[AOE]=true;
            flags[COE]=false;
        }else if(!strcmp(args[i],"-coe")){
            flags[COE]=true;
            flags[AOE]=false;
        }else if(!strcmp(args[i],"-echo")){
            flags[ECHO]=false;
        }else if(!strcmp(args[i],"-ebf")){
            flags[CBF]=true;
        }else if(!strcmp(args[i],"-fks")){
            flags[FKS]=true;
        }else if(args[i][0]=='-'){
            _scope.addVar(args[i]+1,true);
        }else{
            bat_files.push_back(i);
        }
    }
    if(bat_files.size()==0){
        std::cout<<"no bat file(s) specified\n";
        return 1;
    }
    while(bat_files.size()>0){
        char* file=readFile(args[bat_files[0]]);
        bat_files.erase(bat_files.begin());
        if(!file)continue;
        ridComments(file);
        ridBigSpace(file);
        scope s=_scope;
        if(!flags[FKS])s=scope(&_scope);
        int res=proccess_body(file,&s);
        delete[] file;
        if(res==ERROR){
            std::cout<<"ERROR propagated";
            if(!continue_qm(&s)){
                return ERROR;
            }
        }else if(res==RETURN){
            std::cout<<"RETURN propagated";
            return 0;
        }
    }
    return 0;
}