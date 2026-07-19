#include <cmath>
#include <iostream>
#include <chrono>

enum class Days{
    Sun=1, Mon, Tue, Wed, Thu, Fri, Sat
};
using namespace std::chrono;

int main() {
    year_month_day d = 2026y/5/31;
    sys_days sd = d;                 // convert to day-count time_point
    weekday wd = weekday{sd};        // extract weekday

    unsigned idx = wd.c_encoding();  // 0=Sun ... 6=Sat
    unsigned iso = wd.iso_encoding();// 1=Mon ... 7=Sun
    //std::cout<<idx;


    year_month_day jan1 = 2026y/1/1;
    sys_days sjan1 = jan1;

    weekday j1_wd = weekday{sjan1};        // extract weekday

    unsigned j1_idx = j1_wd.c_encoding();  // 0=Sun ... 6=Sat
    unsigned j1_iso = wd.iso_encoding();// 1=Mon ... 7=Sun

    int x = 6 - j1_idx;
    std::cout<<x<<'\n';
    unsigned int week {0};
    if((sd - sjan1).count() - x > 0){
        std::cout<<"in  if\n";
        week = std::ceil((((sd - sjan1).count() - x) / 7) + 1);
    }
    else{
        week = 0;
    }
    auto diff = (sd - sjan1).count();
    std::cout<<diff<<'\n'<<week;
}
