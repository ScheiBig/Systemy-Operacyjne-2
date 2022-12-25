/* ========================================================================== */
/* Author: Marcin Jeznach || plz no steal 😭                                  */
/*                                                                            */
/* Basic string formatting utility, similar to `std::format` (C++20)          */
/* ========================================================================== */
#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <string_view>
#include <stdexcept>

namespace util
{
    template<typename T>
    void __________format_helper(std::ostringstream& _oss, std::string_view& _str, const T& _val)
    {
        std::size_t dolar_i;
        std::size_t escaped_dolar_i;


        while (true)
        {
            dolar_i = _str.find('$');
            escaped_dolar_i = _str.find("$$");

            if (dolar_i == std::string::npos)
            {
                return;
            }
            if (dolar_i == escaped_dolar_i)
            {
                _oss << _str.substr(0, dolar_i + 1);
                _str = _str.substr(escaped_dolar_i + 2);
                continue;
            }
            break;
        }

        _oss << _str.substr(0, dolar_i) << _val;
        _str = _str.substr(dolar_i + 1);
    }

    /**
     * @brief Performs basic formating of input string, using `std::ostringstream`
     *
     * Function is fully unaware of number of values, so:
     * if too much arguments is being supplied, additional ones are ignored,
     * if too little arguments is being supplied, rest of placeholders will remain as dollar signs.
     *
     * @tparam Targs type of parameter pack
     * @param _str Input format string, with `$` characters used for formatting (`$$` escapes dolar character)
     * @param _args Parameter pack used as replacements for `_str` placeholders
     * @return Formatted string
     */
    template<typename... Targs>
    std::string bformat(std::string_view _str, Targs..._args)
    {
        std::ostringstream _oss;
        (__________format_helper(_oss, _str, _args), ...);
        _oss << _str;
        return _oss.str();
    }
}
