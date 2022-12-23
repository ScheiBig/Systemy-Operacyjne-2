/* ========================================================================== */
/* Author: Marcin Jeznach || plz no steal 😭                                  */
/*                                                                            */
/* Mutex primitive with OS-independent interface. Since Linux doesn't have    */
/* native named mutex                                                         */
/* ========================================================================== */
#pragma once

#include "macro.hpp"

#ifdef OS_WIN32
#include <windows.h>
#endif
#ifdef OS_LINUX
#include <pthread.h>
#endif

#include "./timeunit.hpp"

namespace i_op
{
    /**
     * @brief Common interface for mutexes
     */
    class basic_mutex
    {
    protected:
#ifdef OS_WIN32
        HANDLE __mux_hdl{ nullptr };
#endif
#ifdef OS_LINUX
        pthread_mutex_t* __mux_ptr;
#endif
        basic_mutex();
        basic_mutex(basic_mutex const&) = delete;
    public:
        /**
         * @brief Locks the mutex, waiting indefinitely if it's currently locked
         * @throw i_op::error_msg contains a OS-specific error code
         */
        void lock() noexcept(false);
        /**
         * @brief Locks the mutex, waiting no longer(ish) than `_duration` if it's currently locked
         * @param _duration Maximal(ish) waiting time. If duration points past /03:14:08 UTC, 2038.01.19/, behaviour may be undefined
         * @throw i_op::error_msg contains a OS-specific error code
         */
        void lock(i_op::time_unit const& _duration) noexcept(false);

        /**
         * @brief Locks the mutex, exiting immediately if it's currently locked
         * @return true if the lock was successful
         * @throw i_op::error_msg – contains a OS-specific error code
         */
        bool try_lock() noexcept(false);

        /**
         * @brief Locks the mutex, exiting no longer(ish) than after `_duration` if it's currently locked
         * @return true if the lock was successful
         * @throw i_op::error_msg – contains a OS-specific error code
         */
        bool try_lock(i_op::time_unit const& _duration) noexcept(false);

        /**
         * @brief Releases the mutex
         * @throw i_op::error_msg – contains OS-specific error code
         */
        void release() noexcept(false);

        /**
         * @brief Performs destruction of object, throwing on failure
         * @throw i_op::error_msg – contains OS-specific error code
         */
        void close() noexcept(false);
    };

    /**
     * @brief Unnamed mutex
     */
    class mutex final: public basic_mutex
    {
    private:
#ifdef OS_LINUX
        pthread_mutex_t __mux_v;
#endif
        mutex(mutex const&) = delete;
    public:
        /**
         * @brief Construct a new mutex object
         */
        mutex() noexcept(false);

        /**
         * @brief Performs destruction of object, ignoring exceptions on failure
         */
        ~mutex();
    };

    /**
     * @brief Named mutex
     * @tparam _cr_new if false, object will open the mutex if one with specified name already exist,
     *         if true, will fail
     */
    template<bool _cr_new = false>
    class named_mutex final: public basic_mutex
    {
    private:
#ifdef OS_LINUX
        int __mux_mem_fl_dtr{ -1 };
#endif
        char const* __name;
        named_mutex(named_mutex const&) = delete;
    public:
        /**
         * @brief Construct a new mutex object
         * @param _name name of the mutex
         */
        named_mutex(char const* _name) noexcept(false);

        /**
         * @brief Performs destruction of object, ignoring exceptions on failure
         */
        ~named_mutex();

        /**
         * @brief Performs destruction of object (specific to named mutex), throwing on failure
         * @throw i_op::error_msg – contains OS-specific error code
         */
        void unlink() noexcept(false);
    };
}


/* ============== */
/* Implementation */
/* ============== */

#include "./error_msg.hpp"
#ifdef OS_WIN32
#include <excpt.h>
#endif
#ifdef OS_LINUX
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

i_op::basic_mutex::basic_mutex() {}

