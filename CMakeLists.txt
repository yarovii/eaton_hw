cmake_minimum_required(VERSION 3.14)
project(eaton_hw)

set(CMAKE_CXX_STANDARD 14)

file(GLOB source_files
        "*.h"
        "*.cpp"
        "Classes/*/*.cpp"
        "Classes/*/*.h"
        )

add_executable(Server ${source_files})