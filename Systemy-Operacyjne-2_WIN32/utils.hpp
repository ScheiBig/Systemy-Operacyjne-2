#pragma once

#include <vector>
#include <string>
#include <fstream>

namespace util
{
    using std::vector;
    using std::string;
    class split final
    {
    private:
        string delimiter;
    public:
        explicit split(const string& _delimiter) : delimiter(_delimiter) {}
        vector<string> operator () (const string& _this) const
        {

            size_t cur_offset = 0;
            auto next_index = _this.find(delimiter, cur_offset);
            if (next_index == string::npos)
            {
                return vector<string>(1u, _this);
            }
            vector<string> result;
            do
            {
                result.push_back(_this.substr(cur_offset, next_index - cur_offset));
                cur_offset = next_index + delimiter.size();
                next_index = _this.find(delimiter, cur_offset);
            } while (next_index != string::npos);

            result.push_back(_this.substr(cur_offset, _this.size() - cur_offset));
            return result;
        }

        friend vector<string> operator >>= (const string& _str, const split& _functor)
        {
            return _functor(_str);
        }
    };

    using std::fstream;
    template <typename T>
    class autoclose_stream
    {
    public:
        autoclose_stream(T& _stream) : stream(_stream) {}
        ~autoclose_stream()
        {
            stream.close();
        }
    private:
        T& stream;
    };

    const char nl = '\n';
}

namespace ANSI
{
    static const char* black = "\u001b[30m";
    static const char* red = "\u001b[31m";
    static const char* green = "\u001b[32m";
    static const char* yellow = "\u001b[33m";
    static const char* blue = "\u001b[34m";
    static const char* magenta = "\u001b[35m";
    static const char* cyan = "\u001b[36m";
    static const char* white = "\u001b[37m";
    static const char* b_black = "\u001b[90m";
    static const char* b_red = "\u001b[91m";
    static const char* b_green = "\u001b[92m";
    static const char* b_yellow = "\u001b[93m";
    static const char* b_blue = "\u001b[94m";
    static const char* b_magenta = "\u001b[95m";
    static const char* b_cyan = "\u001b[96m";
    static const char* b_white = "\u001b[97m";

    static const char* bg_black = "\u001b[40m";
    static const char* bg_red = "\u001b[41m";
    static const char* bg_green = "\u001b[42m";
    static const char* bg_yellow = "\u001b[43m";
    static const char* bg_blue = "\u001b[44m";
    static const char* bg_magenta = "\u001b[45m";
    static const char* bg_cyan = "\u001b[46m";
    static const char* bg_white = "\u001b[47m";
    static const char* bg_b_black = "\u001b[100m";
    static const char* bg_b_red = "\u001b[101m";
    static const char* bg_b_green = "\u001b[102m";
    static const char* bg_b_yellow = "\u001b[103m";
    static const char* bg_b_blue = "\u001b[104m";
    static const char* bg_b_magenta = "\u001b[105m";
    static const char* bg_b_cyan = "\u001b[106m";
    static const char* bg_b_white = "\u001b[107m";
     
    static const char* reset = "\u001b[0m";
} 