void i_op::basic_mutex::lock() noexcept(false)
{
#ifdef OS_WIN32
    if (WaitForSingleObject(this->__mux_hdl, INFINITE) != WAIT_OBJECT_0)
    {
        throw i_op::error_msg{ GetLastError(), "i_op::basic_mutex::lock()", "WaitForSingleObject(HANDLE, DWORD)" };
    }
#endif
#ifdef OS_LINUX
    int result = pthread_mutex_lock(this->__mux_ptr);
    if (result != 0)
    {
        throw i_op::error_msg{ result, "i_op::basic_mutex::lock()", "pthread_mutex_lock(pthread_mutex_t*)" };
    }
#endif
}

void i_op::basic_mutex::lock(i_op::time_unit const& _duration) noexcept(false)
{
#ifdef OS_WIN32
    DWORD wait_time{ (DWORD)((((_duration.h * 60) + _duration.min) * 60 + _duration.s * 1000) + _duration.ms) };
    if (WaitForSingleObject(this->__mux_hdl, wait_time) != WAIT_OBJECT_0)
    {
        throw i_op::error_msg{ GetLastError(), "i_op::basic_mutex::lock(i_op::time_unit const&)", "WaitForSingleObject(HANDLE, DWORD)" };
    }
#endif
#ifdef OS_LINUX
    timespec dur;
    dur.tv_sec = ((_duration.h * 60) + _duration.min) * 60 + _duration.s + time(nullptr);
    dur.tv_nsec = ((_duration.ms * 1000) + _duration.us) * 1000 + _duration.ns;
    int result = pthread_mutex_timedlock(this->__mux_ptr, &dur);
    if (result != 0)
    {
        throw i_op::error_msg{ result, "i_op::basic_mutex::lock(i_op::time_unit const&)", "pthread_mutex_timedlock(pthread_mutex_t*, struct timespec const*)" };
    }
#endif
}

bool i_op::basic_mutex::try_lock() noexcept(false)
{
#ifdef OS_WIN32
    switch (WaitForSingleObject(this->__mux_hdl, 0))
    {
    case WAIT_OBJECT_0: return true;
    case WAIT_TIMEOUT: return false;
    default: throw i_op::error_msg{ GetLastError(), "i_op::basic_mutex::try_lock()", "WaitForSingleObject(HANDLE, DWORD)" };
    }
#endif
#ifdef OS_LINUX
    int result = pthread_mutex_trylock(this->__mux_ptr);
    switch (result)
    {
    case 0: return true;
    case EBUSY: return false;
    default: throw i_op::error_msg{ result, "i_op::basic_mutex::try_lock()", "pthread_mutex_trylock(pthread_mutex_t*)" };
    }
#endif
}

bool i_op::basic_mutex::try_lock(i_op::time_unit const& _duration) noexcept(false)
{
#ifdef OS_WIN32
    DWORD wait_time{ (DWORD)((((_duration.h * 60) + _duration.min) * 60 + _duration.s * 1000) + _duration.ms) };
    switch (WaitForSingleObject(this->__mux_hdl, wait_time))
    {
    case WAIT_OBJECT_0: return true;
    case WAIT_TIMEOUT: return false;
    default: throw i_op::error_msg{ GetLastError(), "i_op::basic_mutex::try_lock(i_op::time_unit const&)", "WaitForSingleObject(HANDLE, DWORD)" };
    }
#endif
#ifdef OS_LINUX
    timespec dur;
    dur.tv_sec = ((_duration.h * 60) + _duration.min) * 60 + _duration.s + time(nullptr);
    dur.tv_nsec = ((_duration.ms * 1000) + _duration.us) * 1000 + _duration.ns;
    int result = pthread_mutex_timedlock(this->__mux_ptr, &dur);
    switch (result)
    {
    case 0: return true;
    case ETIMEDOUT: return false;
    default: throw i_op::error_msg{ result, "i_op::basic_mutex::try_lock(i_op::time_unit const&)", "pthread_mutex_timedlock(pthread_mutex_t*, struct timespec const*)" };
    }
#endif
}

