#include <iostream>
#include <sstream>
#include <stdexcept>
#include <fstream>

using namespace std;

void Create(){
    string dash(5, '-');
    cout<<dash<<"Create Tasks"<<dash<<'\n';
    ofstream file {"tasks.txt"};
    try{
        if(!file)
            throw runtime_error("Opening tasks.txt file");

        for(int task_count=1;;++task_count){
            cout<<task_count<<". ";
            string temp{};
            getline(cin, temp);
            stringstream task{temp};

            string first_word{};
            task>>first_word;

            if(!task){
                if(task.eof()){
                    //task.clear();
                    return;
                }
                // pretty sure we will never reach the else part
                else{
                    throw runtime_error("Bad input.");
                }
            }

            if((first_word == "q") || (first_word == "quit")){
                string second_word{};
                task>>second_word;
                if(!task){
                    if(task.eof()){
                        return;
                    }
                    else{
                        throw runtime_error("Bad input");
                    }
                }
                //if task didn't fail then this is not exit call
            }

            //if we reached here then it was a valid task not a quit call
            file << task.str() << '\n';
            // do we need to clear the stringstream buffer?
            task.str("");
            task.clear();
        }
   }
   catch(runtime_error e){
       std::cerr<<"Error Occured: "<<e.what()<<'\n';
       return;
   }
}
