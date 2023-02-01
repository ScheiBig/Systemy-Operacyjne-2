/* ========================================================================== */
/* Author: Marcin Jeznach || plz no steal 😭                                  */
/*                                                                            */
/* Thread wrapper with OS-independent interface. Actual API is heavily        */
/* inspired by `java.lang.Thread`, as it provides very good abstraction of    */
/* threads with minimal tinkering of OS-dependant stuff. This thread wrapper  */
/* supports separate "allocation" and starting of the thread (although this   */
/* not what actually happens underneath), joining, time-suspending and        */
/* priority manipulation of threads, as this are only operations needed in    */
/* majority of usages.                                                        */
/* ========================================================================== */
#pragma once
#include "./macro.hpp"

#ifdef OS_WIN32
#include <windows.h>
#endif
#ifdef OS_LINUX
#include <pthread.h>
#include <map>
#include "./mutex.hpp"
#include "./critical_section.hpp"
#include <unistd.h>
#ifndef __USE_GNU
#include <system_error>
#endif
#endif

#include "./timeunit.hpp"
#include "./error_msg.hpp"

namespace i_op
{
    /**
     * @brief Wrapper that allows creation and choreography of native implementations of threads.
     */
    class thread
    {

    private:


#ifdef OS_WIN32
        /// @brief OS-specific function type of thread's runnable
        typedef LPTHREAD_START_ROUTINE os_runnable;
        /// @brief OS-specific thread handle
        typedef HANDLE thread_handle;
#endif
#ifdef OS_LINUX
        /// @brief OS-specific function type of thread's runnable
        typedef void* (*os_runnable)(void*);
        /// @brief OS-specific thread handle
        typedef pthread_t thread_handle;
#endif

        thread_handle __thr_hdl{};
        os_runnable __thr_main;
        void* __thr_arg;


    public:
#ifdef OS_WIN32
        enum struct priority
        {
            Lowest = THREAD_PRIORITY_LOWEST,
            Low = THREAD_PRIORITY_BELOW_NORMAL,
            Normal = THREAD_PRIORITY_NORMAL,
            High = THREAD_PRIORITY_ABOVE_NORMAL,
            Highest = THREAD_PRIORITY_HIGHEST
        };
        /// @brief Return value of `os_runnable`
        static constexpr DWORD OS_Runnable_OK = 0;
#endif
#ifdef OS_LINUX
        enum struct priority
        {
            Lowest,
            Low,
            Normal,
            High,
            Highest
        };
        /// @brief Return value of `os_runnable`
        static constexpr void* OS_Runnable_OK = nullptr;
#endif

#ifdef OS_LINUX
        /** This variable is not for directs access */
        static std::map<unsigned long long, unsigned long> __thr_tid_lookup;
        /** This variable is not for directs access */
        static i_op::mutex __thr_tid_lookup_lock;
#endif

        /**
         * @brief Allocates a new thread wrapper
         *
         * @param _main Fuction to be executed by thread
         */
        inline thread(os_runnable _main) noexcept
        {
            this->__thr_main = _main;
            this->__thr_arg = nullptr;
        }


        /**
         * @brief Allocates a new thread wrapper
         *
         * @param _main Fuction to be executed by thread
         * @param _args Argument(s) to be passed to `_main`
         */
        inline thread(os_runnable _main, void* _args) noexcept
        {
            this->__thr_main = _main;
            this->__thr_arg = _args;
        }


