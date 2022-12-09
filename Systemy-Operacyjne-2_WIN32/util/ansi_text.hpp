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
    [[maybe_unused]] const char nl = '\n';
}

/// Contains ANSI escape sequences (mainly ones for formatting output)
namespace ANSI
{
    /// Black font
    [[maybe_unused]] static const char* black = "\u001b[30m";
    /// Red font
    [[maybe_unused]] static const char* red = "\u001b[31m";
    /// Green font
    [[maybe_unused]] static const char* green = "\u001b[32m";
    /// Yellow font
    [[maybe_unused]] static const char* yellow = "\u001b[33m";
    /// Blue font
    [[maybe_unused]] static const char* blue = "\u001b[34m";
    /// Magenta font
    [[maybe_unused]] static const char* magenta = "\u001b[35m";
    /// Cyan font
    [[maybe_unused]] static const char* cyan = "\u001b[36m";
    /// Almost-white (usually light-gray) font
    [[maybe_unused]] static const char* white = "\u001b[37m";
    /// Dark-gray font
    // Best for additional informations (or punctuations)
    [[maybe_unused]] static const char* b_black = "\u001b[90m";
    /// Brighter-red font
    // Best for error messages (terminating exceptions)
    [[maybe_unused]] static const char* b_red = "\u001b[91m";
    /// Brighter-green font
    // Best for confirmation messages / printing requested data
    [[maybe_unused]] static const char* b_green = "\u001b[92m";
    /// Brighter-yellow font
    // Best for warning messages (handled exceptions)
    [[maybe_unused]] static const char* b_yellow = "\u001b[93m";
    /// Brighter-blue font
     // Best for information messages (current state)
    [[maybe_unused]] static const char* b_blue = "\u001b[94m";
    /// Brighter-magenta font
     // Best for debug messages
    [[maybe_unused]] static const char* b_magenta = "\u001b[95m";
    /// Brighter-cyan font
     // Best for input prompts
    [[maybe_unused]] static const char* b_cyan = "\u001b[96m";
    /// True-white font
     // Best for important informations
    [[maybe_unused]] static const char* b_white = "\u001b[97m";

    /// Black background
    [[maybe_unused]] static const char* bg_black = "\u001b[40m";
    /// Red background
    [[maybe_unused]] static const char* bg_red = "\u001b[41m";
    /// Green background
    [[maybe_unused]] static const char* bg_green = "\u001b[42m";
    /// Yellow background
    [[maybe_unused]] static const char* bg_yellow = "\u001b[43m";
    /// Blue background
    [[maybe_unused]] static const char* bg_blue = "\u001b[44m";
    /// Magenta background
    [[maybe_unused]] static const char* bg_magenta = "\u001b[45m";
    /// Cyan background
    [[maybe_unused]] static const char* bg_cyan = "\u001b[46m";
    /// Almost-white (usually light-gray) background
    [[maybe_unused]] static const char* bg_white = "\u001b[47m";
    /// Dark-gray background
    [[maybe_unused]] static const char* bg_b_black = "\u001b[100m";
    /// Brighter-red background
    [[maybe_unused]] static const char* bg_b_red = "\u001b[101m";
    /// Brighter-green background
    [[maybe_unused]] static const char* bg_b_green = "\u001b[102m";
    /// Brighter-yellow background
    [[maybe_unused]] static const char* bg_b_yellow = "\u001b[103m";
    /// Brighter-blue background
    [[maybe_unused]] static const char* bg_b_blue = "\u001b[104m";
    /// Brighter-magenta background
    [[maybe_unused]] static const char* bg_b_magenta = "\u001b[105m";
    /// Brighter-cyan background
    [[maybe_unused]] static const char* bg_b_cyan = "\u001b[106m";
    /// True-white background
    [[maybe_unused]] static const char* bg_b_white = "\u001b[107m";

    /// Resets all formatting (from this point)
    [[maybe_unused]] static const char* reset = "\u001b[0m";
} 
