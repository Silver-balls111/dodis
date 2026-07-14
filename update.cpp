#include <cstdio>
#include <exception>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <chrono>
#include <format>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

int main(){
    ifstream read {"task.txt"};
    if (!read)
        throw runtime_error("Error opening test.txt file.");

    float aggregate_opacity {0};
    float final_opacity {0};
    int task_count {0};
    while (true){
        string task;
        getline(read, task);

        if(!read){
            if (read.eof())
                break;
            else
                throw runtime_error("Error reading task");
        }

        float opacity {0};
        while (true){
            cout<<task<<" (done(y)/not-done(n)/percent): ";
            string answer;
            cin>>answer;
            if ((answer == "y") || answer == "yes"){
                opacity = 1;
                break;
            }
            else  if ((answer == "n") || answer == "no"){
                opacity = 0;
                break;
            }
            else{
                try{
                    opacity = stof(answer);
                    if (opacity < 0)
                        opacity = 0;
                    else if (opacity > 100)
                        opacity = 1;
                    else
                        opacity = opacity / 100;
                    break;
                }
                catch (invalid_argument){
                    cout<<"Invalid answer.\n";
                    continue;
                }
            }
        }
        aggregate_opacity += opacity;
        ++task_count;
    }
    final_opacity = aggregate_opacity / task_count;

    ifstream read_contr {"contributions.txt"};
    vector<string> dates;
    vector<float> opacitys;
    stringstream buffer;
    if(read_contr){
        buffer << read_contr.rdbuf();
        while(buffer){
            string date;
            float opac;
            buffer >> date >> opac;
            if (!buffer)
                break;
            dates.push_back(date);
            opacitys.push_back(opac);
        }
    }
    read_contr.close();
    cout<<buffer.str();

    fstream contribution {"contributions.txt", ios::app};
    if (!contribution)
        throw runtime_error("Couldn't open contributions file.");

    auto today = chrono::floor<chrono::days>(chrono::system_clock::now());
    string todays_date = format("{:%Y/%m/%d}", today);

    //auto it = ranges::find(dates, todays_date);
    //while it is one line, it returns a iterator and later we have to find the
    // position and there is signed to unsigned conversion so just avoid that
    size_t index{0};
    bool found = false;
    for (size_t i=0; i<dates.size(); ++i){
        if (dates[i] == todays_date){
            index = i;
            found = true;
            break;
        }
    }
    if (found){
        contribution.close();
        opacitys[index] = final_opacity;
        ofstream contribution {"contributions.txt", ios::out};
        for (size_t i=0; i<dates.size(); ++i){
            contribution << dates[i] << '\t' << setprecision(3)<<opacitys[i] << '\n';
        }
        //call the graph printer fun
        // for now return
        return 0;
    }
    else{
        contribution<<todays_date<<'\t'<<setprecision(3)<<final_opacity<<'\n';
        //call the graph printer fun
        // for now return
        return 0;
    }
}
