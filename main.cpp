#include <cstring>
#include <iostream>

void Create();
void Update();
void Graph();

int main(const int argc, const char **argkv){
    if(argc == 1){
        //Menu()
    }
    else if(argc == 2){
        if(!strcmp(argkv[1], "-c")){     //strcmp return 0 if equal so !
            Create();
            return 0;
        }
        else if(!strcmp(argkv[1], "-u")){
            Update();
            Graph();
            return 0;
        }
        else if(!strcmp(argkv[1], "-g")){
            Graph();
            return 0;
        }
        else{
            std::cerr<<"Invalid argument.";
            //help()
            return 1;
        }
    }
    else{
        std::cerr<<"Invalid argument.";
        //help()
        return 1;
    }
}
