#pragma once

#include <chrono>
#include <iostream>
#include <climits>
#include <cmath>
#include <vector>

#include "interoperability/thread.hpp"
#include "interoperability/process.hpp"
#include "util/memory.hpp"

namespace sieving_strategy
{
    typedef long long llong;
    
    struct measure
    {
        /// @brief ms of initialization phase
        llong ms_i;
        /// @brief ns of initialization phase
        llong ns_i;

        /// @brief ms of single-threaded (pre)calculation phase
        llong ms_sc;
        /// @brief ns of single-threaded (pre)calculation phase
        llong ns_sc;

        /// @brief ms of multi-threaded calculation phase
        llong ms_mc;
        /// @brief ns of multi-threaded calculation phase
        llong ns_mc;
    };
    
    namespace num
    {
        constexpr char Root = '@';
        constexpr char Prime = '#';
        constexpr char Composite = '`';
    }



    /**
     * @brief Performs calculation on `_n` numbers, saving results to `_sieve`.
     *
     * This functions runs single-threaded, and so there's nothing to adjust.
     */
    measure single_thread(llong _n, util::mem<char>& _sieve)
    {
        /* =============================================================================================== */
        /* Initialization phase - assumes every number is prime (until later it isn't) and marks 1 as root */
        /* =============================================================================================== */
        auto start_i = std::chrono::high_resolution_clock::now();

        for (auto& i : _sieve)
        {
            i = num::Prime;
        }
        if (1 <= _n) { _sieve.ptr()[1] = num::Root; }

        auto end_i = std::chrono::high_resolution_clock::now();


        /* ================================================================================================== */
        /* Calculation phase - performs simple sieving, that skips marking composite numbers. Marking is done */
        /* up to sqrt(n) - no need to go above that.                                                          */
        /* ================================================================================================== */
        auto start_c = std::chrono::high_resolution_clock::now();

        for (llong i = 2; i * i <= _n; i++)
        {
            if (_sieve.ptr()[i] == num::Prime)
            {
                for (llong j = i * i; j <= _n + 1; j += i)
                {
                    _sieve.ptr()[j] = num::Composite;
                }
            }
        }

        auto end_c = std::chrono::high_resolution_clock::now();


        /* ================================== */
        /* Calculate and return time measures */
        /* ================================== */
        return measure{
            (llong)std::chrono::duration_cast<std::chrono::milliseconds>(end_i - start_i).count(),
            (llong)std::chrono::duration_cast<std::chrono::nanoseconds>(end_i - start_i).count(),
            (llong)std::chrono::duration_cast<std::chrono::milliseconds>(end_c - start_c).count(),
            (llong)std::chrono::duration_cast<std::chrono::nanoseconds>(end_c - start_c).count(),
            0,
            0
        };
    }



    /**
     * @brief Wrapper of arguments passed to calculation threads
     */
    struct slice
    {
        /// @brief Lower, inclusive bound of slice
        llong begin;
        /// @brief Upper, inclusive bound of slice
        llong end;
        /// @brief Pointer to results
        util::mem<char>* sieve;
        /// @brief Sqrt(n) of numbers in `sieve`
        llong sqr;
    };

    /**
     * @brief Runnable thread function, that performs sieving on given slice
     *
     * @param _slice `sieving_strategy::slice*` casted to `void*`
     * @return `i_op::thread::OS_Runnable_OK`
     */
    auto thread_calculation(void* _slice)
    {
        slice slice = *((struct slice*)_slice);

        for (llong i = 2; i <= slice.sqr; i++)
        {
            if (slice.sieve->ptr()[i] == num::Prime)
            {
                /// smallest multiple of `i` in this slice
                llong min_factor = ((slice.begin / i) + 1) * i;

                for (llong j = min_factor; j <= slice.end; j += i)
                {
                    slice.sieve->ptr()[j] = num::Composite;
                }
            }
        }

        return i_op::thread::OS_Runnable_OK;
    }

