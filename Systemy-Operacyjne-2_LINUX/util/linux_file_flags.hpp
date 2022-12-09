#pragma once 

#include <fcntl.h>

namespace O_flag
{
    enum struct access
    {
        READ_ONLY = O_RDONLY,
        WRITE_ONLY = O_WRONLY,
        READ_WRITE = O_RDWR,
        // EXECUTE_ONLY = ,
    };

}