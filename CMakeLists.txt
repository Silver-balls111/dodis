cmake_minimum_required(VERSION 3.23)

project(dodis)

add_executable(graph)
target_sources(graph
	PRIVATE
		finalGraph.cpp
		monthInfo.cpp
)

include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v7.0.1
)
FetchContent_MakeAvailable(ftxui)

target_link_libraries(graph PRIVATE
    # Use the umbrella target (recommended)
    ftxui::ftxui
)
