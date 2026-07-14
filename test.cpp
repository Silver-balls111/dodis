#include <chrono>
#include <fstream>
#include <iostream>
#include <format>
#include <sstream>

int main() {
    auto today = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
    std::cout << std::format("{:%Y/%m/%d}", today) << "\n";

    std::fstream file {"task.txt"};
    std::stringstream scontent;
    std::string content;
    file >> content;
    std::cout<<content;
}
