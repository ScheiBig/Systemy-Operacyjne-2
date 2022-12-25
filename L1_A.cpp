#ifdef OS_WIN32
#include <windows.h>
#endif
#ifdef OS_LINUX
#include "interoperability/shared_memory.hpp"
#endif

#include <iostream>
#include <string>

#include "interoperability/mutex.hpp"
#include "interoperability/process.hpp"
#include "interoperability/timeunit.hpp"

#include "util/ansi_text.hpp"

#define mux_name "__mux__L1__A__cpp__"

#ifdef OS_LINUX
#include "util/memory.hpp"

#define shm_name "__shm__L1__A__cpp__"

/** @brief Wrapper for cstring that allows to consume it only once in verbose way */
struct consumable
{
    char const* str;

    /** @return true if consumable is still avalible for consumption */
    bool is_aval() { return this->str != nullptr; }

    /**
     * @brief Consumes underlying cstring, so no other subsequent call can consume it
     * @return consumed cstring
     */
    char const* consume()
    {
        const char* result = this->str;
        this->str = nullptr;
        return result;
    }
};

using namespace std::string_literals;
#endif

int main(int argc, char const* argv[])
{
    /* =============================================================================================== */
    /* Whole main needs to be split, since process creation is fundamentally different between both OS */
    /* =============================================================================================== */
#ifdef OS_WIN32
    /* ====================================================================================================== */
    /* Windows version needs to launch new process similarly to execve(char cons*, char const*, char const*), */
    /* therefore there are no flags in `argv`. Since we don't want to use child programs, we will launch same */
    /* program recursively, consuming only one argument each time and incrementing `argv` pointer before next */
    /* launch. Since last recursion can't distinguish between end of argument, and no arguments, we will use  */
    /* additional mutex to signal the argument consumption                                                    */
    /* ====================================================================================================== */


    /* ===================================================================================================== */
    /* Open mutex and check if its locked (this would signal that the arguments are already being consumed). */
    /* If its locked, consume first argument and continue, otherwise after locking it check for arguments    */
    /* ===================================================================================================== */
    bool locked{ false };
    i_op::named_mutex arg_end(mux_name);
    if (arg_end.try_lock() == i_op::mux_result::Success)
    {
        locked = true;
        if (argc == 1)
        {
            arg_end.release();
            std::cerr
                << ANSI::b_red << "No arguments specified!"
                << ANSI::reset << util::nl;
            return 1;
        }
    }
    else
    {
        if (argc == 1)
        {
            return 0;
        }
    }
    std::cout
        << ANSI::b_green << "Argument: "
        << ANSI::reset << argv[1] << ", "
        << ANSI::b_magenta << "from pid: "
        << ANSI::reset << i_op::process::get_current_pid()
        << util::nl;

    /* ============================================= */
    /* Create and initialize process-associated data */
    /* ============================================= */
    STARTUPINFO start_i;
    PROCESS_INFORMATION proc_i;
    ZeroMemory(&start_i, sizeof(start_i));
    start_i.cb = sizeof(start_i);
    ZeroMemory(&proc_i, sizeof(proc_i));

    std::string cmd{ argv[0] };
    for (int i = 2; i < argc; i++)
    {
        cmd += ' ';
        cmd += argv[i];
    }

    /* ===================================== */
    /* Create process with default arguments */
    /* ===================================== */
    if (!CreateProcessA(nullptr, (LPSTR)cmd.c_str(), nullptr, nullptr, false, 0, nullptr, nullptr, &start_i, &proc_i))
    {
        std::cerr << i_op::error_msg{ GetLastError(), "main(int, char const**)", "CreateProcessA(LPCSTR, LPSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, WINBOOL, DWORD, LPVOID, LPCSTR, LPSTARTUPINFOA, LPPROCESS_INFORMATION)" };
    }

    /* ===================================================== */
    /* Join child-process, dispose its data after completion */
    /* ===================================================== */
    WaitForSingleObject(proc_i.hProcess, INFINITE);
    CloseHandle(proc_i.hProcess);
    CloseHandle(proc_i.hThread);
    if (locked) { arg_end.release(); }

#endif
#ifdef OS_LINUX
    /* ========================================================================================================== */
    /* Linux versions uses two distinct process-creating facilities; forking current process in-place and running */
    /* new code specified in function (thread-like way). This requires flags in `argv` to specify what kind of    */
    /* function do we want to use. Both functions will be launched in for-loops Flag `c` will indicate `clone(..)`*/
    /* function, which will launch function, that simply prints current iterations argument. Other flags mean     */
    /* that `fork()` function will be used, before printing next argument. Since each iteration will have 2^i     */
    /* processes, `consumable` will be used to ensure that each argument can be printed only once. Since          */
    /* `struct consumable` is not thread-safe, mutex will be used to wrap consumption in critical section.        */
    /* ========================================================================================================== */


    /* =========== */
    /* Check flags */
    /* =========== */
    if (argc <= 2)
    {
        std::cerr
            << ANSI::b_red << "No arguments specified!"
            << ANSI::reset << util::nl;
        return 1;
    }
    else if ("c"s == argv[1]) for (int i = 2; i < argc; i++)
    {
        /* ============================ */
        /* Create stack for new process */
        /* ============================ */
        util::mem<int> buf = util::mem<int>::calloc(1024_z);

        /* ========================================================================== */
        /* Create new process, as process function passing lambda that accepts single */
        /* cstring and prints it to stdout, as process parameter - current argument   */
        /* ========================================================================== */
        clone(
            [](void* arg) {
                std::cout
                    << ANSI::b_green << "Argument: "
                    << ANSI::reset << (char*)arg << ", "
                    << ANSI::b_magenta << "from pid: "
                    << ANSI::reset << i_op::process::get_current_pid()
                    << util::nl;
                return 0;
            },
            buf.ptr() + buf.len(), 0, (void*)argv[i]);
    }
    else
    {
        /* ===================================== */
        /* Create mutex protecting shared memory */
        /* ===================================== */
        i_op::named_mutex crit_sec(mux_name);

        /* ========================================== */
        /* Create shared memory and fill it with data */
        /* ========================================== */
        i_op::shared_memory<consumable> shmem(shm_name, argc - 2);
        for (std::size_t i = 0; i < shmem.len(); i++)
        {
            shmem.ptr()[i] = consumable{ argv[i + 2] };
        }
        pid_t owner = (pid_t)i_op::process::get_current_pid();

        /* ================================================================== */
        /* Child process attemps consumption of argument, parent does nothing */
        /* ================================================================== */
        for (std::size_t i = 0; i < shmem.len(); i++) if (fork() == 0)
        {

            /* ==================================================================== */
            /* Sleeping for little time guarantees, that order of arguments is kept */
            /* during printing                                                      */
            /* ==================================================================== */
            i_op::process::sleep(2_tu_ms * i);
            /* ================================================================= */
            /* In critical section, check if argument had been consumed already, */
            /* if not, then consume it and print to stdout                       */
            /* ================================================================= */
            crit_sec.lock();
            if (shmem.ptr()[i].is_aval())
            {
                std::cout
                    << ANSI::b_green << "Argument: "
                    << ANSI::reset << (shmem.ptr() + i)->consume() << ", "
                    << ANSI::b_magenta << "from pid: "
                    << ANSI::reset << (pid_t)i_op::process::get_current_pid()
                    << util::nl;
            }
            crit_sec.release();
        }

        /* ====================================================================================== */
        /* Shared memory was mapped by `owner` process, and same process should manually close it */
        /* ====================================================================================== */
        if ((pid_t)i_op::process::get_current_pid() == owner)
        {
            shmem.close();
        }
    }
#endif

    return 0;
}
