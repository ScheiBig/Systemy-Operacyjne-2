/* ========================================================================== */
/* Author: Marcin Jeznach || plz no steal 😭                                  */
/*                                                                            */
/* Condition variable primitive with OS-independent interface.                */
/*                                                                            */
/* ========================================================================== */
#pragma once
#include "./macro.hpp"

#ifdef OS_WIN32
#include <windows.h>
#endif
#ifdef OS_LINUX
#include <pthread.h>
#endif

#include "./error_msg.hpp"
#include "./timeunit.hpp"
#include "./mutex.hpp"

namespace i_op
{
    class condition_variable
    {
    private:
        #ifdef OS_WIN32
        
        #endif
        #ifdef OS_LINUX
        pthread_cond_t __cnd_v;
        #endif

    public:
        inline condition_variable();


        inline void wait(i_op::mutex _locked_mux) noexcept(false)
        {

        }


        inline void notify();


        inline void notify_all();
    };
}