        /**
         * @brief Creates and begins execution of native thread.
         *
         * Consecutive calls to this fuction will exit immediately without creating additional threads.
         *
         * @return `this`
         * @throw i_op::error_msg – contains OS-specific error code
         */
        inline thread& start() noexcept(false)
        {
            if (this->__thr_main == nullptr) { return *this; }
#ifdef OS_WIN32
            this->__thr_hdl = CreateThread(nullptr, 0, this->__thr_main, this->__thr_arg, 0, nullptr);
            if (this->__thr_hdl == nullptr)
            {
                throw i_op::error_msg{ GetLastError(), "i_op::thread::start()", "CreateThread(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD)" };
            }
#endif
#ifdef OS_LINUX
            int result = pthread_create(&this->__thr_hdl, nullptr, this->__thr_main, this->__thr_arg);
            if (result != 0)
            {
                throw i_op::error_msg{ result, "i_op::thread::start()", "pthread_create(pthread_t*, pthread_attr_t const*, void* (*)(void*), void*)" };
            }
#endif
            this->__thr_main = nullptr;
            return *this;
        }


        /**
         * @brief Suspend execution of current thread until `this` thread terminates.
         * @throw i_op::error_msg – contains OS-specific error code
         */
        inline void join() noexcept(false)
        {
#ifdef OS_WIN32
            if (WaitForSingleObject(this->__thr_hdl, INFINITE) == WAIT_FAILED)
            {
                throw i_op::error_msg{ GetLastError(), "i_op::thread::join()", "WaitForSingleObject(HANDLE, DWORD)" };
            }
#endif
#ifdef OS_LINUX
            int result = pthread_join(this->__thr_hdl, nullptr);
            if (result != 0)
            {
                throw i_op::error_msg{ result, "i_op::thread::join()", "pthread_join(pthread_t, void **)" };
            }
#endif
        }


        /**
         * @brief Suspend execution of current thread until `this` thread terminates.
         * @throw i_op::error_msg – contains OS-specific error code
         * @throw std::system_error – if timed join is not supported by this OS
         */
        inline void join(i_op::time_unit const& _duration) noexcept(false)
        {
#ifdef OS_WIN32
            DWORD wait_time{ (DWORD)((((_duration.h * 60) + _duration.min) * 60 + _duration.s * 1000) + _duration.ms) };
            if (WaitForSingleObject(this->__thr_hdl, wait_time) == WAIT_FAILED)
            {
                throw i_op::error_msg{ GetLastError(), "i_op::thread::join(i_op::time_unit const&)", "WaitForSingleObject(HANDLE, DWORD)" };
            }
#endif
#ifdef OS_LINUX
#ifdef __USE_GNU
            timespec dur;
            dur.tv_sec = ((_duration.h * 60) + _duration.min) * 60 + _duration.s + time(nullptr);
            dur.tv_nsec = ((_duration.ms * 1000) + _duration.us) * 1000 + _duration.ns;
            int result = pthread_timedjoin_np(this->__thr_hdl, nullptr, &dur);
            if (result != 0)
            {
                throw i_op::error_msg{ result, "i_op::thread::join(i_op::time_unit const&)", "pthread_timedjoin_np(pthread_t, void**, timespec const*)" };
            }
#else
            throw std::system_error(std::make_error_code((std::errc)ENOSYS), "pthread_timedjoin_np(pthread_t, void**, timespec const*) not supported on this platform");
#endif
#endif
        }


        /**
         * @return Thread ID of `this` thread. This value may not be available immediately after
         * starting of thread, due to initialization overhead
         */
        inline unsigned long get_tid()
        {
#ifdef OS_WIN32
            return (unsigned long)GetThreadId(this->__thr_hdl);
#endif
#ifdef OS_LINUX
            unsigned long res;
            { i_op::critical_section lck(i_op::thread::__thr_tid_lookup_lock); {

                res = i_op::thread::__thr_tid_lookup[(unsigned long)this->__thr_hdl];
            }}
            return res;
#endif
        }


