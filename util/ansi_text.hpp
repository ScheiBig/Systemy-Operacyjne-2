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
    /**
     * @brief New line character, usually more suitable than `std::endl`
     */
    [[maybe_unused]] const char nl = '\n';
}

/**
 * @brief Contains ANSI escape sequences (mainly ones for formatting output)
 */
namespace ANSI
{
    /**
     * @brief Black font
     */
    [[maybe_unused]] static const char* black = "\u001b[30m";
    /**
     * @brief Red font
     */
    [[maybe_unused]] static const char* red = "\u001b[31m";
    /**
     * @brief Green font
     */
    [[maybe_unused]] static const char* green = "\u001b[32m";
    /**
     * @brief Yellow font
     */
    [[maybe_unused]] static const char* yellow = "\u001b[33m";
    /**
     * @brief Blue font
     */
    [[maybe_unused]] static const char* blue = "\u001b[34m";
    /**
     * @brief Magenta font
     */
    [[maybe_unused]] static const char* magenta = "\u001b[35m";
    /**
     * @brief Cyan font
     */
    [[maybe_unused]] static const char* cyan = "\u001b[36m";
    /**
     * @brief Almost-white (usually light-gray) font
     */
    [[maybe_unused]] static const char* white = "\u001b[37m";
    /**
     * @brief Dark-gray font
     */
     // Best for additional informations (or punctuations)
    [[maybe_unused]] static const char* b_black = "\u001b[90m";
    /**
     * @brief Brighter-red font
     */
     // Best for error messages (terminating exceptions)
    [[maybe_unused]] static const char* b_red = "\u001b[91m";
    /**
     * @brief Brighter-green font
     */
     // Best for confirmation messages / printing requested data
    [[maybe_unused]] static const char* b_green = "\u001b[92m";
    /**
     * @brief Brighter-yellow font
     */
     // Best for warning messages (handled exceptions)
    [[maybe_unused]] static const char* b_yellow = "\u001b[93m";
    /**
     * @brief Brighter-blue font
     */
     // Best for information messages (current state)
    [[maybe_unused]] static const char* b_blue = "\u001b[94m";
    /**
     * @brief Brighter-magenta font
     */
     // Best for debug messages
    [[maybe_unused]] static const char* b_magenta = "\u001b[95m";
    /**
     * @brief Brighter-cyan font
     */
     // Best for input prompts
    [[maybe_unused]] static const char* b_cyan = "\u001b[96m";
    /**
     * @brief True-white font
     */
     // Best for important informations
    [[maybe_unused]] static const char* b_white = "\u001b[97m";

    /**
     * @brief Black background
     */
    [[maybe_unused]] static const char* bg_black = "\u001b[40m";
    /**
     * @brief Red background
     */
    [[maybe_unused]] static const char* bg_red = "\u001b[41m";
    /**
     * @brief Green background
     */
    [[maybe_unused]] static const char* bg_green = "\u001b[42m";
    /**
     * @brief Yellow background
     */
    [[maybe_unused]] static const char* bg_yellow = "\u001b[43m";
    /**
     * @brief Blue background
     */
    [[maybe_unused]] static const char* bg_blue = "\u001b[44m";
    /**
     * @brief Magenta background
     */
    [[maybe_unused]] static const char* bg_magenta = "\u001b[45m";
    /**
     * @brief Cyan background
     */
    [[maybe_unused]] static const char* bg_cyan = "\u001b[46m";
    /**
     * @brief Almost-white (usually light-gray) background
     */
    [[maybe_unused]] static const char* bg_white = "\u001b[47m";
    /**
     * @brief Dark-gray background
     */
    [[maybe_unused]] static const char* bg_b_black = "\u001b[100m";
    /**
     * @brief Brighter-red background
     */
    [[maybe_unused]] static const char* bg_b_red = "\u001b[101m";
    /**
     * @brief Brighter-green background
     */
    [[maybe_unused]] static const char* bg_b_green = "\u001b[102m";
    /**
     * @brief Brighter-yellow background
     */
    [[maybe_unused]] static const char* bg_b_yellow = "\u001b[103m";
    /**
     * @brief Brighter-blue background
     */
    [[maybe_unused]] static const char* bg_b_blue = "\u001b[104m";
    /**
     * @brief Brighter-magenta background
     */
    [[maybe_unused]] static const char* bg_b_magenta = "\u001b[105m";
    /**
     * @brief Brighter-cyan background
     */
    [[maybe_unused]] static const char* bg_b_cyan = "\u001b[106m";
    /**
     * @brief True-white background
     */
    [[maybe_unused]] static const char* bg_b_white = "\u001b[107m";

    /**
     * @brief Resets all formatting (from this point)
     */
    [[maybe_unused]] static const char* reset = "\u001b[0m";
}
