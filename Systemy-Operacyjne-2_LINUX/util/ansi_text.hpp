/* ========================================================================== */
/* Author: Marcin Jeznach || plz no steal 😭                                  */
/*                                                                            */
/* Various text sequences, normal and control ones                            */
/* ========================================================================== */
#pragma once

#include <vector>
#include <string>
#include <fstream>

namespace util
{
    /// New line character, usually more suitable than `std::endl`
    const char nl = '\n';
};

/// Contains ANSI escape sequences (mainly ones for formatting output)
namespace ANSI
{
    /// Black font
    static const char* black = "\u001b[30m";
    /// Red font
    static const char* red = "\u001b[31m";
    /// Green font
    static const char* green = "\u001b[32m";
    /// Yellow font
    static const char* yellow = "\u001b[33m";
    /// Blue font
    static const char* blue = "\u001b[34m";
    /// Magenta font
    static const char* magenta = "\u001b[35m";
    /// Cyan font
    static const char* cyan = "\u001b[36m";
    /// Almost-white (usually light-gray) font
    static const char* white = "\u001b[37m";
    /// Dark-gray font
    // Best for additional informations (or punctuations)
    static const char* b_black = "\u001b[90m";
    /// Brighter-red font
    // Best for error messages (terminating exceptions)
    static const char* b_red = "\u001b[91m";
    /// Brighter-green font
    // Best for confirmation messages / printing requested data
    static const char* b_green = "\u001b[92m";
    /// Brighter-yellow font
    // Best for warning messages (handled exceptions)
    static const char* b_yellow = "\u001b[93m";
    /// Brighter-blue font
     // Best for information messages (current state)
    static const char* b_blue = "\u001b[94m";
    /// Brighter-magenta font
     // Best for debug messages
    static const char* b_magenta = "\u001b[95m";
    /// Brighter-cyan font
     // Best for input prompts
    static const char* b_cyan = "\u001b[96m";
    /// True-white font
     // Best for important informations
    static const char* b_white = "\u001b[97m";

    /// Black background
    static const char* bg_black = "\u001b[40m";
    /// Red background
    static const char* bg_red = "\u001b[41m";
    /// Green background
    static const char* bg_green = "\u001b[42m";
    /// Yellow background
    static const char* bg_yellow = "\u001b[43m";
    /// Blue background
    static const char* bg_blue = "\u001b[44m";
    /// Magenta background
    static const char* bg_magenta = "\u001b[45m";
    /// Cyan background
    static const char* bg_cyan = "\u001b[46m";
    /// Almost-white (usually light-gray) background
    static const char* bg_white = "\u001b[47m";
    /// Dark-gray background
    static const char* bg_b_black = "\u001b[100m";
    /// Brighter-red background
    static const char* bg_b_red = "\u001b[101m";
    /// Brighter-green background
    static const char* bg_b_green = "\u001b[102m";
    /// Brighter-yellow background
    static const char* bg_b_yellow = "\u001b[103m";
    /// Brighter-blue background
    static const char* bg_b_blue = "\u001b[104m";
    /// Brighter-magenta background
    static const char* bg_b_magenta = "\u001b[105m";
    /// Brighter-cyan background
    static const char* bg_b_cyan = "\u001b[106m";
    /// True-white background
    static const char* bg_b_white = "\u001b[107m";

    /// Resets all formatting (from this point)
    static const char* reset = "\u001b[0m";
} 
