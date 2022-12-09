#include <iostream>
#include <windows.h>
#include <excpt.h>

#include "util\ansi_text.hpp"
#include "util\container.hpp"
#include "util\split.hpp"

#define event_name     "__event__main3B_cpp__SYSTEMY_OPERASYJNE_2__WIN32__"
#define mutex_name     "__mutex__main3B_cpp__SYSTEMY_OPERASYJNE_2__WIN32__"
#define shm_mutex_name "__shm_mutex__main3B_cpp__SYSTEMY_OPERASYJNE_2__WIN32__"
#define shm_name       "__sh_memory__main3B_cpp__SYSTEMY_OPERASYJNE_2__WIN32__"

int main(int argc, char const* argv[], char const* envp[])
{
    std::cout
        << ANSI::b_magenta << "Current pid: "
        << ANSI::reset << GetCurrentProcessId() << util::nl;

    /* ======================================================================== */
    /* Create map of envp, parsing format `VARIABLE=value` to [VARIABLE, value] */
    /* ======================================================================== */
    std::map<std::string, std::string> env_variables{
        create_map<char const*, std::string, std::string>(
            envp,
            [](auto e) {
                auto spl{ std::string(e) >>= util::split("=") };
                return std::pair<std::string, std::string>(spl[0], spl[1]);
            }
        )
    };

    bool is_new;

    /* ================================================================= */
    /* Check is env variable `SO2` exists, and if it has a proper format */
    /* ================================================================= */
    try
    {
        if (env_variables.at("SO2") == "new")
        {
            is_new = true;
        }
        else
        {
            std::cerr
                << ANSI::b_red << "Variable "
                << ANSI::b_white << "PO2"
                << ANSI::b_red << " value invalid: "
                << ANSI::reset << env_variables.at("SO2") << util::nl;
            return -1;
        }
    }
    catch (const std::out_of_range&)
    {
        is_new = false;
    }

    /* ============================================ */
    /* Create instance-Mutex and SharedMemory-Mutex */
    /* ============================================ */
    HANDLE mutex = CreateMutexA(nullptr, false, mutex_name);
    HANDLE shm_mutex = CreateMutexA(nullptr, false, shm_mutex_name);
    
    /* ============================= */
    /* Create (or open) SharedMemory */
    /* ============================= */
    HANDLE shm_handle = CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(DWORD), shm_name);
    if (shm_handle == nullptr)
    {
        char* msg{};
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            msg,
            0,
            nullptr
        );
        std::cerr << ANSI::b_red << "Could not create file mapping: " << ANSI::reset << msg << util::nl;
        LocalFree(msg);
        return -2;
    }
    /**************************************************************************************
     *                               = == === TODO === == =
     **************************************************************************************
     *
     * Map SharedMemory to pointer to DWORD - which will be used to access ProcessId
     *
     *************************************************************************************/

     /* ============================================================ */
     /* NEW branch - new instance of program should replace previous */
     /* ============================================================ */
    if (is_new)
    {
        /**************************************************************************************
         *                               = == === TODO === == =
         **************************************************************************************
         *
         * Setup Event
         *
         *************************************************************************************/

         /**************************************************************************************
          *                               = == === TODO === == =
         **************************************************************************************
         *
         * Setup new Thread, which will block waiting for Event, and when it comes,
         * Thanos-snaps entire process, and releases/closes resources
         * (maybe introduce structure for passing resources as arguments to Thread)
         *
         *************************************************************************************/

        /**************************************************************************************
         *                               = == === TODO === == =
         **************************************************************************************
         *
         * In infinite-loop:
         * - try to acquire lock
         *   - if unsuccessful - try firing Event to terminate running instance
         *   - if successful - break the loop
         *
         *************************************************************************************/
        
         /**************************************************************************************
          *                               = == === TODO === == =
          **************************************************************************************
          *
          * Wait for user input (or event - display adequate prompt)
          *
          *************************************************************************************/
    }
    /* ============================================================================ */
    /* OLD branch - new instance of program should exit if another is still running */
    /* ============================================================================ */
    else
    {
        /* ================================================================= */
        /* Try to acquire lock, if unsuccessful - terminate current instance */
        /* ================================================================= */
        if (WaitForSingleObject(mutex, 2000) == WAIT_TIMEOUT)
        {
            
            /* ============================================================== */
            /* Retrieve currenly running instance ProcessId from SharedMemory */
            /* ============================================================== */
            DWORD owner_pid;
            WaitForSingleObject(shm_mutex, INFINITE);
            {
                // TODO //
            }
            ReleaseMutex(shm_mutex);
            
            std::cerr
                << ANSI::b_red << "Another instance of program is already running: "
                << ANSI::reset << util::nl;
            return -2;
        }
        else
        {
            /**************************************************************************************
             *                               = == === TODO === == =
             **************************************************************************************
             *
             * Save current ProcessId in shared memory, so other processes can know which
             * goofy-ahh program cock-blocked them
             *
             *************************************************************************************/
        }

        std::cout
            << ANSI::b_cyan << "Waiting to continue... "
            << ANSI::reset;

        /**************************************************************************************
         *                               = == === TODO === == =
         **************************************************************************************
         *
         * Wait for user input
         *
         *************************************************************************************/
    }

    /**************************************************************************************
     *                               = == === TODO === == =
     **************************************************************************************
     *
     * Unmap SharedMemory, if needed (dunno, WinApi weird af)
     *
     *************************************************************************************/
    /* ================================ */
    /* Unlock mutex and close resources */
    /* ================================ */
    ReleaseMutex(mutex);
    CloseHandle(mutex);
    CloseHandle(shm_mutex);
    CloseHandle(shm_handle);

    return 0;
}
