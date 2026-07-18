#include "monthInfo.hpp"
#include <chrono>

MonthInfo::MonthInfo(std::chrono::month month){
    std::chrono::sys_days today = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
    std::chrono::year_month_day temp = today;
    auto current_year = temp.year();

    initializer(current_year, month);
}

MonthInfo::MonthInfo(std::chrono::year current_year, std::chrono::month month){
    initializer(current_year, month);
}

void MonthInfo::initializer(std::chrono::year current_year, std::chrono::month month){
    start_ymd_form = {current_year/month/1};
    start_sys_form = start_ymd_form;
    start_weekday = std::chrono::weekday(start_sys_form);
    start_weekday_no = start_weekday.c_encoding();  //Sun=0, Sat=6

    end_ymd_form = {current_year/std::chrono::month_day_last(month)};
    end_sys_form = end_ymd_form;
    end_weekday = std::chrono::weekday(end_sys_form);
    end_weekday_no = end_weekday.c_encoding();  //Sun=0, Sat=6
}
