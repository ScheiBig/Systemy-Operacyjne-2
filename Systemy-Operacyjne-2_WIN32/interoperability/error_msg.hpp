/* ========================================================================== */
/* Author: Marcin Jeznach || plz no steal 😭                                  */
/*                                                                            */
/* Wrapper for OS-specific error codes, that allows printing the detailed     */
/* error messages, based on those codes                                       */
/* ========================================================================== */
#pragma once

#include <iostream>
#include "./macro.hpp"

#ifdef OS_WIN32
#include <windows.h>
#endif
#ifdef OS_LINUX
#include <cstring>
#endif

namespace i_op
{
    /**
     * @brief Wrapper for OS-specific error codes
     */
    struct error_msg
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
         * @brief Prints detailed error message (based on error code, actual meaning may be more intricate)
         * 
         * @param _s Output stream to print to, also used as return reference
         * @param _em Error code wrapper
         */
        friend std::ostream& operator <<(std::ostream& _s, error_msg const& _em)
        {
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
    };
}
