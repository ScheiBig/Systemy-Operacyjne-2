/* ========================================================================== */
/* Author: Marcin Jeznach || plz no steal 😭                                  */
/*                                                                            */
/* Wrapper for OS-specific error codes, that allows printing the detailed     */
/* error messages, based on those codes                                       */
/* ========================================================================== */
#pragma once
#include "./macro.hpp"

#include <iostream>

#ifdef OS_WIN32
#include <windows.h>
#endif
#ifdef OS_LINUX
#include <cstring>
#endif
#include <exception>
#include <iostream>

namespace i_op
{
    /**
     * @brief Wrapper for OS-specific error codes
     */
    struct error_msg: public std::exception
    {
#ifdef OS_WIN32
        /** @brief Error code */
        DWORD __err_code;
#endif
#ifdef OS_LINUX
        /** @brief Error code */
        long __err_code;
#endif

        /**
         * @brief Description of source of error
         */
        char const* __err_src;

        /**
         * @brief Description of native source of error
         */
        char const* __err_dsc;


#ifdef OS_WIN32
        inline error_msg(DWORD _err_code, char const* _err_src, char const* _err_dsc):
            __err_code(_err_code), __err_src(_err_src), __err_dsc(_err_dsc)
        {
        }
#endif
#ifdef OS_LINUX
        inline error_msg(long _err_code, char const* _err_src, char const* _err_dsc):
            __err_code(_err_code), __err_src(_err_src), __err_dsc(_err_dsc)
        {
        }
#endif

        /**
         * @brief Prints detailed error message (based on error code, actual meaning may be more intricate)
         *
         * @param _s Output stream to print to, also used as return reference
         * @param _em Error code wrapper
         */
        inline friend std::ostream& operator <<(std::ostream& _s, error_msg const& _em)
        {
            _s << "\u001b[91m" << _em.__err_src << "\u001b[0m" << " @ " << "\u001b[95m" << _em.__err_dsc << "\u001b[0m" << '\n';
#ifdef OS_WIN32
            LPSTR msg_buf = nullptr;

            [[maybe_unused]] size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr, _em.__err_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg_buf, 0, nullptr);

            _s << msg_buf;

            LocalFree(msg_buf);
#endif
#ifdef OS_LINUX
            _s << std::strerror((int)_em.__err_code);
#endif
            return _s;
        }

        inline char const* what() const noexcept override
        {
            std::cerr << *this;
            return "";
        }
    };
}
