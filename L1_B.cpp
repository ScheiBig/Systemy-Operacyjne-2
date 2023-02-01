#include "interoperability/macro.hpp"

#include <iostream>
#include <string>

#include "interoperability/mutex.hpp"
#include "interoperability/semaphore.hpp"
#include "interoperability/process.hpp"
#include "interoperability/thread.hpp"

#include "util/ansi_text.hpp"

#define mux_name "__mux__L1__B__cpp__"
#define sem_name "__sem__L1__B__cpp__"

using namespace std::string_literals;

int main()
{
    std::cout
        << ANSI::b_magenta << "Current pid: "
        << ANSI::reset << i_op::process::get_current_pid() << util::nl;

    /* ============================================================================================== */
    /* This program, using i_op API and cstdlib, can actually be made without single `#ifdef` branch. */
    /* It must be noted that, without introducing signal handling thread, program will not be         */
    /* responsive while waiting for termination request (it doesn't seem to be topic of exercise tho')*/
    /* ============================================================================================== */

    /* =================================================================================================*/
    /* We create two IPC primitives. Semaphores value can be increased by any process, which means that */
    /* if only one process is waiting for semaphore to be "signaled", the semaphore behaves exactly     */
    /* WinAPIs EventObject. One problem with semaphore is that, while on Windows it will be closed      */
    /* automaticaly when process aborts (via `Ctrl+C` for example), on Linux it won't and new instance  */
    /* will still think, that previous one is running. Thats why additionally, we will use robust mutex */
    /* that can detect that process that locked it is no longer running.                                */
    /* =================================================================================================*/
    i_op::named_mutex abort_detect(mux_name);
    i_op::named_semaphore abort_signal(sem_name, 1);


    /* ================================= */
    /* Detect `SO2` environment variable */
    /* ================================= */
    bool so2 = (getenv("SO2") != nullptr && "NEW"s == getenv("SO2"));

    while (true)
    {
        /* ================================================================ */
        /* Check if we can decrement semaphore - "if event is not signaled" */
        /* ================================================================ */
        if (abort_signal.try_acquire())
        {
            abort_detect.lock();
            break;
        }
        else
        {
            /* ================================== */
            /* Detect if previous instance failed */
            /* ================================== */
            if (abort_detect.try_lock() == i_op::mux_result::Recovered)
            {
                std::cout
                    << ANSI::b_blue << "Previous instance terminated abnormally."
                    << ANSI::reset << util::nl << "Recovered resources." << util::nl;
                abort_signal.release();
                abort_signal.acquire();
                break;
            }

            /* ================================================================= */
            /* Otherwise check if previous instance can be signaled do terminate */
            /* ================================================================= */
            if (so2)
            {
                std::cout
                    << ANSI::b_yellow << "Another instance of program is already running!"
                    << ANSI::reset << util::nl
                    << ANSI::b_blue << "Sending termination request..."
                    << ANSI::reset << util::nl;
                abort_signal.release();

                i_op::thread::sleep(2_tu_s);
                continue;
            }
            else
            {
                std::cerr
                    << ANSI::b_red << "Another instance of program is already running!"
                    << ANSI::reset << util::nl << "Terminate previous instance or try again with "
                    << ANSI::b_blue << "$env:SO2"
                    << ANSI::reset << " == "
                    << ANSI::b_green << R"("NEW")"
                    << ANSI::reset << util::nl;
#ifdef OS_WIN32
                return ERROR_ALREADY_EXISTS;
#endif
#ifdef OS_LINUX
                return EALREADY;
#endif
            }
        }
    }

    /* ===================================================================== */
    /* Attempt decreasing zeroed semaphore - "wait for event to be signaled" */
    /* ===================================================================== */
    std::cout
        << ANSI::b_cyan << "Waiting to continue or request termination... "
        << ANSI::reset << util::nl;
    abort_signal.acquire();

    
    /* ======================================================================== */
    /* Once "event" semaphore was signaled, release all resources and terminate */
    /* ======================================================================== */
    abort_detect.release();
    abort_signal.release();
    std::cout
        << ANSI::b_blue << "Terminating per request"
        << ANSI::reset << util::nl;

    return 0;
}
