#include <cstring>
#include <iostream>

void Create();
void Update();
void Graph(bool interactive);
void Help();

int main(const int argc, const char **argkv){
    if(argc == 1){
        Help();
    }
    else if(argc == 2){
        if(!strcmp(argkv[1], "-c")){     //strcmp return 0 if equal so !
            Create();
            return 0;
        }
        else if(!strcmp(argkv[1], "-u")){
            Update();
            Graph(false);
            return 0;
        }
        else if(!strcmp(argkv[1], "-g")){
            Graph(true);
            return 0;
        }
        else{
            std::cerr<<"Invalid argument.";
            Help();
            return 1;
        }
    }
    else{
        std::cerr<<"Invalid argument.";
        Help();
        return 1;
    }
}
