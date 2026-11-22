cmake_minimum_required(VERSION 4.5)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(betterman)

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

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/helpers/*.cpp
)

file(GLOB_RECURSE MODULE_FILES CONFIGURE_DEPENDS 
    ${PROJECT_SOURCE_DIR}/modules/*.cppm
)

add_executable(betterman ${SOURCE_FILES})

target_sources(betterman
    PRIVATE
    FILE_SET CXX_MODULES
    BASE_DIRS ${PROJECT_SOURCE_DIR}/modules
    FILES ${MODULE_FILES}
)

target_link_libraries(betterman PRIVATE ftxui::ftxui efsw)
