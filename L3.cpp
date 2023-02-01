#include "interoperability/macro.hpp"

#include <iostream>
#include <chrono>
#include <string>

#include "util/memory.hpp"
#include "util/ansi_text.hpp"
#include "sieving_strategies.hpp"
#include "ulam.hpp"

using namespace std::string_literals;

enum struct output
{
    None, Text, Image
};


/* ========================================================================================================= */
/* This program again uses i_op API and stdlib, so it should work under both Win32 and Linux. Because of how */
/* WSL handles multiple threads (poorly, seems like whole VVM is single-processed), it is highly recommended */
/* to launch this under native host.                                                                         */
/*                                                                                                           */
/* This program is highly modified compared to requirements - it can output as file or BMP image. It can     */
/* also produce no output - which combined with backdoor for every parameter of program means, that outside  */
/* script can launch this program with different parameters, and plot graphs that present how calculation    */
/* time might be dependant on different values                                                               */
/* ========================================================================================================= */
int main(int argc, char const* argv[])
{
    /* ============================================================= */
    /* Check if required parameter is present - print help otherwise */
    /* ============================================================= */
    if (argc < 2)
    {
        std::cerr
            << ANSI::b_red << "Two few args."
            << ANSI::b_yellow << " Usage: " << argv[0] << " [n] <type = 1> <max threads = -1> <precalc per mile = 1000>"
            << ANSI::reset << "\nWhere: \n"
            << ANSI::b_green << "               n "
            << ANSI::reset << "- upper bound of searched range\n"
            << ANSI::b_green << "            type "
            << ANSI::reset << "- 0 for silent output (only time measures), 1 for text output, 2 for image output"
            << " (careful — for n = 2e8 image output is approximately 0.5 GB! Output for n above is not allowed)\n"
            << ANSI::b_green << "     max threads "
            << ANSI::reset << "- max limit of threads that will be spawned or -1 for equal to CPU count\n";
        return 1;
    }
    
    /* ================================================================================================ */
    /* Parse required parameter - positive long integer that represents upper bound of calculated range */
    /* ================================================================================================ */
    long long n = std::atoll(argv[1]);
    if (n <= 0)
    {
        std::cerr
            << ANSI::b_red << "Negative [n] not allowed"
            << ANSI::reset << "\n";
        return 2;
    }

    /* ====================================== */
    /* Parse optional parameter - output mode */
    /* ====================================== */
    int mode = 1;
    if (argc >= 3)
    {
        mode = std::atoi(argv[2]);
        if (0 > mode || mode > 2)
        {
            std::cerr
                << ANSI::b_red << "<type>s allowed: 0 - no output, 1 - text output, 2 - image output"
                << ANSI::reset << "\n";
            return 3;
        }
        if (n > (long long)2e8 && mode != 0)
        {

            std::cerr
                << ANSI::b_red << "Outputting ranges above 2e8 is not allowed for stability reasons"
                << ANSI::reset << "\n";
            return 4;
        }
    }
    
    /* ============================================= */
    /* Parse optional parameter - thread count limit */
    /* ============================================= */
    int th = -1;
    if (argc >= 4)
    {
        th = std::atoi(argv[3]);
        if (th < 1 && th != -1)
        {
            std::cerr
                << ANSI::b_red << "<max threads> must be positive or `-1' for limited by hardware"
                << ANSI::reset << "\n";
            return 5;
        }
    }
    if (getenv("PRIME_SINGLE_THREAD") != nullptr && "1"s == getenv("PRIME_SINGLE_THREAD"))
    {
        th = 1;
        std::cout
            << ANSI::b_blue << "Override of <max threads> via environment variable. "
            << ANSI::reset << "New value := 1" << "\n";
    }


    auto sieve = util::mem<char>::calloc(n + 1);
    if (sieve.ptr() == nullptr)
    {
        std::cerr
            << ANSI::b_red << "[n] value too big - cannot allocate memory for sieve"
            << ANSI::reset << "\n";
        return 5;
    }

    /* ===================================================================================== */
    /* Sieve numbers, print time measures - in no output mode show total time in nanoseconds */
    /* ===================================================================================== */
    if (th == 1)
    {
        if (mode == (int)output::None)
        {
            auto stopwatch = sieving_strategy::single_thread(n, sieve);
            long long total_ns = stopwatch.ns_i + stopwatch.ns_sc;
            std::cout << "  " << total_ns << "  \n";
        }
        else
        {
            auto stopwatch = sieving_strategy::single_thread(n, sieve);
            std::cout << "Init time:        " << stopwatch.ms_i << "ms " << stopwatch.ns_i % 1'000'000 << "ns\n";
            std::cout << "Calculation time: " << stopwatch.ms_sc << "ms " << stopwatch.ns_sc % 1'000'000 << "ns\n";
            std::cout << "\n";
        }
    }
    else
    {
        if (mode == (int)output::None)
        {
            auto stopwatch = sieving_strategy::multi_thread(n, sieve, th);
            long long total_ns = stopwatch.ns_i + stopwatch.ns_sc + stopwatch.ns_mc;
            std::cout << "  " << total_ns << "  \n";
        }
        else
        {
            auto stopwatch = sieving_strategy::multi_thread(n, sieve, th);
            std::cout << "Init time:        " << stopwatch.ms_i << "ms " << stopwatch.ns_i % 1'000'000 << "ns\n";
            std::cout << "Precalc time:     " << stopwatch.ms_sc << "ms " << stopwatch.ns_sc % 1'000'000 << "ns\n";
            std::cout << "Calculation time: " << stopwatch.ms_mc << "ms " << stopwatch.ns_mc % 1'000'000 << "ns\n";
            std::cout << "\n";
        }
    }

    
    /* =============================== */
    /* Output Ulam Spiral, if selected */
    /* =============================== */
    if (mode == (int)output::Text)
    {
        ulam::print("spiral.txt", sieve);
    }
    else if (mode == (int)output::Image)
    {
        ulam::picture("spiral.bmp", sieve);
    }

    sieve.free();

    return 0;
}
