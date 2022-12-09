#ifndef header_file_memory
#define header_file_memory

#include <cstdlib>
#include <new>

template<typename T, std::size_t _size>
class temp_alloc final
{
public:
    temp_alloc()
    {
        ptr = new T[_size];
    }

    ~temp_alloc()
    {
        delete[] ptr;
    }

    T* first()
    {
        return ptr;
    }

    T* last()
    {
        return ptr + _size;
    }

    std::size_t size()
    {
        return _size;
    }

private:
    T* ptr;
};

#endif
