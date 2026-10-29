cmake_minimum_required(VERSION 3.20)
project(portscanner VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

# библиотека: весь код, кроме main — им пользуются и программа, и тесты
add_library(portscanner_core src/ports.cpp src/scanner.cpp src/syn.cpp)
target_include_directories(portscanner_core PUBLIC include)
target_compile_options(portscanner_core PRIVATE -Wall -Wextra -Wpedantic -Wshadow)

# программа
add_executable(portscanner src/main.cpp)
target_compile_options(portscanner PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
target_link_libraries(portscanner PRIVATE portscanner_core Threads::Threads)

# тесты: GoogleTest скачается сам при первой сборке
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(portscanner_tests tests/test_ports.cpp)
target_link_libraries(portscanner_tests PRIVATE portscanner_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(portscanner_tests)