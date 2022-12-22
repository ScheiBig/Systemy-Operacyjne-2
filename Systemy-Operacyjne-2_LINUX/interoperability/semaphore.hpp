/* ========================================================================== */
/* Author: Marcin Jeznach || plz no steal 😭                                  */
/*                                                                            */
/* Semaphore primitive with OS-independent interface                         */
/* ========================================================================== */
#pragma once

#include "macro.hpp"

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
        basic_semaphore();
        basic_semaphore(basic_semaphore const&) = delete;
    public:
        /**
         * @brief Decrements the semaphore, waiting indefinitely if the current value is 0
         * @throw i_op::error_msg contains a OS-specific error code
         */
        void wait() noexcept(false);
        /**
         * @brief Decrements the semaphore, waiting no longer(ish) than `_duration` if the current value is 0
         * @param _duration Maximal(ish) waiting time. If duration points past /03:14:08 UTC, 2038.01.19/, behaviour may be undefined
         * @throw i_op::error_msg contains a OS-specific error code
         */
        void wait(i_op::time_unit const& _duration) noexcept(false);
        /**
         * @brief Decrements the semaphore, exiting immediately if the current value is 0
         * @return true if the decrement was successful
         * @throw i_op::error_msg – contains a OS-specific error code
         */
        bool try_wait() noexcept(false);

        /**
         * @brief Increments the semaphore
         * @throw i_op::error_msg – contains OS-specific error code
         */
        void post() noexcept(false);

        /**
         * @brief Performs destruction of object, throwing on failure
         * @throw i_op::error_msg – contains OS-specific error code
         */
        void close() noexcept(false);
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
        semaphore(const semaphore&) = delete;
    public:
        /**
         * @brief Construct a new semaphore object
         * @param _value initial value of the semaphore
         */
        semaphore(unsigned int _value) noexcept(false);

        /**
         * @brief Performs destruction of object, ignoring exceptions on failure
         */
        ~semaphore();
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
        named_semaphore(const named_semaphore&) = delete;
    public:
        /**
         * @brief Construct a new semaphore object
         * @param _name name of the semaphore
         * @param _value initial value of the semaphore
         */
        named_semaphore(char const* _name, unsigned int _value) noexcept(false);
        
        /**
         * @brief Performs destruction of object, ignoring exceptions on failure
         */
        ~named_semaphore();
        
        /**
         * @brief Performs destruction of object (specific to named semaphores), throwing on failure
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
#include <fcntl.h>
#include <time.h>
#endif

i_op::basic_semaphore::basic_semaphore() {}

void i_op::basic_semaphore::wait() noexcept(false)
{
#ifdef OS_WIN32
    int result{ (int)WaitForSingleObject(this->__sem_hdl, INFINITE) };
    if (result != WAIT_OBJECT_0)
    {
        throw i_op::error_msg{ GetLastError() };
    }
#endif
#ifdef OS_LINUX
    if (sem_wait(this->__sem_ptr) != 0)
    {
        throw i_op::error_msg{ errno };
    }
#endif
}

void i_op::basic_semaphore::wait(i_op::time_unit const& _duration) noexcept(false)
{
#ifdef OS_WIN32
    DWORD wait_time{ (DWORD)((((_duration.h * 60) + _duration.min) * 60 + _duration.s * 1000) + _duration.ms) };
    int result{ (int)WaitForSingleObject(this->__sem_hdl, wait_time) };
    if (result != WAIT_OBJECT_0)
    {
        throw i_op::error_msg{ GetLastError() };
    }
#endif
#ifdef OS_LINUX
    timespec dur;
    dur.tv_sec = ((_duration.h * 60) + _duration.min) * 60 + _duration.s + time(nullptr);
    dur.tv_nsec = ((_duration.ms * 1000) + _duration.us) * 1000 + _duration.ns;
    if (sem_timedwait(this->__sem_ptr, &dur) != 0)
    {
        throw i_op::error_msg{ errno };
    }
#endif
}

bool i_op::basic_semaphore::try_wait() noexcept(false)
{
#ifdef OS_WIN32
    int result{ (int)WaitForSingleObject(this->__sem_hdl, 0) };
    switch (result)
    {
    case WAIT_OBJECT_0: return true;
    case WAIT_TIMEOUT: return false;
    default: throw i_op::error_msg{ GetLastError() };
    }
#endif
#ifdef OS_LINUX
    if (sem_trywait(this->__sem_ptr) != 0)
    {
        int er{ errno };
        if (er == EAGAIN)
        {
            return false;
        }
        else
        {
            throw i_op::error_msg{ errno };
        }
    }
    return true;
#endif
}

void i_op::basic_semaphore::post() noexcept(false)
{
#ifdef OS_WIN32
    if (!!ReleaseSemaphore(this->__sem_hdl, 1, nullptr) == false)
    {
        throw i_op::error_msg{ GetLastError() };
    }
#endif
#ifdef OS_LINUX
    if (sem_post(this->__sem_ptr) != 0)
    {
        throw i_op::error_msg{ errno };
    }
#endif
}

inline void i_op::basic_semaphore::close() noexcept(false)
{
#ifdef OS_WIN32
    if (!!CloseHandle(this->__sem_hdl) == false)
    {
        throw i_op::error_msg{ GetLastError() };
    }
    this->__sem_hdl = nullptr;
#endif
#ifdef OS_LINUX
    if (sem_close(this->__sem_ptr) != 0)
    {
        throw i_op::error_msg{ errno };
    }
    this->__sem_ptr = nullptr;
#endif
}



i_op::semaphore::semaphore(unsigned int _value) noexcept(false)
{
#ifdef OS_WIN32
    this->__sem_hdl = CreateSemaphoreA(nullptr, _value, _value, nullptr);
    if (this->__sem_hdl == nullptr)
    {
        throw i_op::error_msg{ GetLastError() };
    }
#endif
#ifdef OS_LINUX
    if (sem_init(&this->__sem_v, (int)true, _value) != 0)
    {
        throw i_op::error_msg{ errno };
    }
    this->__sem_ptr = &this->__sem_v;
#endif
}

i_op::semaphore::~semaphore()
{
    try
    {
        if (
#ifdef OS_WIN32
            this->__sem_hdl != nullptr
#endif
#ifdef OS_LINUX
            this->__sem_ptr != nullptr
#endif
            )
        {
            this->close();
        }
    }
    catch (i_op::error_msg const&) {}
}



template<bool _cr_new>
i_op::named_semaphore<_cr_new>::named_semaphore(char const* _name, unsigned int _value) noexcept(false)
{
#ifdef OS_WIN32
    this->__sem_hdl = CreateSemaphoreA(nullptr, _value, _value, _name);
    DWORD err = GetLastError();
    if (this->__sem_hdl == nullptr)
    {
        throw i_op::error_msg{ err };
    }
    if (_cr_new && err == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(this->__sem_hdl);
        throw i_op::error_msg{ err };
    }
#endif
#ifdef OS_LINUX
    int m{ O_CREAT };
    if (_cr_new) { m |= O_EXCL; }
    sem_t* result{ sem_open(_name, m, 0664, _value) };
    if (result == SEM_FAILED)
    {
        throw i_op::error_msg{ errno };
    }
    this->__sem_ptr = result;
    this->__name = _name;
#endif
}

template<bool _cr_new>
i_op::named_semaphore<_cr_new>::~named_semaphore()
{
    try
    {
        if (
#ifdef OS_WIN32
            this->__sem_hdl != nullptr
#endif
#ifdef OS_LINUX
            this->__sem_ptr != nullptr
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
inline void i_op::named_semaphore<_cr_new>::unlink() noexcept(false)
{
#ifdef OS_LINUX
    if (sem_unlink(this->__name) != 0)
    {
        throw i_op::error_msg{ errno };
    }
#endif
    this->__name = nullptr;
}
