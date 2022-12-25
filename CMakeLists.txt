cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)

project(SO2_L1_A CXX)
project(SO2_L1_B CXX)

message("G++ compiler required!")
add_compile_options(
    "-pedantic"
    "-Wall"
    "-Werror"
    "-fmax-errors=16"

    "-fdiagnostics-color=always"

    "$<$<CONFIG:RELEASE>:-O3>"
    "$<$<CONFIG:DEBUG>:-ggdb>"
)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_compile_options(
        "-pthread"
        "-DOS_LINUX"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_compile_options(
        "-DOS_WIN32"
    )
endif()

add_executable(SO2_L1_A
    L1_A.cpp
)

add_executable(SO2_L1_B
    L1_B.cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(SO2_L1_A
        "pthread"
        "rt"
    )
    target_link_libraries(SO2_L1_B
        "pthread"
        "rt"
    )
endif()
