cmake_minimum_required(VERSION 3.19)
project(mtfind2)

set(CMAKE_CXX_STANDARD 20)

add_executable(mtfind2
        src/SearchService.cpp
        src/Client.cpp
        src/mtfind2.cpp)
target_link_libraries(mtfind2 pthread)
target_include_directories(mtfind2 PRIVATE include)