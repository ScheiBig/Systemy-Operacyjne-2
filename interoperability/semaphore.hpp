/* ========================================================================== */
/* Author: Marcin Jeznach || plz no steal 😭                                  */
/*                                                                            */
/* Semaphore primitive with OS-independent interface                          */
/* ========================================================================== */
#pragma once
#ifdef OS_WIN32
#include <windows.h>
#endif
#ifdef OS_LINUX
#include <semaphore.h>
#endif

#include "./timeunit.hpp"

namespace i_op
{
    /**
     * @brief Common interface for semaphores
     */
    class basic_semaphore
    {
    protected:
#ifdef OS_WIN32
        HANDLE __sem_hdl{ nullptr };
#endif
#ifdef OS_LINUX
        sem_t* __sem_ptr{ nullptr };
#endif
        inline basic_semaphore();
        basic_semaphore(basic_semaphore const&) = delete;
    public:
        /**
         * @brief Decrements the semaphore, waiting indefinitely if the current value is 0
         * @throw i_op::error_msg contains a OS-specific error code
         */
        inline void acquire() noexcept(false);
        /**
         * @brief Decrements the semaphore, waiting no longer(ish) than `_duration` if the current value is 0.
         * Timed functions do not guarantee accuracy higher that miliseconds!
         * @param _duration Maximal(ish) waiting time. If duration points past /03:14:08 UTC, 2038.01.19/, behaviour may be undefined
         * @throw i_op::error_msg contains a OS-specific error code
         */
        inline void acquire(i_op::time_unit const& _duration) noexcept(false);
        /**
         * @brief Decrements the semaphore, exiting immediately if the current value is 0
         * @return true if the decrement was successful
         * @throw i_op::error_msg – contains a OS-specific error code
         */
        inline bool try_acquire() noexcept(false);
        /**
         * @brief Decrements the semaphore, exiting no longer(ish) than after `_duration` if the current value is 0.
         * Timed functions do not guarantee accuracy higher that miliseconds!
         * @return true if the decrement was successful
         * @throw i_op::error_msg – contains a OS-specific error code
         */
        inline bool try_acquire(i_op::time_unit const& _duration) noexcept(false);

        /**
         * @brief Increments the semaphore
         * @throw i_op::error_msg – contains OS-specific error code
         */
        inline void release() noexcept(false);

        /**
         * @brief Performs destruction of object, throwing on failure
         * @throw i_op::error_msg – contains OS-specific error code
         */
        inline void close() noexcept(false);
    };

    /**
     * @brief Unnamed semaphore
     */
    class semaphore final: public basic_semaphore
    {
    private:
#ifdef OS_LINUX
        sem_t __sem_v;
#endif
        semaphore(semaphore const&) = delete;
    public:
        /**
         * @brief Construct a new semaphore object
         * @param _value initial value of the semaphore
         */
        inline semaphore(unsigned int _value) noexcept(false);

        /**
         * @brief Performs destruction of object, ignoring exceptions on failure
         */
        inline ~semaphore();
    };

    /**
     * @brief Named semaphore
     * @tparam _cr_new if false, object will open the semaphore if one with specified name already exist,
     *         if true, will fail
     */
    template<bool _cr_new = false>
    class named_semaphore final: public basic_semaphore
    {
    private:
        char const* __name;
        named_semaphore(named_semaphore const&) = delete;
    public:
        /**
         * @brief Construct a new semaphore object
         * @param _name name of the semaphore
         * @param _value initial value of the semaphore
         */
        inline named_semaphore(char const* _name, unsigned int _value) noexcept(false);

        /**
         * @brief Performs destruction of object, ignoring exceptions on failure
         */
        inline ~named_semaphore();

