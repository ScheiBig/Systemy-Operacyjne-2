cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(
    "-pedantic"
    "-Wall"

    # "-Weverything" # clang

    # "-Werror"
    "-O3"

    # "-Wno-c++98-compat" # clang
    # "-Wno-c++98-compat-pedantic" # clang
    # "-Wno-old-style-cast"
    # "-Wno-missing-noreturn"

    # "-ferror-limit=16" #clang
    "-fmax-errors=16" # gcc

    # "-fno-limit-debug-info" #clang
    "-ggdb" # gcc

    # "-pthread"
)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Systemy_Operacyjne_2_zadanie_3A CXX)

add_executable(Systemy_Operacyjne_2_zadanie_3A
    main3A.cpp
)

project(Systemy_Operacyjne_2_zadanie_3A_child CXX)

add_executable(Systemy_Operacyjne_2_zadanie_3A_child
    main3A_child.cpp
)

project(Systemy_Operacyjne_2_zadanie_3B CXX)

add_executable(Systemy_Operacyjne_2_zadanie_3B
    main3B.cpp
)


# include_directories("/usr/share/mingw-w64/include")

# target_link_libraries(Systemy_Operacyjne_2 pthread)