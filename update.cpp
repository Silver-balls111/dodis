#include <chrono>
#include <cstdio>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

using namespace std;

void Update() {
    try {
        ifstream read{"tasks.txt"};

        if (std::filesystem::is_empty("tasks.txt")) {
            throw runtime_error("Task file empty. Nothing to update.");
        }
        if (!read) {
            throw runtime_error("Failed to open tasks file.");
        }

        float aggregate_opacity{0};
        float final_opacity{0};
        int task_count{0};
        while (true) {
            string task;
            getline(read, task);

            if (!read) {
                if (read.eof())
                    break;
                else
                    throw runtime_error("Reading 'task'");
            }

            ++task_count;
            float opacity{0};

            while (true) {
                cout << task_count << ". " << task
                     << " [done(y)/not-done(n)/percent]: ";
                string answer;
                cin >> answer;
                if ((answer == "y") || answer == "yes") {
                    opacity = 1;
                    break;
                } else if ((answer == "n") || answer == "no") {
                    opacity = 0;
                    break;
                } else {
                    try {
                        opacity = stof(answer);
                        if (opacity < 0)
                            opacity = 0;
                        else if (opacity > 100)
                            opacity = 1;
                        else
                            opacity = opacity / 100;
                        break;
                    } catch (invalid_argument i) {
                        cout << "Invalid answer.\n";
                        continue;
                    }
                }
            }
            aggregate_opacity += opacity;
        }
        final_opacity = aggregate_opacity / task_count;

        const auto today =
            chrono::floor<chrono::days>(chrono::system_clock::now());
        const string todays_date = format("{:%Y/%m/%d}", today);
        const string current_year = format("{:%Y}", today);
        std::filesystem::path contribution_file_path{
            std::format("contributions/{}.txt", current_year)};

        /*
        if we had just appended to the end of the file, when the program is run
        multiple time the same day there would be duplicate entry,
        thus we rewrite the file each time, this also implies if the system date
        is changed by the user to old dates then future entries will be removed.
        */
        ifstream read_contribution{contribution_file_path};
        vector<string> dates;
        vector<float> opacitys;
        stringstream buffer;
        if (read_contribution) {
            // makes buffer a pointer to the contribution file buffer
            // so we don't have to worry about whitespce while reading
            buffer << read_contribution.rdbuf();
            while (buffer) {
                string date;
                float opac;
                buffer >> date >> opac;
                if (!buffer || (date == todays_date)) {
                    cout << '\n' << dates.size();
                    break;
                }
                dates.push_back(date);
                opacitys.push_back(opac);
            }
        }
        read_contribution.close();

        dates.push_back(todays_date);
        opacitys.push_back(final_opacity);

        ofstream contribution_file{contribution_file_path};
        if (!contribution_file)
            throw runtime_error("Couldn't open contributions file.");

        for (size_t i = 0; i < dates.size(); ++i) {
            contribution_file << format("{}\t{:.3}\n", dates[i], opacitys[i]);
        }

        return;
    } catch (runtime_error e) {
        std::cerr << "Error: " << e.what() << '\n';
        return;
    }
}
