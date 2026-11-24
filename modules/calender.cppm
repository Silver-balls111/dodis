module;
#include <cmath>
export module betterman.calender;

import std;

namespace chrono = std::chrono;

export {
    struct MonthInfo {
        MonthInfo(chrono::month);
        MonthInfo(chrono::year, chrono::month);
        void initializer(chrono::year, chrono::month);
        chrono::year_month_day start_ymd_form;
        chrono::sys_days start_sys_form;
        chrono::year_month_day end_ymd_form;
        chrono::sys_days end_sys_form;
        chrono::weekday start_weekday;
        unsigned int start_weekday_num;
        chrono::weekday end_weekday;
        unsigned int end_weekday_num;
    };

    unsigned int weekDayNum(chrono::year_month_day ymd);
    unsigned int weekNumInYear(chrono::year_month_day ymd);
    unsigned int weeksInMonth(chrono::year year, chrono::month month);
    chrono::year_month_day dayInYear(chrono::year year, int diff);
}

export template<bool AsString = true>
auto getZonedDate(){
    const auto now = chrono::system_clock::now();
    const chrono::zoned_time zt{chrono::current_zone(), now};
    const chrono::local_days today = chrono::floor<chrono::days>(zt.get_local_time());
    if constexpr (AsString)
        return std::format("{:%Y/%m/%d}", today); 
    else return today;
}

export template<bool AsString = true>
auto getZonedTime(){
    const auto now = chrono::system_clock::now();
    const chrono::zoned_time zt{chrono::current_zone(), now};
    const chrono::local_time today = chrono::floor<chrono::seconds>(zt.get_local_time());
    if constexpr (AsString)
        return std::format("{:%H:%M:%S}", today); 
    else return today;
}



MonthInfo::MonthInfo(chrono::month month) {
    // Initializes the struct MonthInfo with current_year annd month
    chrono::year_month_day today {getZonedDate<false>()};
    initializer(today.year(), month);
}

MonthInfo::MonthInfo(chrono::year current_year, chrono::month month) {
    // Initializes the struct MonthInfo with given year and month
    initializer(current_year, month);
}

void MonthInfo::initializer(chrono::year year, chrono::month month) {
    // Initializes all the data member of MonthInfo
    start_ymd_form = {year / month / 1};
    start_sys_form = start_ymd_form;
    start_weekday = chrono::weekday(start_sys_form);
    start_weekday_num = start_weekday.c_encoding(); // Sun=0, Sat=6

    end_ymd_form = {year / chrono::month_day_last(month)};
    end_sys_form = end_ymd_form;
    end_weekday = chrono::weekday(end_sys_form);
    end_weekday_num = end_weekday.c_encoding(); // Sun=0, Sat=6
}

unsigned int weekDayNum(chrono::year_month_day ymd) {
    /*
    returns which day (Sun, Mon,...) the given date is
    in integer format (Sun=0, Sat=6)
    */
    chrono::sys_days sd = ymd;
    return chrono::weekday(sd).c_encoding();
}

unsigned int weekNumInYear(chrono::year_month_day ymd) {
    /*
    return the week no given date is, jan1 week is week0
    dec31 is week51/52
    */
    MonthInfo jan{ymd.year(), chrono::January};
    chrono::sys_days given_date = ymd;
    int diff = (given_date - jan.start_sys_form).count();
    return (diff + jan.start_weekday_num) / 7;
}

unsigned int weeksInMonth(chrono::year year, chrono::month month) {
    // Returns number of weeks in given month, 1 indexed
    MonthInfo given{year, month};
    int diff = (given.end_sys_form - given.start_sys_form).count();
    /*
    take 2026 may as example, it has 31 days start day is friday and end day is
    sunday when we calc diff we get (31-1 = 30) that is start from date 2 (Sat)
    end at date 31(Sun) there are 4 completely filled weeks from 3 to 30, we
    need to fill first week, it has 6 days empty from Sun to Fri, so add 6 but
    *start_weekday_num* gives 0 indexed so we add 1 to it now we have 36 days so
    ceil of div by 7.0
    */
    return std::ceil((diff + given.start_weekday_num + 1) / 7.0);
}

chrono::year_month_day dayInYear(const chrono::year year,
                                      const int days_since_jan1) {
    // Returns ymd representation of a given date.
    chrono::year_month_day jan1 = {year / chrono::January / 1};
    chrono::sys_days sjan1 = jan1;
    chrono::year_month_day date =
        sjan1 + chrono::days{days_since_jan1};
    return date;
}