        /**
         * @brief Set the priority of `this` thread
         *
         * @param _pr Priority to set — it must be noted, that boundary values (like `0`, `TIME_CRITICAL`, etc.)
         *      will not be result of this call.
         * @throw i_op::error_msg – contains OS-specific error code
         */
        inline void set_priority(priority _pr) noexcept(false)
        {
#ifdef OS_WIN32
            if (!SetThreadPriority(this->__thr_hdl, (int)_pr))
            {
                throw i_op::error_msg{ GetLastError(), "i_op::thread::set_priority(priority)", "SetThreadPriority(HANDLE, int)" };
            }
#endif
#ifdef OS_LINUX
            struct sched_param sp;
            int min = sched_get_priority_min(SCHED_OTHER);
            int max = sched_get_priority_max(SCHED_OTHER);

            switch (_pr)
            {
            case priority::Lowest:
                sp.sched_priority = min + 1 * (max - min) / 6;
                break;
            case priority::Low:
                sp.sched_priority = min + 2 * (max - min) / 6;
                break;
            case priority::Normal:
                sp.sched_priority = min + 3 * (max - min) / 6;
                break;
            case priority::High:
                sp.sched_priority = min + 4 * (max - min) / 6;
                break;
            case priority::Highest:
                sp.sched_priority = min + 5 * (max - min) / 6;
                break;
            }

            int result = pthread_setschedparam(this->__thr_hdl, SCHED_OTHER, &sp);
            if (result != 0)
            {
                throw i_op::error_msg{ result, "i_op::thread::set_priority(priority)", "pthread_setschedparam(pthread_t, int, sched_param const*)" };
            }

#endif
        }


        /**
         * @return Closest `priority` mapping of curent priority of `this` thread. This value is polled from OS,
         *      as result actual value might be slightly different due to rounding errors
         * @throw i_op::error_msg – contains OS-specific error code
         */
        inline priority get_priority() noexcept(false)
        {
#ifdef OS_WIN32
            int result = GetThreadPriority(this->__thr_hdl);
            if (result == THREAD_PRIORITY_ERROR_RETURN)
            {
                throw i_op::error_msg{ GetLastError(), "i_op::thread::get_priority()", "GetThreadPriority(HANDLE)" };
            }
            return (priority)result;
#endif
#ifdef OS_LINUX
            struct sched_param sp;
            int p;
            int result = pthread_getschedparam(this->__thr_hdl, &p, &sp);
            if (result != 0)
            {
                throw i_op::error_msg{ result, "i_op::thread::get_priority()", "pthread_getschedparam(pthread_t, int*, sched_param const*)" };
            }

            int min = sched_get_priority_min(p);
            int max = sched_get_priority_max(p);

            int pr = sp.sched_priority;

            if (pr >= (min + 5 * (max - min) / 6)) { return priority::Highest; }
            else if (pr >= (min + 4 * (max - min) / 6)) { return priority::High; }
            else if (pr >= (min + 3 * (max - min) / 6)) { return priority::Normal; }
            else if (pr >= (min + 2 * (max - min) / 6)) { return priority::Low; }
            else { return priority::Lowest; }
#endif
        }



        /**
         * @return Thread ID of current thread
         */
        inline static unsigned long get_current_tid()
        {
#ifdef OS_WIN32
            return GetCurrentThreadId();
#endif
#ifdef OS_LINUX
            return gettid();
#endif
        }


        /**
         * @brief Suspend execution of current thread for at least `_duration` of time
         */
        inline static void sleep(i_op::time_unit const& _duration)
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


        /**
         *  @brief Wraps existing OS-specific thread handle as `thread`
         */
        inline static thread of(thread_handle _thr_hdl) noexcept
        {
            thread t(nullptr);
            t.__thr_hdl = _thr_hdl;
            return t;
        }


        /**
         * @brief Wraps current thread
         */
        inline static thread current() noexcept//?
        {
#ifdef OS_WIN32
            return of(GetCurrentThread());
#endif
#ifdef OS_LINUX
            return of(pthread_self());
#endif
        }
    };
}


/* ====================== */
/* Init of static members */
/* ====================== */

#ifdef OS_LINUX
std::map<unsigned long long, unsigned long> i_op::thread::__thr_tid_lookup;
i_op::mutex i_op::thread::__thr_tid_lookup_lock;
#endif
