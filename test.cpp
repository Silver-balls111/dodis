#include <ncurses.h>

int main() {
    initscr();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);

    WINDOW *win = newwin(10, 45, 0, 0);
    refresh();
    box(win, 0, 0);
    wattron(win, COLOR_PAIR(1));
    mvwprintw(win, 1, 1, "TEST");
    wattroff(win, COLOR_PAIR(1));
    wrefresh(win);

    getch();
    endwin();
    return 0;
}
