#pragma once

#ifdef OS_WIN32
#include <windows.h>
#endif
#ifdef OS_LINUX
#include <unistd.h>
#endif
#include "./timeunit.hpp"

namespace i_op
{
    class process
    {
    public:
        inline static unsigned long get_current_pid();
        inline static void sleep(i_op::time_unit const& _duration);
    };
}


/* ============== */
/* Implementation */
/* ============== */

inline unsigned long i_op::process::get_current_pid()
{
    #ifdef OS_WIN32
    return (unsigned long)GetCurrentProcessId();
    #endif
    #ifdef OS_LINUX
    return (unsigned long)getpid();
    #endif
}

inline void i_op::process::sleep(i_op::time_unit const& _duration)
{
    #ifdef OS_WIN32
    DWORD wait_time{ (DWORD)((((_duration.h * 60) + _duration.min) * 60 + _duration.s * 1000) + _duration.ms) };
    Sleep(wait_time);
#endif
    #ifdef OS_LINUX
    unsigned int seconds = ((_duration.h * 60) + _duration.min) * 60 + _duration.s;
    unsigned int microseconds = (_duration.ms * 1000) + _duration.us;
    ::sleep(seconds);
    usleep(microseconds);
#endif
}
