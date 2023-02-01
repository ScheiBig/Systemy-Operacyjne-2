/* ========================================================================== */
/* Author: Marcin Jeznach || plz no steal 😭                                  */
/*                                                                            */
/*                                                                            */
/* ========================================================================== */
#pragma once
#include "./macro.hpp"

#include "./mutex.hpp"

namespace i_op
{
    /**
     * @brief RAII wrapper for mutex objects
     * 
     */
    class critical_section
    {
    private:
        i_op::basic_mutex& __mux;
    public:
        /**
         * @brief Construct a new critical section object, which will lock `_mux` immediately,
         *        and unlock in in destructor (when current scope ends)
         */
        inline critical_section(i_op::basic_mutex& _mux): __mux(_mux)
        {
            this->__mux.lock();
        }

        inline ~critical_section()
        {
            this->__mux.release();
        }
    };
}
