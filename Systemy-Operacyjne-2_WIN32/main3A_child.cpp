#include <iostream>
#include <windows.h>

#include "util/ansi_text.hpp"


/* =============================================================================== */
/* Child-process program for main3A.cpp - it accepts single argument and prints it */
/* along with its ProcessId                                                        */
/* =============================================================================== */
int main(int argc, char const* argv[])
{
    if (argc != 2)
    {
        std::cerr << ANSI::b_red << "No argument specified!" << ANSI::reset << util::nl;
    }
    
    std::cout << ANSI::b_green << "Argument: "
        << ANSI::reset << argv[1] << ", "
        << ANSI::b_magenta << "from pid: "
        << ANSI::reset << GetCurrentProcessId()
        << util::nl;

    return 0;
}
