#pragma once

#include <chrono>

struct MonthInfo{
    MonthInfo(std::chrono::month);
    MonthInfo(std::chrono::year, std::chrono::month);
    void initializer(std::chrono::year, std::chrono::month);
     std::chrono::year_month_day start_ymd_form;
     std::chrono::sys_days start_sys_form;
     std::chrono::year_month_day end_ymd_form;
     std::chrono::sys_days end_sys_form;
     std::chrono::weekday start_weekday;
     unsigned int start_weekday_no;
     std::chrono::weekday end_weekday;
     unsigned int end_weekday_no;
};

unsigned int weekdayNo(std::chrono::year_month_day ymd);
unsigned int weekNo(std::chrono::year_month_day ymd);
unsigned int weeksInMonth(std::chrono::year year, std::chrono::month month);
