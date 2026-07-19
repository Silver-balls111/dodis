#include <chrono>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

unsigned int weekday_no(std::chrono::year_month_day ymd){
    std::chrono::sys_days sd = ymd;
    int x = std::chrono::weekday(sd).c_encoding();
    std::cout<<x<<'\n';
    return x;
}

auto weeksInMonth(std::chrono::year year, std::chrono::month month){
    std::chrono::year_month_day start = {year/month/1};
    std::chrono::sys_days s_start = start;
    std::chrono::year_month_day last = {year/std::chrono::month_day_last(month)};
    std::chrono::sys_days s_last = last;

    int diff = (s_last - s_start).count();
    std::cout<<diff;
    return (diff + weekday_no(start))/ 7 + 1;     //ceiling taken here cuz we need number of week starting from 1
}

int main(){
    using namespace std;
    ifstream file{"contributions.txt"};

    stringstream file_buffer;
    file_buffer<<file.rdbuf();

    //cout<<s.str();
    while (true){
        std::chrono::year_month_day ymd{};
        float opacity{0.0};
        // string temp;
        // file_buffer>>temp;
        // file_buffer>> opacity;
        // stringstream temps;
        // temps<<temp;
        //consume the whitespace because parserser dosen't do it.
        file_buffer >>std::ws >> std::chrono::parse("%Y/%m/%d", ymd)>>opacity;

        //cout<<ymd<<"\t"<<opacity<<'\n';
        cout<<file_buffer.str();
        if(!file_buffer){
            if(file_buffer.eof())
                break;
            else{
                cerr<<"error";
                break;
            }
        }
        cout<<"non buff"<<ymd<<"\t"<<opacity<<'\n';
    }
}
