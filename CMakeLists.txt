cmake_minimum_required(VERSION 3.23)

project(dodis)

add_executable(dodis)
target_sources(dodis
    PRIVATE
        main.cpp
        create.cpp
        update.cpp
        graph.cpp
        monthInfo.cpp
)

add_executable(graph)
target_sources(graph
	PRIVATE
		Igraph.cpp
		monthInfo.cpp
)

add_executable(test2)
target_sources(test2
    PRIVATE
        test2.cpp
)

include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v7.0.1
)
FetchContent_MakeAvailable(ftxui)

target_link_libraries(dodis
    PRIVATE
        ftxui::ftxui
)

target_link_libraries(graph PRIVATE
    # Use the umbrella target (recommended)
    ftxui::ftxui
)

target_link_libraries(test2
    PRIVATE
        ftxui::ftxui
)
