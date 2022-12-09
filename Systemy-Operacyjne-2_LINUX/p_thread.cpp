#include "p_thread.hpp"

#include <pthread.h>
#include <system_error>

using std::errc;
using std::error_code;
using std::system_error;
using std::make_error_code;

const void* p_th::cancelled = PTHREAD_CANCELED;

const pthread_attr_t* thread_p::attr::operator ()() { return &this->attributes; }
const pthread_attr_t* thread_p::attr::operator ()() const { return &this->attributes; }

const thread_p::attr thread_p::attr::none()
{
    return attr{  };
}

bool thread_p::attr::is_empty() const
{
    return this->empty;
}

thread_p::attr::attr(pthread_attr_t _attributes) : attributes(_attributes), empty(false) {}

thread_p::attr::attr() : empty(true) {}


thread_p::thread_p(const attr& attributes, generic_function_t start_routine, generic_pointer_t routine_arguments)
{
    int result;
    if (attributes.is_empty())
    {
        result = pthread_create(&this->identifier, nullptr, start_routine, routine_arguments);
    }
    else
    {
        result = pthread_create(&this->identifier, attributes(), start_routine, routine_arguments);
    }

    errc eagain = errc::resource_unavailable_try_again;
    if (result == (int)eagain)
    {
        throw system_error(make_error_code(eagain),
            "The system lacked the necessary resources to create another thread, or the system - imposed limit on"
            "the total number of threads in a process { PTHREAD_THREADS_MAX } would be exceeded."
        );
    }

    errc einval = errc::invalid_argument;
    if (result == (int)einval)
    {
        throw system_error(make_error_code(einval),
            "Invalid settings in attributes."
        );
    }
    
    errc eperm = errc::operation_not_permitted;
    if (result == (int)eperm)
    {
        throw system_error(make_error_code(eperm),
            "The caller does not have appropriate privileges to set the required scheduling parameters"
            "or scheduling policy."
        );
    }

    this->is_detached = false;
}

thread_p::thread_p(generic_function_t start_routine, generic_pointer_t routine_arguments) :
    thread_p{ attr::none(), start_routine, routine_arguments }
{
}

void thread_p::exit(generic_pointer_t return_value)
{
    pthread_exit(return_value);
}

void thread_p::join()
{
    int result = pthread_join(this->identifier, nullptr);

    errc edeadlk = errc::resource_deadlock_would_occur;
    if (result == (int)edeadlk)
    {
        throw system_error(make_error_code(edeadlk),
            "A deadlock was detected."
        );
    }

    errc einval = errc::invalid_argument;
    if (result == (int)einval)
    {
        throw system_error(make_error_code(einval),
            "This thread is not a joinable thread."
            " |or| "
            "Another thread is already waiting to join with this thread."
        );
    }

    errc esrch = errc::no_such_process;
    if (result == (int)esrch)
    {
        throw system_error(make_error_code(esrch),
            "No thread with this identifier could be found."
        );
    }
}

generic_pointer_t thread_p::join(generic_pointer_t* value_for_return)
{
    int result = pthread_join(this->identifier, value_for_return);

    errc edeadlk = errc::resource_deadlock_would_occur;
    if (result == (int)edeadlk)
    {
        throw system_error(make_error_code(edeadlk),
            "A deadlock was detected."
        );
    }
    
    errc einval = errc::invalid_argument;
    if (result == (int)einval)
    {
        throw system_error(make_error_code(einval),
            "This thread is not a joinable thread."
            " |or| "
            "Another thread is already waiting to join with this thread."
        );
    }

    errc esrch = errc::no_such_process;
    if (result == (int)esrch)
    {
        throw system_error(make_error_code(esrch),
            "No thread with this identifier could be found."
        );
    }
    

    return *value_for_return;
}

void thread_p::detach()
{
    if (this->is_detached == false)
    {
        int result = pthread_detach(this->identifier);

        errc einval = errc::invalid_argument;
        if (result == (int)einval)
        {
            throw system_error(make_error_code(einval),
                "This thread is not a joinable thread."
            );
        }
        
        errc esrch = errc::no_such_process;
        if (result == (int)esrch)
        {
            throw system_error(make_error_code(esrch),
                "No thread with this identifier could be found."
            );
        }
    }
}
