cmake_minimum_required(VERSION 3.23)
project(dodis)

add_executable(dodis main.cpp)

target_sources(
    dodis
    PRIVATE
        create.cpp
        update.cpp
        graph.cpp
        help.cpp
        helpers/monthInfo.cpp
        helpers/conversions.cpp
)

include(FetchContent)
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/Silver-balls111/FTXUI
    GIT_TAG 5b967a68e51f05ae418ac2933471e7a74a01d779
)
FetchContent_MakeAvailable(ftxui)

target_link_libraries(dodis PRIVATE ftxui::ftxui)
