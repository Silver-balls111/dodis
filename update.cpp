#include <cstdio>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <chrono>
#include <format>
#include <vector>
#include <sstream>

using namespace std;

void Update(){
    try{
        ifstream read {"task.txt"};
        if (!read)
            throw runtime_error("Failed to open tasks file.");

        //the default value is set to 1 as if the file is empty then opacity caluculation
        // would throw 0/0 division error.
        float aggregate_opacity {1};
        float final_opacity {0};
        int task_count {1};
        while (true){
            string task;
            getline(read, task);

            if(!read){
                if (read.eof())
                    break;
                else
                    throw runtime_error("Reading 'task'");
            }

            float opacity {0};
            while (true){
                cout<<task_count<<". "<<task<<" (done(y)/not-done(n)/percent): ";
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
                    catch (invalid_argument i){
                        cout<<"Invalid answer.\n";
                        continue;
                    }
                }
            }
            aggregate_opacity += opacity;
            ++task_count;
        }
        final_opacity = aggregate_opacity / task_count;

        const auto today = chrono::floor<chrono::days>(chrono::system_clock::now());
        const string todays_date = format("{:%Y/%m/%d}", today);
        const string current_year = format("{:%Y}", today);
        const string contribution_file_name = current_year + ".txt";
        // to handle file  being updated multiple a day
        ifstream read_contribution {contribution_file_name};
        vector<string> dates;
        vector<float> opacitys;
        stringstream buffer;
        if(read_contribution){
            buffer << read_contribution.rdbuf();
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
        read_contribution.close();
        cout<<buffer.str();

        fstream contribution_file {contribution_file_name, ios::app};
        if (!contribution_file)
            throw runtime_error("Couldn't open contributions file.");

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
            contribution_file.close();
            opacitys[index] = final_opacity;
            //since we found duplicate entry gotta rewirte the whole file so close and reopen.
            ofstream contribution {contribution_file_name, ios::out};
            for (size_t i=0; i<dates.size(); ++i){
                contribution << dates[i] << '\t' << setprecision(3)<<opacitys[i] << '\n';
            }
            return;
        }
        else{
            contribution_file<<todays_date<<'\t'<<setprecision(3)<<final_opacity<<'\n';
            return;
        }
    }
    catch(exception e){
        std::cerr<<"Error: "<<e.what()<<'\n';
        return;
    }
}
