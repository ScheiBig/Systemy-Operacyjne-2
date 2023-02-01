#pragma once

#include <chrono>
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
}
