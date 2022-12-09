#include <iostream>
#include <windows.h>
#include <tchar.h>

#include "util\ansi_text.hpp"
#include "util\bformat.hpp"
#include "util\memory.hpp"

int main(int argc, char const* argv[])
{
    if (argc == 1)
    {
        std::cerr << ANSI::b_red << "No arguments specified!" << ANSI::reset << util::nl;
    }
    else
    {
        /* ============================ */
        /* Create new processes in loop */
        /* ============================ */
        for (int i = 1; i < argc; i++)
        {
            /* ======================================= */
            /* Create process-associated structures... */
            /* ======================================= */
            STARTUPINFO start_i;
            PROCESS_INFORMATION proc_i;
            /* ======================= */
            /* ... and initialize them */
            /* ======================= */
            ZeroMemory(&start_i, sizeof(start_i));
            start_i.cb = sizeof(start_i);
            ZeroMemory(&proc_i, sizeof(proc_i));

            /* ============================================================== */
            /* Create command for child-process program, see main3A_child.cpp */
            /* ============================================================== */
            util::mem<char> command = util::mem<void>::strdup(
                util::bformat(
                    R"(.\Systemy_Operacyjne_2_zadanie_3A_child.exe $)",
                    argv[i]
                ).c_str()
            );

            /* ===================================== */
            /* Create process with default arguments */
            /* ===================================== */
            if (!CreateProcessA(nullptr, command.ptr(), nullptr, nullptr, false, 0, nullptr, nullptr, &start_i, &proc_i))
            {
                std::cerr << ANSI::b_red << "Error: " << ANSI::reset << GetLastError() << util::nl;
            }

            /* ===================================================== */
            /* Join child-process, dispose its data after completion */
            /* ===================================================== */
            WaitForSingleObject(proc_i.hProcess, INFINITE);
            CloseHandle(proc_i.hProcess);
            CloseHandle(proc_i.hThread);
            command.free();
        }
    }

    return 0;
}
