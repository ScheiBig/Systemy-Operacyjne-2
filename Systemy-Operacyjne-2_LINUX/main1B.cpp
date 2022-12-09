#include <cstdio>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include "util/ansi_text.hpp"
#include "util/container.hpp"
#include "util/split.hpp"

#define f_lck "instance.lock"
typedef FILE* file_p;

int main(int argc, char const* argv[], char const* envp[])
{
    std::cout
        << ANSI::b_magenta << "Current pid: "
        << ANSI::reset << getpid() << util::nl;

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

    
    /* ========================================================================== */
    /* Open file used as lock and configure file-lock (write lock for whole file) */
    /* ========================================================================== */
    file_p file = std::fopen(f_lck, "a");
    flock lock{
            F_WRLCK,
            SEEK_SET,
            0,
            0,
            getpid()
    };
    
    /* ============================================================ */
    /* NEW branch - new instance of program should replace previous */
    /* ============================================================ */
    if (is_new)
    {
        /* ========================================================================================== */
        /* Register signal handler for SIGUSR1 - it should unlock file and terminate current instance */
        /* ========================================================================================== */
        signal(SIGUSR1,
            [](int) {
                std::cout << util::nl;
                std::cerr
                    << ANSI::b_blue << "Unlocking file and terminating per request"
                    << ANSI::reset << util::nl;
                file_p f = std::fopen(f_lck, "a");
                flock l{
                    F_UNLCK,
                    SEEK_SET,
                    0,
                    0,
                    getpid()
                };
                fcntl(fileno(f), F_SETLK, &l);
                exit(-3);
            }
        );


        while (true)
        {
            /* ============================================================================== */
            /* Try to acquire lock, if unsuccessful - try signaling lock holder to terminate, */
            /* reset lock configuration and retry                                             */
            /* ============================================================================== */
            if (fcntl(fileno(file), F_SETLK, &lock) != 0)
            {
                fcntl(fileno(file), F_GETLK, &lock);
                std::cerr
                    << ANSI::b_yellow << "Another instance of program is already running: "
                    << ANSI::reset << lock.l_pid << util::nl;
                std::cerr
                    << ANSI::b_blue << "Sending termination request: "
                    << ANSI::reset << SIGUSR1 << util::nl;
                if (kill(lock.l_pid, SIGUSR1) != 0)
                {
                    return -2;
                }
                usleep(500);
            }
            else
            {
                break;
            }
            lock = flock{
                F_WRLCK,
                SEEK_SET,
                0,
                0,
                getpid()
            };
        }

        std::cout
            << ANSI::b_cyan << "Waiting to continue or request termination... "
            << ANSI::reset;

    }
    /* ============================================================================ */
    /* OLD branch - new instance of program should exit if another is still running */
    /* ============================================================================ */
    else
    {
        /* ================================================================= */
        /* Try to acquire lock, if unsuccessful - terminate current instance */
        /* ================================================================= */
        if (fcntl(fileno(file), F_SETLK, &lock) != 0)
        {
            fcntl(fileno(file), F_GETLK, &lock);
            std::cerr
                << ANSI::b_red << "Another instance of program is already running: "
                << ANSI::reset << lock.l_pid << util::nl;
            return -2;
        }
        
        std::cout
            << ANSI::b_cyan << "Waiting to continue... "
            << ANSI::reset;
    }
    
    /* ======================================= */
    /* Wait for user input, or auxilary signal */
    /* ======================================= */
    getchar();

    
    /* =============================== */
    /* Unlock file-lock and close file */
    /* =============================== */
    lock.l_type = F_UNLCK;
    fcntl(fileno(file), F_SETLK, &lock);
    fclose(file);

    return 0;
}