void i_op::basic_mutex::release() noexcept(false)
{
#ifdef OS_WIN32
    if (!!ReleaseMutex(this->__mux_hdl) == false)
    {
        throw i_op::error_msg{ GetLastError(), "i_op::basic_mutex::release()", "ReleaseSemaphore(HANDLE, LONG, LPLONG)" };
    }
#endif
#ifdef OS_LINUX
    int result = pthread_mutex_unlock(this->__mux_ptr);
    if (result != 0)
    {
        throw i_op::error_msg{ result, "i_op::basic_mutex::release()", "pthread_mutex_unlock(pthread_mutex_t*)" };
    }
#endif
}

void i_op::basic_mutex::close() noexcept(false)
{
#ifdef OS_WIN32
    if (!!CloseHandle(this->__mux_hdl) == false)
    {
        throw i_op::error_msg{ GetLastError(), "i_op::basic_mutex::close()", "CloseHandle(HANDLE)" };
    }
    this->__mux_hdl = nullptr;
#endif
#ifdef OS_LINUX
    int result = pthread_mutex_destroy(this->__mux_ptr);
    if (result != 0)
    {
        throw i_op::error_msg{ result, "i_op::basic_mutex::close()", "pthread_mutex_destroy(pthread_mutex_t*)" };
    }
#endif
}



i_op::mutex::mutex() noexcept(false)
{
#ifdef OS_WIN32
    this->__mux_hdl = CreateMutexA(nullptr, false, nullptr);
    if (this->__mux_hdl == nullptr)
    {
        throw i_op::error_msg{ GetLastError(), "i_op::mutex::mutex()", "CreateMutexA(LPSECURITY_ATTRIBUTES, WINBOOL, LPCSTR)" };
    }
#endif
#ifdef OS_LINUX
    pthread_mutexattr_t attr;
    (void)pthread_mutexattr_init(&attr);
    (void)pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    (void)pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    int result = pthread_mutex_init(&this->__mux_v, &attr);
    if (result != 0)
    {
        pthread_mutexattr_destroy(&attr);
        throw i_op::error_msg{ result, "i_op::mutex::mutex()", "pthread_mutex_init(pthread_mutex_t*)" };
    }

    pthread_mutexattr_destroy(&attr);
    this->__mux_ptr = &this->__mux_v;
#endif
}

i_op::mutex::~mutex()
{
    try
    {
        if (
#ifdef OS_WIN32
            this->__mux_hdl != nullptr
#endif
#ifdef OS_LINUX
            this->__mux_ptr != nullptr
#endif
            )
        {
            this->close();
#ifdef OS_LINUX
            this->__mux_ptr = nullptr;
#endif
        }
    }
    catch (i_op::error_msg const&) {}
}



