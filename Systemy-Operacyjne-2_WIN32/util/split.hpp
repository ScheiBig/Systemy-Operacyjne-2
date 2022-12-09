/* ========================================================================== */
/* Author: Marcin Jeznach || plz no steal 😭                                  */
/*                                                                            */
/* Utilities for splitting input based on delimiters                          */
/* ========================================================================== */
#pragma once

#include <vector>
#include <string>

namespace util
{
    /**
     * @brief Wrapper class for splitting functional object
     */
    class split final
    {
    private:
        std::string __delimiter;
    public:
        /**
         * @brief Construct a new functional object.
         *
         * @param _delimiter Delimiter character used for splitting
         */
        explicit split(const std::string& _delimiter) : __delimiter(_delimiter) {}

        /**
         * @brief Invokes parsing operation
         *
         * @param _this string that shall be splitted using this object
         * @return vector containing individual tokens splitted from `_this`
         */
        std::vector<std::string> operator () (const std::string& _this) const
        {
            
            /* ======================================================================================= */
            /* Find first occurrence of `delimiter`, if none is found, then return whole input string */
            /* ======================================================================================= */
            size_t cur_offset = 0;
            auto next_index = _this.find(__delimiter, cur_offset);
            if (next_index == std::string::npos)
            {
                return std::vector<std::string>(1u, _this);
            }

            /* ========================================================================================= */
            /* Save token - substr between cur_offset and next_index, and repeat searching until no more */
            /* delimiters are found                                                                      */
            /* ========================================================================================= */
            std::vector<std::string> result;
            do
            {
                result.push_back(_this.substr(cur_offset, next_index - cur_offset));
                cur_offset = next_index + __delimiter.size();
                next_index = _this.find(__delimiter, cur_offset);
            } while (next_index != std::string::npos);

            /* ====================================================================== */
            /* Push rest of string (after last delimiter) and return vector of tokens */
            /* ====================================================================== */
            result.push_back(_this.substr(cur_offset, _this.size() - cur_offset));
            return result;
        }

        /**
         * @brief Invokes parsing operation
         *
         * @param _str string that shall be splitted using this object
         * @param _functor splitting functional object
         * @return vector containing individual tokens splitted from `_this`
         */
        friend std::vector<std::string> operator >>= (const std::string& _str, const split& _functor)
        {
            return _functor(_str);
        }
    };
}
