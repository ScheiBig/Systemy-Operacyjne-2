cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(
    "-pedantic"
    "-Wall"
    "-Weverything"

    # "-Werror"
    # "-O3"
    "-Wno-c++98-compat"
    "-Wno-c++98-compat-pedantic"
    "-Wno-old-style-cast"
    "-Wno-missing-noreturn"

    "-ferror-limit=16"
    "-fno-limit-debug-info"
    "-pthread"

    "-fcolor-diagnostics"
    # "-fdiagnostics-color=always"
) 


project(Systemy_Operacyjne_2_zadanie_1A CXX)

add_executable(Systemy_Operacyjne_2_zadanie_1A
    main1A.cpp
    util/linux_semaphore.cpp
)

target_link_libraries(Systemy_Operacyjne_2_zadanie_1A 
    pthread
    rt
)


project(Systemy_Operacyjne_2_zadanie_1B CXX)

add_executable(Systemy_Operacyjne_2_zadanie_1B
    main1B.cpp
)

target_link_libraries(Systemy_Operacyjne_2_zadanie_1B
    pthread
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)