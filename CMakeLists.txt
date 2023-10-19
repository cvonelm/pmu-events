cmake_minimum_required(VERSION 3.8)

project(pmu-events VERSION 0.0.1)


add_library(pmu-events INTERFACE pmu-events.c)
target_include_directories(pmu-events SYSTEM INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:include>)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL x86_64)
    target_sources(pmu-events PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/x86.c)
endif()

execute_process(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/jevents.py all all ${CMAKE_CURRENT_SOURCE_DIR}/arch pmu-events.c)