        /**
         * @brief Performs destruction of shared object (specific to named semaphores), throwing on failure.
         * This call is not called in destructor, as it could destroy semaphore that other programs are using,
         * effectively resetting its value in next run. This call is not necessarily required, but it can leave
         * objects behind in OS.
         * @throw i_op::error_msg – contains OS-specific error code
         */
        inline void unlink() noexcept(false);
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
#include <fcntl.h>
#include <time.h>
#endif

inline i_op::basic_semaphore::basic_semaphore() {}

inline void i_op::basic_semaphore::acquire() noexcept(false)
{
#ifdef OS_WIN32
    if (WaitForSingleObject(this->__sem_hdl, INFINITE) != WAIT_OBJECT_0)
    {
        throw i_op::error_msg{ GetLastError(), "i_op::basic_semaphore::acquire()", "WaitForSingleObject(HANDLE, DWORD)" };
    }
#endif
#ifdef OS_LINUX
    if (sem_wait(this->__sem_ptr) != 0)
    {
        throw i_op::error_msg{ errno, "i_op::basic_semaphore::acquire()", "sem_wait(sem_t*)" };
    }
#endif
}

inline void i_op::basic_semaphore::acquire(i_op::time_unit const& _duration) noexcept(false)
{
#ifdef OS_WIN32
    DWORD wait_time{ (DWORD)((((_duration.h * 60) + _duration.min) * 60 + _duration.s * 1000) + _duration.ms) };
    if (WaitForSingleObject(this->__sem_hdl, wait_time) != WAIT_OBJECT_0)
    {
        throw i_op::error_msg{ GetLastError(), "i_op::basic_semaphore::acquire(i_op::time_unit const&)", "WaitForSingleObject(HANDLE, DWORD)" };
    }
#endif
#ifdef OS_LINUX
    timespec dur;
    dur.tv_sec = ((_duration.h * 60) + _duration.min) * 60 + _duration.s + time(nullptr);
    dur.tv_nsec = ((_duration.ms * 1000) + _duration.us) * 1000 + _duration.ns;
    if (sem_timedwait(this->__sem_ptr, &dur) != 0)
    {
        throw i_op::error_msg{ errno, "i_op::basic_semaphore::acquire(i_op::time_unit const&)", "sem_timedwait(sem_t*, struct timespec const*)" };
    }
#endif
}

inline bool i_op::basic_semaphore::try_acquire() noexcept(false)
{
#ifdef OS_WIN32
    switch (WaitForSingleObject(this->__sem_hdl, 0))
    {
    case WAIT_OBJECT_0: return true;
    case WAIT_TIMEOUT: return false;
    default: throw i_op::error_msg{ GetLastError(), "i_op::basic_semaphore::try_acquire()", "WaitForSingleObject(HANDLE, DWORD)" };
    }
#endif
#ifdef OS_LINUX
    if (sem_trywait(this->__sem_ptr) != 0)
    {
        int er{ errno };
        if (er == EAGAIN) { return false; }
        else
        {
            throw i_op::error_msg{ errno, "i_op::basic_semaphore::try_acquire()", "sem_trywait(sem_t*)" };
        }
    }
    return true;
#endif
}

inline bool i_op::basic_semaphore::try_acquire(i_op::time_unit const& _duration) noexcept(false)
{
#ifdef OS_WIN32
    DWORD wait_time{ (DWORD)((((_duration.h * 60) + _duration.min) * 60 + _duration.s * 1000) + _duration.ms) };
    switch (WaitForSingleObject(this->__sem_hdl, wait_time))
    {
    case WAIT_OBJECT_0: return true;
    case WAIT_TIMEOUT: return false;
    default: throw i_op::error_msg{ GetLastError(), "i_op::basic_semaphore::try_acquire(i_op::time_unit const&)", "WaitForSingleObject(HANDLE, DWORD)" };
    }
#endif
#ifdef OS_LINUX
    timespec dur;
    dur.tv_sec = ((_duration.h * 60) + _duration.min) * 60 + _duration.s + time(nullptr);
    dur.tv_nsec = ((_duration.ms * 1000) + _duration.us) * 1000 + _duration.ns;
    if (sem_timedwait(this->__sem_ptr, &dur) != 0)
    {
        int er{ errno };
        if (er == ETIMEDOUT) { return false; }
        else
        {
            throw i_op::error_msg{ errno, "i_op::basic_semaphore::try_acquire(i_op::time_unit const&)", "sem_timedwait(sem_t*, struct timespec const*)" };
        }
    }
    return true;
#endif
}

inline void i_op::basic_semaphore::release() noexcept(false)
{
#ifdef OS_WIN32
    if (!!ReleaseSemaphore(this->__sem_hdl, 1, nullptr) == false)
    {
        throw i_op::error_msg{ GetLastError(), "i_op::basic_semaphore::release()", "ReleaseSemaphore(HANDLE, LONG, LPLONG)" };
    }
#endif
#ifdef OS_LINUX
    if (sem_post(this->__sem_ptr) != 0)
    {
        throw i_op::error_msg{ errno, "i_op::basic_semaphore::release()", "sem_post(sem_t*)" };
    }
#endif
}

inline void i_op::basic_semaphore::close() noexcept(false)
{
#ifdef OS_WIN32
    if (!!CloseHandle(this->__sem_hdl) == false)
    {
        throw i_op::error_msg{ GetLastError(), "i_op::basic_semaphore::close()", "CloseHandle(HANDLE)" };
    }
    this->__sem_hdl = nullptr;
#endif
#ifdef OS_LINUX
    if (sem_close(this->__sem_ptr) != 0)
    {
        throw i_op::error_msg{ errno, "i_op::basic_semaphore::close()", "sem_close(sem_t*)" };
    }
    this->__sem_ptr = nullptr;
#endif
}



inline i_op::semaphore::semaphore(unsigned int _value) noexcept(false)
{
#ifdef OS_WIN32
    this->__sem_hdl = CreateSemaphoreA(nullptr, _value, _value, nullptr);
    if (this->__sem_hdl == nullptr)
    {
        throw i_op::error_msg{ GetLastError(), "i_op::semaphore::semaphore(unsigned int)", "CreateSemaphoreA(LPSECURITY_ATTRIBUTES, LONG, LONG, LPCSTR)" };
    }
#endif
#ifdef OS_LINUX
    if (sem_init(&this->__sem_v, (int)true, _value) != 0)
    {
        throw i_op::error_msg{ errno, "i_op::semaphore::semaphore(unsigned int)", "sem_init(sem_t*, int, unsigned int)" };
    }
    this->__sem_ptr = &this->__sem_v;
#endif
}

inline i_op::semaphore::~semaphore()
{
    try
    {
        if (
#ifdef OS_WIN32
            this->__sem_hdl != nullptr &&
#endif
#ifdef OS_LINUX
            this->__sem_ptr != nullptr &&
#endif
            true)
        {
            this->close();
        }
    }
    catch (i_op::error_msg const&) {}
}



template<bool _cr_new>
inline i_op::named_semaphore<_cr_new>::named_semaphore(char const* _name, unsigned int _value) noexcept(false)
{
#ifdef OS_WIN32
    this->__sem_hdl = CreateSemaphoreA(nullptr, _value, _value, _name);
    DWORD err = GetLastError();
    if (this->__sem_hdl == nullptr)
    {
        throw i_op::error_msg{ err, "i_op::named_semaphore<bool>::named_semaphore(char const*, unsigned int)", "CreateSemaphoreA(LPSECURITY_ATTRIBUTES, LONG, LONG, LPCSTR)" };
    }
    if (_cr_new && err == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(this->__sem_hdl);
        throw i_op::error_msg{ err, "i_op::named_semaphore<true>::named_semaphore(char const*, unsigned int)", "CreateSemaphoreA(LPSECURITY_ATTRIBUTES, LONG, LONG, LPCSTR)" };
    }
#endif
#ifdef OS_LINUX
    int m{ O_CREAT };
    if (_cr_new) { m |= O_EXCL; }
    sem_t* result{ sem_open(_name, m, 0664, _value) };
    if (result == SEM_FAILED)
    {
        throw i_op::error_msg{ errno, "i_op::named_semaphore<bool>::named_semaphore(char const*, unsigned int)", "sem_open(const char*, int, mode_t, unsigned int)" };
    }
    this->__sem_ptr = result;
    this->__name = _name;
#endif
}

template<bool _cr_new>
inline i_op::named_semaphore<_cr_new>::~named_semaphore()
{
    try
    {
        if (
#ifdef OS_WIN32
            this->__sem_hdl != nullptr &&
#endif
#ifdef OS_LINUX
            this->__sem_ptr != nullptr &&
#endif
            true)
        {
            this->close();
        }
    }
    catch (i_op::error_msg const&) {}
}

template<bool _cr_new>
inline void i_op::named_semaphore<_cr_new>::unlink() noexcept(false)
{
#ifdef OS_LINUX
    if (sem_unlink(this->__name) != 0)
    {
        throw i_op::error_msg{ errno, "i_op::named_semaphore<bool>::unlink()", "sem_unlink(const char*)" };
    }
#endif
    this->__name = nullptr;
}