    /**
     * @brief Performs calculation on `_n` numbers, saving results to `_sieve`.
     *
     * This functions runs single-threaded for precalculation phase, which is in range 0..sqrt(_n).
     * Rest of range is divided between threads, which calculate their respective slices concurrently.
     * Number of spawned threads is equal `_th`, but not more that numbers of CPU cores (_th == -1 is ignored).
     *
     * As slices have minimal, single or few numbers overlap (to counter rounding errors), no synchronization
     * is used in there - as introducing one would overshadow any time profits from concurrency. 
     */
    measure multi_thread(llong _n, util::mem<char>& _sieve, int _th = -1)
    {
        /* =============================================================================================== */
        /* Initialization phase - assumes every number is prime (until later it isn't) and marks 1 as root */
        /* =============================================================================================== */
        auto start_i = std::chrono::high_resolution_clock::now();

        for (auto& i : _sieve)
        {
            i = num::Prime;
        }
        if (1 <= _n) { _sieve.ptr()[1] = num::Root; }

        auto end_i = std::chrono::high_resolution_clock::now();


        /* ===================================================================================================== */
        /* Precalculation phase - performs simple sieving, that skips marking composite numbers. Marking is done */
        /* up to sqrt(n) on slice that is 10 numbers larger - to counter rounding errors on beginning of         */
        /* concurrent calculation phase                                                                          */
        /* ===================================================================================================== */
        auto start_sc = std::chrono::high_resolution_clock::now();

        llong sqr = std::sqrt(_n);

        for (llong i = 2; i <= sqr; i++)
        {
            if (_sieve.ptr()[i] == num::Prime)
            {
                for (llong j = i * i; j <= sqr + 10; j += i)
                {
                    _sieve.ptr()[j] = num::Composite;
                }
            }
        }

        auto end_sc = std::chrono::high_resolution_clock::now();


        /* ==================================================================== */
        /* Concurrent calculation phase - divides rest of range between threads */
        /* ==================================================================== */
        auto start_mc = std::chrono::high_resolution_clock::now();

        int thread_cnt = _th == -1 ?
            i_op::process::get_CPU_count() :
            std::min(_th, (signed)i_op::process::get_CPU_count());


        /* ======================================================================== */
        /* Get size of rest of range - adjusting for at least one number per thread */
        /* ======================================================================== */
        llong rest = _n - sqr;
        if (rest <= thread_cnt)
        {
            thread_cnt = 1;
        }


        /* ========================================================================= */
        /* Calculate size of slices - last one will take any remainder from division */
        /* ========================================================================= */
        llong slice_size = rest / thread_cnt;
        llong last_slice = slice_size * thread_cnt == rest ? slice_size : slice_size + rest % slice_size;


        /* ===================================================================================== */
        /* Allocate threads and their arguments - vector for threads, since no empty constructor */
        /* for them exist. Arguments are better suited in allocated memory, since their address  */
        /* shouldn't change for lifetime of threads                                              */
        /* ===================================================================================== */
        std::vector<i_op::thread> threads;
        auto args = util::mem<slice>::calloc(thread_cnt);

        for (int i = 1; i < thread_cnt; i++)
        {

            /* ============================================================================================= */
            /* Slices begin after sqrt(n) (without overlap), and overlap each other by 1 number, to counter  */
            /* any rounding errors in sieving                                                                */
            /* ============================================================================================= */
            args.ptr()[i - 1] = slice{
                sqr + 1 + (i - 1) * slice_size,
                sqr + 1 + i * slice_size,
                &_sieve,
                sqr
            };

            threads.push_back(i_op::thread{ thread_calculation, &args.ptr()[i - 1] }.start());
        }

        /* ========================================================================== */
        /* Last range uses own length, to cover last slice until last number in range */
        /* ========================================================================== */
        args.ptr()[thread_cnt - 1] = slice{
            sqr + 1 + (thread_cnt - 1) * slice_size,
            sqr + 2 + (thread_cnt - 1) * slice_size + last_slice,
            &_sieve,
            sqr
        };

        threads.push_back(i_op::thread{ thread_calculation, &args.ptr()[thread_cnt - 1] }.start());


        /* ======================================================================================== */
        /* Join all threads - since their work should take roughly the same amount of time and all  */
        /* need to finish, there's no need for more sophisticated joining "algorithm". Also free    */
        /* memory used by threads after they finish                                                 */
        /* ======================================================================================== */
        for (auto th : threads)
        {
            th.join();
        }
        args.free();

        auto end_mc = std::chrono::high_resolution_clock::now();

        /* ================================== */
        /* Calculate and return time measures */
        /* ================================== */
        return measure{
            (llong)std::chrono::duration_cast<std::chrono::milliseconds>(end_i - start_i).count(),
            (llong)std::chrono::duration_cast<std::chrono::nanoseconds>(end_i - start_i).count(),
            (llong)std::chrono::duration_cast<std::chrono::milliseconds>(end_sc - start_sc).count(),
            (llong)std::chrono::duration_cast<std::chrono::nanoseconds>(end_sc - start_sc).count(),
            (llong)std::chrono::duration_cast<std::chrono::milliseconds>(end_mc - start_mc).count(),
            (llong)std::chrono::duration_cast<std::chrono::nanoseconds>(end_mc - start_mc).count(),
        };
    }
}
