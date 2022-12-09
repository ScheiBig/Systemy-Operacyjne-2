#pragma once

#include <pthread.h>

#ifndef generic_function
#define generic_function

typedef void* (*generic_function_t)(void*);
typedef void* generic_pointer_t;

#endif

namespace p_th
{
    namespace create
    {
        /* Detach state.  */
        enum type
        {
            JOINABLE,
            DETACHED
        }; // enum type
    } // namespace create

    namespace mutex
    {
        /* Mutex types.  */
        enum type
        {
            TIMED_NP,
            RECURSIVE_NP,
            ERRORCHECK_NP,
            ADAPTIVE_NP

#if defined __USE_UNIX98 || defined __USE_XOPEN2K8
            ,
            NORMAL = TIMED_NP,
            RECURSIVE = RECURSIVE_NP,
            ERRORCHECK = ERRORCHECK_NP,
            DEFAULT = NORMAL
#endif

// #ifdef __USE_GNU
//             /* For compatibility.  */
//             , FAST_NP = TIMED_NP
// #endif
        }; // enum type

#ifdef __USE_XOPEN2K
        /* Robust mutex or not flags.  */
        enum robustness
        {
            STALLED,
            ROBUST,
        }; // enum robustness
#endif
    } // namespace mutex

#if defined __USE_POSIX199506 || defined __USE_UNIX98
    namespace prio
    {
        /* Mutex protocols.  */
        enum type
        {
            NONE,
            INHERIT,
            PROTECT
        }; // enum type
    } // namespace prio
#endif

#if defined __USE_UNIX98 || defined __USE_XOPEN2K
    namespace rw_lock
    {
        /* Read-write lock types.  */
        enum type
        {
            PREFER_READER_NP,
            PREFER_WRITER_NP,
            PREFER_WRITER_NONRECURSIVE_NP,
            DEFAULT_NP = PTHREAD_RWLOCK_PREFER_READER_NP
        }; // enum type
    } // namespace rw_lock
#endif
    namespace scheduler
    {
        /* Scheduler inheritance.  */
        enum inheritance
        {
            INHERIT,
            EXPLICIT
        }; // namespace inheritance
    } // namespace scheduler

    namespace scope
    {
        /* Scope handling.  */
        enum type
        {
            SYSTEM,
            PROCESS
        }; // enum type
    } // enum scope


    namespace process
    {
        /* Process shared or private flag.  */
        enum type
        {
            PRIVATE,
            SHARED

        }; // namespace type
    } // namespace process

    namespace cancel
    {
        /* Cancellation */
        enum flag
        {
            ENABLE,
            DISABLE

        }; // namespace flag
        enum type
        {
            DEFERRED,
            ASYNCHRONOUS

        }; // namespace type
    } // namespace cancel

    extern const void* cancelled;
} // namespace p_th

class thread_p
{
public:
    class attr
    {
    public:
        /**
         *@brief Retrieves raw `pthread_attr_t` attributes
         */
        const pthread_attr_t* operator ()();

        /**
         *@brief Retrieves raw `pthread_attr_t` attributes
         */
        const pthread_attr_t* operator ()() const;

        /**
         *@brief Creates default configuration
         */
        static const attr none();

        /**
         *@brief Returns `true` if this consfiguration object should represent the default configuration
         */
        bool is_empty() const;
    private:
        attr(pthread_attr_t _attributes);
        attr();
        pthread_attr_t attributes;
        bool empty;
    };

    /**
     * @brief Create a new thread
     *
     * @param attributes Configuration object
     * @param start_routine Execution entry point of a new thread
     * @param routine_arguments Arguments passed to the `start_routine`
     */
    thread_p(const attr& attributes, generic_function_t start_routine, generic_pointer_t routine_arguments);

    /**
     * @brief Create a new thread with default attributes
     *
     * @param start_routine Execution entry point of a new thread
     * @param routine_arguments Arguments passed to the `start_routine`
     */
    thread_p(generic_function_t start_routine, generic_pointer_t routine_arguments);

    /**
     *@brief Terminate the calling thread
     * 
     * @param return_value Arguments that should be exposed to a thread_p::join call
     */
    static void exit(generic_pointer_t return_value);

    /**
     *@brief Make calling thread wait for tremination of this thread.
     */
    void join();

    /**
     *@brief Make calling thread wait for tremination of this thread.
     * 
     * @return Pointer to arguments exposed in a thread_p::exit call.
     */
    generic_pointer_t join(generic_pointer_t* value_for_return);


    void detach();
private:
    pthread_t identifier;

    bool is_detached;
};
