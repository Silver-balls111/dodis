#include <iostream>
#include <sstream>
#include <stdexcept>
#include <fstream>

using namespace std;

int main(){
    cout<<"Enter task: ";
    int task_count {1};
    while (true){
        cout<<task_count<<". ";
        string temp{};
        getline(cin, temp);
        stringstream task{temp};

        string first_word{};
        task>>first_word;

        if(!task){
            if(task.eof()){
                task.clear();
            }
            else{
                throw runtime_error("Bad input.");
            }
        }

        if((first_word == "q") || (first_word == "quit")){
            string second_word{};
            task>>second_word;
            if(!task){
                if(task.eof()){
                    //return from function
                    return 0;
                }
                if(task.fail()){
                    throw runtime_error("bad input");
                }
            }
            //if task didn't fail then this is not exit call
        }

        //if we reached here then it was a valid task not a quit call
        ofstream file {"tasks.txt", ios::app};
        if(!file)
            throw runtime_error("Error opening tasks.txt file");

        file << task.str() << '\n';
        // do we need to clear the stringstream buffer?
        ++task_count;
    }
}