template<bool _cr_new>
i_op::named_mutex<_cr_new>::named_mutex(char const* _name) noexcept(false)
{
#ifdef OS_WIN32
    this->__mux_hdl = CreateMutexA(nullptr, false, _name);
    DWORD err = GetLastError();
    if (this->__mux_hdl == nullptr)
    {
        throw i_op::error_msg{ err, "i_op::named_mutex<bool>::named_mutex(char const*)", "CreateMutexA(LPSECURITY_ATTRIBUTES, WINBOOL, LPCSTR)" };
    }
    if (_cr_new && err == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(this->__mux_hdl);
        throw i_op::error_msg{ err, "i_op::named_mutex<true>::named_mutex(char const*)", "CreateMutexA(LPSECURITY_ATTRIBUTES, WINBOOL, LPCSTR)" };
    }
#endif
#ifdef OS_LINUX
    int tmp;

    this->__mux_mem_fl_dtr = shm_open(_name, O_CREAT | O_RDWR, 0664);
    if (this->__mux_mem_fl_dtr < 0)
    {
        throw i_op::error_msg{ errno, "i_op::named_mutex<bool>::named_mutex(char const*)", "shm_open(char const*, int, mode_t)" };
    }
    this->__name = _name;

    struct stat st;
    if (fstat(this->__mux_mem_fl_dtr, &st) != 0)
    {
        tmp = errno;
        shm_unlink(this->__name);
        ::close(this->__mux_mem_fl_dtr);
        throw i_op::error_msg{ tmp, "i_op::named_mutex<bool>::named_mutex(char const*)", "fstat(int, struct stat*)" };
    }

    bool does_exist{ st.st_size != 0 };

    if (does_exist && _cr_new)
    {
        shm_unlink(this->__name);
        ::close(this->__mux_mem_fl_dtr);

        throw i_op::error_msg{ EEXIST, "i_op::named_mutex<true>::named_mutex(char const*)", "shm_open(char const*, int, mode_t)" };
    }
    
    if (ftruncate(this->__mux_mem_fl_dtr, 1 * sizeof(pthread_mutex_t)) != 0)
    {
        tmp = errno;
        shm_unlink(this->__name);
        ::close(this->__mux_mem_fl_dtr);
        throw i_op::error_msg{ tmp, "i_op::named_mutex<false>::named_mutex(char const*)", "ftruncate(int, size_t)" };
    }

    this->__mux_ptr = (pthread_mutex_t*)mmap(nullptr, 1, PROT_READ | PROT_WRITE, MAP_SHARED, this->__fl_dtr, 0);
    if (this->__ptr == MAP_FAILED)
    {
        tmp = errno;
        shm_unlink(this->__name);
        ::close(this->__mux_mem_fl_dtr);
        throw i_op::error_msg{ tmp, "i_op::named_mutex<bool>::named_mutex(char const*)", "mmap(void*, size_t, int, int, int, off_t)" };
    }


    pthread_mutexattr_t attr;
    (void)pthread_mutexattr_init(&attr);
    (void)pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    (void)pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    int result = pthread_mutex_init(this->__mux_ptr, &attr);
    if (result != 0)
    {
        pthread_mutexattr_destroy(&attr);
        shm_unlink(this->__name);
        ::close(this->__mux_mem_fl_dtr);
        munmap(this->__mux_ptr, 1);
        throw i_op::error_msg{ result, "i_op::named_mutex<bool>::named_mutex(char const*)", "pthread_mutex_init(pthread_mutex_t*)" };
    }

    pthread_mutexattr_destroy(&attr);
#endif
}

template<bool _cr_new>
i_op::named_mutex<_cr_new>::~named_mutex()
{
    try
    {
        if (
#ifdef OS_WIN32
            this->__mux_hdl != nullptr
#endif
#ifdef OS_LINUX
            this->__mux_ptr
#endif
            )
        {
            this->close();
        }
    }
    catch (i_op::error_msg const&) {}

    try { if (this->__name != nullptr) { this->unlink(); } }
    catch (i_op::error_msg const&) {}
}

template<bool _cr_new>
void i_op::named_mutex<_cr_new>::unlink() noexcept(false)
{
#ifdef OS_LINUX
    int e[3]{ 0 };
    if (munmap(this->__mux_ptr, 1) != 0)
    {
        e[0] = errno;
    }
    if (shm_unlink(this->__name) != 0)
    {
        e[1] = errno;
    }
    if (::close(this->__fl_dtr) != 0)
    {
        e[2] = errno;
    }
    this->__fl_dtr = -1;
#endif
    this->__name = nullptr;
#ifdef OS_LINUX
    const char* m = "i_op::named_mutex<bool>::unlink()";
    if (e[0] != 0)
    {
        throw i_op::error_msg{ e[0], m, "munmap(void*, size_t)" };
    }
    if (e[1] != 0)
    {
        throw i_op::error_msg{ e[1], m, "shm_unlink(char const*)" };
    }
    if (e[2] != 0)
    {
        throw i_op::error_msg{ e[2], m, "close(int)" };
    }
#endif
}
