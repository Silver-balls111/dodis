#include <ncurses.h>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

int main(){
    initscr();

    WINDOW * win = newwin(10, 45, 4, 0);
    refresh();
    box(win, 0, 0);
    wrefresh(win);

    if(!has_colors()){
        printw("Colors not supported");
        getch();
        return -1;
    }

    start_color();

    init_pair(1, COLOR_RED, COLOR_BLACK);
    vector<string> days{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    wattron(win, COLOR_PAIR(1));
    for(int i=0; i<7; ++i){
        mvwprintw(win, i+2, 1, days[i].c_str());
    }
    wattroff(win, COLOR_PAIR(1));
    wattron(win, A_BOLD);
    mvwprintw(win, 1, 1, "Days");
    wattroff(win, A_BOLD);
    wrefresh(win);
    getch();
    endwin();
}
