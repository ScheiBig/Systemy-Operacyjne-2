#pragma once

#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#define OS_WIN 1
#endif

#if  defined(__linux) || defined(__gnu_linux__) || defined(linux) || defined(__linux__)
#define OS_LINUX 1
#endif
