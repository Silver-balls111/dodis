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

unsigned int weekdayNo(std::chrono::year_month_day ymd){
    std::chrono::sys_days sd = ymd;
    return std::chrono::weekday(sd).c_encoding();
}

unsigned int weekNo(std::chrono::year_month_day ymd){
    //return the week no given date is, jan1 week is week0
    MonthInfo jan{ymd.year(), std::chrono::January};
    std::chrono::sys_days given_date = ymd;
    int diff = (given_date - jan.start_sys_form).count();
    return (diff + jan.start_weekday_no) / 7;
}

unsigned int weeksInMonth(std::chrono::year year, std::chrono::month month){
    std::chrono::year_month_day start = {year/month/1};
    std::chrono::sys_days s_start = start;
    std::chrono::year_month_day last = {year/std::chrono::month_day_last(month)};
    std::chrono::sys_days s_last = last;

    auto diff = (s_last - s_start).count();
    /*using ceiling is wrong here, we must use +1, because if its exact 35 count
     * after adding the dim days the ceiling won't give correct value. so add 1
     * ex: 2026 may
     */
    return (diff + weekdayNo(start)) / 7 + 1;
}
