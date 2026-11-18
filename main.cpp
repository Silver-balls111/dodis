#include <cstring>
#include <iostream>
#include <filesystem>

void Create();
void Update();
void Graph(bool interactive);
void Help();
void Edit();

int main(const int argc, const char **argkv){
    if(argc == 1){
        std::filesystem::path path {"tasks.txt"};
        if (!std::filesystem::exists(path) || std::filesystem::is_empty(path))
            Create();
        else
            Update();
    }
    else if(argc == 2){
        if(!strcmp(argkv[1], "-c")){     //strcmp return 0 if equal so !
            Create();
            return 0;
        }
        else if(!strcmp(argkv[1], "-u")){
            Update();
            // NOTE: not whether to use this or altscreen.
            // std::cout << "\033[2J\033[H" << std::flush; // clear screen, cursor to top-left
            Graph(false);
            return 0;
        }
        else if(!strcmp(argkv[1], "-g")){
            Graph(true);
            return 0;
        }
        else if(!strcmp(argkv[1], "-e")){
            Edit();
            return 0;
        }
        else if(!strcmp(argkv[1], "-h")){
            Help();
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
