cmake_minimum_required(VERSION 3.14)

project(ste-timer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(STE_TIMER_EXAMPLES OFF)

if(STE_TIMER_EXAMPLES)
    add_subdirectory(examples)
endif()

add_subdirectory(include)


