#include "monthInfo.hpp"
#include <chrono>
#include <cmath>

MonthInfo::MonthInfo(std::chrono::month month){
    //Initializes the struct MonthInfo with current_year annd month 
    std::chrono::sys_days today = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
    std::chrono::year_month_day temp = today;
    auto current_year = temp.year();

    initializer(current_year, month);
}

MonthInfo::MonthInfo(std::chrono::year current_year, std::chrono::month month){
    //Initializes the struct MonthInfo with given year and month
    initializer(current_year, month);
}

void MonthInfo::initializer(std::chrono::year year, std::chrono::month month){
    //Initializes all the data member of MonthInfo
    start_ymd_form = {year/month/1};
    start_sys_form = start_ymd_form;
    start_weekday = std::chrono::weekday(start_sys_form);
    start_weekday_num = start_weekday.c_encoding();  //Sun=0, Sat=6

    end_ymd_form = {year/std::chrono::month_day_last(month)};
    end_sys_form = end_ymd_form;
    end_weekday = std::chrono::weekday(end_sys_form);
    end_weekday_num = end_weekday.c_encoding();  //Sun=0, Sat=6
}

unsigned int weekdayNo(std::chrono::year_month_day ymd){
    /*
    returns which day (Sun, Mon,...) the given date is 
    in integer format (Sun=0, Sat=6)
    */
    std::chrono::sys_days sd = ymd;
    return std::chrono::weekday(sd).c_encoding();
}

unsigned int weekNumYear(std::chrono::year_month_day ymd){
    /*
    return the week no given date is, jan1 week is week0
    dec31 is week52/53
    */
    MonthInfo jan{ymd.year(), std::chrono::January};
    std::chrono::sys_days given_date = ymd;
    int diff = (given_date - jan.start_sys_form).count();
    return (diff + jan.start_weekday_num) / 7;
}

unsigned int weeksInMonth(std::chrono::year year, std::chrono::month month){
    //Returns number of weeks in given month, 1 indexed
    MonthInfo given {year, month};
    int diff = ( given.end_sys_form - given.start_sys_form).count();
    /*
    take 2026 may as example, it has 31 days start day is friday and end day is sunday
    when we calc diff we get (31-1 = 30) that is start from date 2 (Sat) end at 
    date 31(Sun) there are 4 completely filled weeks from 3 to 30, we need to fill 
    first week, it has 6 days empty from Sun to Fri, so add 6 but *start_weekday_num*
    gives 0 indexed so we add 1 to it now we have 36 days so ceil of div by 7.0
    */
    return std::ceil((diff + given.start_weekday_num + 1) / 7.0);
}
