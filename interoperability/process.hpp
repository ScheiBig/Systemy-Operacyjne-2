#pragma once
#include "./macro.hpp"

#ifdef OS_WIN32
#include <windows.h>
#endif
#ifdef OS_LINUX
#include <unistd.h>
#endif

namespace i_op
{
    /**
     * @brief Process-related utilities
     */
    class process
    {

    public:
        /**
         * @return Process ID of current process
         */
        inline static unsigned long get_current_pid()
        {
#ifdef OS_WIN32
            return (unsigned long)GetCurrentProcessId();
#endif
#ifdef OS_LINUX
            return (unsigned long)getpid();
#endif
        }

        /**
         * @return Number of CPU (or virtual) cores available in system
         */
        inline static unsigned int get_CPU_count()
        {
#ifdef OS_WIN32
            SYSTEM_INFO sys_inf;
            GetSystemInfo(&sys_inf);
            return (unsigned int)sys_inf.dwNumberOfProcessors;
#endif
#ifdef OS_LINUX
            return (unsigned int)sysconf(_SC_NPROCESSORS_ONLN);
#endif
        }
    };
}
