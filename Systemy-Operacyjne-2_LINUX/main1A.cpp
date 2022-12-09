#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include "util/ansi_text.hpp"
#include "util/memory.hpp"
#include "util/linux_semaphore.hpp"
#include "util/linux_shared_memory.hpp"

#define sem_name "__semaphore__main1A_cpp__SYSTEMY_OPERASYJNE_2__LINUX__"
#define shm_name "__sh_memory__main1A_cpp__SYSTEMY_OPERASYJNE_2__LINUX__"

/**
 *@brief Wrapper for cstring that allows to consume it only once in verbose way
 */
struct consumable
{
    const char* str;

    /**
     * @return true if consumable is still avalible for consumption
     */
    bool is_aval()
    {
        return this->str != nullptr;
    }

    /**
     * @brief Consumes underlying cstring, so no other subsequent call can consume it
     * @return consumed cstring
     */
    const char* consume()
    {
        const char* result = this->str;
        this->str = nullptr;
        return result;
    }
};

int main(int argc, char const* argv[])
{
    if (argc == 1)
    {
        std::cerr << ANSI::b_red << "No arguments specified!" << ANSI::reset << util::nl;
        return -1;
    }
    else
    {
        if (std::string("-c") == argv[1])
        {
            /* ============================ */
            /* Create new processes in loop */
            /* ============================ */
            for (int i = 2; i < argc; i++)
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
                        std::cout << ANSI::b_green << "Argument: "
                            << ANSI::reset << (char*)arg << ", "
                            << ANSI::b_magenta << "from pid: "
                            << ANSI::reset << getpid()
                            << util::nl;
                        return 0;
                    },
                    buf.ptr() + buf.len(), 0, (void*)argv[i]);

            }
        }
        else if (std::string("-f") == argv[1])
        {

            /* ========================================= */
            /* Create semaphore protecting shared memory */
            /* ========================================= */
            util::named_semaphore lck(sem_name, 0644, 1);
            lck.unlink();

            /* ==================== */
            /* Create shared memory */
            /* ==================== */
            util::shared_memory<consumable, false, true> shm(shm_name, true, true, true, 0644, (unsigned)argc - 2);
            pid_t owner = getpid();


            /* ================================= */
            /* Fill shared memory with arguments */
            /* ================================= */
            for (std::size_t i = 0; i < shm.len(); i++)
            {
                shm.ptr()[i] = consumable{ argv[i + 2] };
            }

            /* ============================ */
            /* Create new processes in loop */
            /* ============================ */
            for (std::size_t i = 0; i < shm.len(); i++)
            {
                /* ================================================================== */
                /* Child process attemps consumption of argument, parent does nothing */
                /* ================================================================== */
                if (fork() == 0)
                {
                    /* ============================================================================= */
                    /* Sleeping for less than milisecond guarantees, that order of arguments is kept */
                    /* during printing                                                               */
                    /* ============================================================================= */
                    usleep((unsigned)(getpid() - owner) * 200u);
                    /* ================================================================ */
                    /* In critical section, check if argument had been consumed alredy, */
                    /* if not, then consume it and print to stdout                      */
                    /* ================================================================ */
                    lck.wait();
                    {
                        if (shm.ptr()[i].is_aval())
                        {
                            std::cout << ANSI::b_green << "Argument: "
                                << ANSI::reset << (shm.ptr() + i)->consume() << ", "
                                << ANSI::b_magenta << "from pid: "
                                << ANSI::reset << getpid()
                                << util::nl;
                        }
                    }
                    lck.post();
                }
            }


            /* ============================================================================== */
            /* Shared memory was mapped by `owner` process, and same process should unlink it */
            /* ============================================================================== */
            if (getpid() == owner)
            {
                shm.unlink();
            }
        }
        else
        {
            std::cerr << ANSI::b_red << "Unknown argument: " << argv[1] << ANSI::reset << util::nl;
        }
    }

    return 0;
}
