cmake_minimum_required(VERSION 3.8)

project(pmu-events VERSION 0.1.0)



execute_process(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/jevents.py all all ${CMAKE_CURRENT_SOURCE_DIR}/arch ${CMAKE_CURRENT_BINARY_DIR}/pmu-events.c)
set (SOURCE_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/pmu-events.c
    src/kernel-defs.c)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL x86_64)
    list(APPEND SOURCE_FILES src/x86/util.c)
endif()

add_library(pmu-events ${SOURCE_FILES})
target_include_directories(pmu-events PUBLIC include)
add_executable(pmu-test examples/main.c)
target_link_libraries(pmu-test pmu-events)

