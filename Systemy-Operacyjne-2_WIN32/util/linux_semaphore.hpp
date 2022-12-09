/* ========================================================================== */
/* Author: Marcin Jeznach || plz no steal 😭                                  */
/*                                                                            */
/* Wrapper for Linux's semaphore,                                             */
/* consult man sem <name of function without `_`> for details of idividual    */
/* functions                                                                  */
/* ========================================================================== */
#pragma once

#include <semaphore.h>

namespace util
{
    class basic_semaphore
    {
    public:
        basic_semaphore(): __sem() {}
        basic_semaphore(const basic_semaphore& _o): __sem(_o.__sem) {}
        virtual ~basic_semaphore() noexcept(false);
        
        void wait() noexcept(false);
        void timed_wait(const timespec& _abstime) noexcept(false);
        void try_wait() noexcept(false);

        void post() noexcept(false);
        int get_value() noexcept(false);
    protected:
        sem_t* __sem;
    };

    class semaphore final : public basic_semaphore
    {
    private:
        sem_t __sem_v;
        semaphore(const semaphore&);
    public:
        semaphore(bool _shared, unsigned int _value) noexcept(false);
        ~semaphore() noexcept(false) override;
    };

    class named_semaphore final : public basic_semaphore
    {
    private:
        named_semaphore(const named_semaphore&);
        const char* __name;
    public:
        named_semaphore(const char* _name, mode_t _mode, unsigned int _value, bool create_new = true) noexcept(false);
        void unlink() noexcept(false);
        ~named_semaphore() noexcept(false) override;
    };
}
