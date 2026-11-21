cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(dodis)

add_executable(dodis main.cpp)

target_sources(
    dodis
    PRIVATE
        create.cpp
        update.cpp
        graph.cpp
        help.cpp
        editor.cpp
        helpers/colors.cpp
        helpers/monthInfo.cpp
        helpers/conversions.cpp
        helpers/theme_switcher.cpp
)

include(FetchContent)
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/Silver-balls111/FTXUI
    GIT_TAG 5b967a68e51f05ae418ac2933471e7a74a01d779
)
FetchContent_MakeAvailable(ftxui)

FetchContent_Declare(
    efsw
    GIT_REPOSITORY https://github.com/SpartanJ/efsw.git
    GIT_TAG 1.7.3
)
FetchContent_MakeAvailable(efsw)

target_link_libraries(dodis PRIVATE ftxui::ftxui efsw)
