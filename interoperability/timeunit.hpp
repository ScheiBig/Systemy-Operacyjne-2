/* ========================================================================== */
/* Author: Marcin Jeznach || plz no steal 😭                                  */
/*                                                                            */
/* Time unit (usually duration) primitive witch OS-independent interface      */
/* ========================================================================== */
#pragma once
#include "./macro.hpp"

namespace i_op
{
    /** @brief Structure for holding time units. Validation not included */
    struct time_unit
    {
        /** @brief Hours */
        unsigned int h;
        /** @brief Minutes */
        unsigned int min;
        /** @brief Seconds */
        unsigned int s;
        /** @brief Milliseconds */
        unsigned int ms;
        /** @brief Microseconds */
        unsigned int us;
        /** @brief Nanoseconds */
        unsigned int ns;
        

        /**
         * @brief Simple addition
         * @param _other  time_unit to add to this
         */
        inline time_unit operator+ (time_unit const& _other)        
{
            time_unit tu{
                this->h + _other.h,
                this->min + _other.min,
                this->s + _other.s,
                this->ms + _other.ms,
                this->us + _other.us,
                this->ns + _other.ns
            };
            unsigned int tmp;
            if (tu.ns >= 1000)
            {
                tmp = tu.ns;
                tu.ns %= 1000;
                tu.ms += tmp / 1000;
            }
            if (tu.us >= 1000)
            {
                tmp = tu.us;
                tu.us %= 1000;
                tu.ms += tmp / 1000;
            }
            if (tu.ms >= 1000)
            {
                tmp = tu.ms;
                tu.ms %= 1000;
                tu.s += tmp / 1000;
            }
            if (tu.s >= 60)
            {
                tmp = tu.s;
                tu.s %= 60;
                tu.min += tmp / 60;
            }
            if (tu.min >= 60)
            {
                tmp = tu.min;
                tu.min %= 60;
                tu.h += tmp / 60;
            }
            return tu;
        }
        

        /**
         * @brief Simple multiplication
         */
        inline time_unit operator* (unsigned int _times)        
{
            time_unit tu{
                this->h * _times,
                this->min * _times,
                this->s * _times,
                this->ms * _times,
                this->us * _times,
                this->ns * _times
            };
            unsigned int tmp;
            if (tu.ns >= 1000)
            {
                tmp = tu.ns;
                tu.ns %= 1000;
                tu.ms += tmp / 1000;
            }
            if (tu.us >= 1000)
            {
                tmp = tu.us;
                tu.us %= 1000;
                tu.ms += tmp / 1000;
            }
            if (tu.ms >= 1000)
            {
                tmp = tu.ms;
                tu.ms %= 1000;
                tu.s += tmp / 1000;
            }
            if (tu.s >= 60)
            {
                tmp = tu.s;
                tu.s %= 60;
                tu.min += tmp / 60;
            }
            if (tu.min >= 60)
            {
                tmp = tu.min;
                tu.min %= 60;
                tu.h += tmp / 60;
            }
            return tu;
        }
    };

}



/** @brief Time unit hour literal */
inline i_op::time_unit operator ""_tu_h(unsigned long long _h)
{
    return i_op::time_unit{
        (unsigned int)_h,
        0,
        0,
        0,
        0,
        0
    };
}


/** @brief Time unit minute literal */
inline i_op::time_unit operator ""_tu_min(unsigned long long _min)
{
    return i_op::time_unit{
        0,
        (unsigned int)_min,
        0,
        0,
        0,
        0
    };
}


/** @brief Time unit second literal */
inline i_op::time_unit operator ""_tu_s(unsigned long long _s)
{
    return i_op::time_unit{
        0,
        0,
        (unsigned int)_s,
        0,
        0,
        0
    };
}


/** @brief Time unit millisecond literal */
inline i_op::time_unit operator ""_tu_ms(unsigned long long _ms)
{
    return i_op::time_unit{
        0,
        0,
        0,
        (unsigned int)_ms,
        0,
        0
    };
}


/** @brief Time unit microsecond literal */
inline i_op::time_unit operator ""_tu_us(unsigned long long _us)
{
    return i_op::time_unit{
        0,
        0,
        0,
        0,
        (unsigned int)_us,
        0
    };
}


/** @brief Time unit nanosecond literal */
inline i_op::time_unit operator ""_tu_ns(unsigned long long _ns)
{
    return i_op::time_unit{
        0,
        0,
        0,
        0,
        0,
        (unsigned int)_ns
    };
}
