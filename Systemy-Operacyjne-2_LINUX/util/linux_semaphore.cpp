#include "linux_semaphore.hpp"

#include <semaphore.h>
#include <system_error>
#include <fcntl.h>

void util::basic_semaphore::wait() noexcept(false)
{
    int result = sem_wait(this->__sem);
    if (result != 0)
    {
        throw std::system_error(std::make_error_code((std::errc)errno), "sem_wait");
    }
}

void util::basic_semaphore::timed_wait(const timespec& _abstime) noexcept(false)
{
    int result = sem_timedwait(this->__sem, &_abstime);
    if (result != 0)
    {
        throw std::system_error(std::make_error_code((std::errc)errno), "sem_timedwait");
    }
}

void util::basic_semaphore::try_wait() noexcept(false)
{
    int result = sem_trywait(this->__sem);
    if (result != 0)
    {
        throw std::system_error(std::make_error_code((std::errc)errno), "sem_trywait");
    }
}

void util::basic_semaphore::post() noexcept(false)
{
    int result = sem_post(this->__sem);
    if (result != 0)
    {
        throw std::system_error(std::make_error_code((std::errc)errno), "sem_post");
    }
}

int util::basic_semaphore::get_value() noexcept(false)
{
    int value;
    int result = sem_getvalue(this->__sem, &value);
    if (result != 0)
    {
        throw std::system_error(std::make_error_code((std::errc)errno), "sem_getvalue");
    }
    return value;
}

util::basic_semaphore::~basic_semaphore() noexcept(false) {}




util::semaphore::semaphore(bool _shared, unsigned int _value) noexcept(false)
{
    int result = sem_init(&this->__sem_v, (int)_shared, _value);
    if (result != 0)
    {
        throw std::system_error(std::make_error_code((std::errc)errno), "sem_init");
    }
    this->__sem = &this->__sem_v;
}

util::semaphore::~semaphore() noexcept(false)
{
    int result = sem_destroy(this->__sem);
    if (result != 0)
    {
        throw std::system_error(std::make_error_code((std::errc)errno), "sem_destroy");
    }
}




util::named_semaphore::named_semaphore(const char* _name, mode_t _mode, unsigned int _value, bool create_new) noexcept(false)
{
    int m = O_CREAT;
    if (create_new) { m |= O_EXCL; }
    sem_t* result = sem_open(_name, m, _mode, _value);
    if (result == SEM_FAILED)
    {
        throw std::system_error(std::make_error_code((std::errc)errno), "sem_open");
    }
    this->__sem = result;
    this->__name = _name;
}

void util::named_semaphore::unlink() noexcept(false)
{
    int result = sem_unlink(this->__name);
    if (result != 0)
    {
        throw std::system_error(std::make_error_code((std::errc)errno), "sem_unlink");
    }
}

util::named_semaphore::~named_semaphore() noexcept(false)
{
    int result = sem_close(this->__sem);
    if (result != 0)
    {
        throw std::system_error(std::make_error_code((std::errc)errno), "sem_close");
    }
}
