/* ========================================================================== */
/* Author: Marcin Jeznach || plz no steal 😭                                  */
/*                                                                            */
/* Shared memory primitive with auto mapping and OS-independent interface     */
/* ========================================================================== */
#pragma once

#include "macro.hpp"

#ifdef OS_WIN32
#include <windows.h>
#endif
#ifdef OS_LINUX
#include <sys/mman.h>
#endif

#include "./timeunit.hpp"

namespace i_op
{
    /**
     * @brief Named shared memory with mapping
     *
     * @tparam T type of objects (or primitives) held in memory
     * @tparam _cr_new if false, object will open the shared memory if one with specified name already exist,
     *         if true, will fail
     */
    template<typename T, bool _cr_new = false>
    class shared_memory final
    {
    private:
        T* __ptr{ nullptr };
        size_t __len{ 0 };
        char const* __name;
#ifdef OS_WIN32
        HANDLE __fl_hdl{ nullptr };
#endif
#ifdef OS_LINUX
        int __fl_dtr{ -1 };
#endif
    public:
        /**
         * @brief Creates (or opens) and a initializes shared memory object and mapping
         *
         * @param _name name of shared memory object
         * @param _n_elem number of elements (of type `T`) that memory should hold
         */
        shared_memory(char const* _name, size_t _n_elem) noexcept(false);

        /**
         * @brief Performs destruction of object, ignoring exceptions on failure
         */
        ~shared_memory();

        /**
         * @return pointer to the beginning of mapping of shared memory
         */
        T* const ptr();
        /**
         * @return length of the mapping of shared memory (treated as array of `T`)
         */
        size_t len();

        /**
         * @brief Performs unpapping of shared memory, throwing on failure
         * @throw i_op::error_msg – contains OS-specific error code
         */
        void unmap() noexcept(false);
        /**
         * @brief Performs destruction of object, throwing on failure
         * @throw i_op::error_msg – contains OS-specific error code
         */
        void close() noexcept(false);
    };
}


/* ============== */
/* Implementation */
/* ============== */

#include "./error_msg.hpp"
#ifdef OS_WIN32
#include <excpt.h>
#endif
#ifdef OS_LINUX
#include <fcntl.h>
#include <unistd.h>
#endif

template<typename T, bool _cr_new>
i_op::shared_memory<T, _cr_new>::shared_memory(char const* _name, size_t _n_elem)
{
#ifdef OS_WIN32
    this->__fl_hdl = CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, _n_elem * sizeof(T), _name);
    DWORD err{ GetLastError() };
    if (this->__fl_hdl == nullptr)
    {
        throw i_op::error_msg{ err, "i_op::shared_memory<T, bool>::shared_memory(char const*, size_t)", "CreateFileMappingA(HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD, DWORD, LPCSTR)" };
    }
    if (_cr_new && err == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(this->__fl_hdl);
        throw i_op::error_msg{ err, "i_op::shared_memory<T, true>::shared_memory(char const*, size_t)", "CreateFileMappingA(HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD, DWORD, LPCSTR)" };
    }


    this->__ptr = (T*)MapViewOfFile(this->__fl_hdl, FILE_MAP_ALL_ACCESS, 0, 0, _n_elem * sizeof(T));
    err = GetLastError();
    if (this->__ptr == nullptr)
    {
        throw i_op::error_msg{ err, "i_op::shared_memory<T, bool>::shared_memory(char const*, size_t)", "MapViewOfFile(HANDLE, DWORD, DWORD, DWORD, SIZE_T)" };
    }
#endif
#ifdef OS_LINUX
    int o_fl{ O_CREAT | O_RDWR };
    if (_cr_new) { o_fl |= O_EXCL; }
    this->__fl_dtr = shm_open(_name, o_fl, 0664);
    if (this->__fl_dtr < 0)
    {
        throw i_op::error_msg{ errno, "i_op::shared_memory<T, bool>::shared_memory(char const*, size_t)", "shm_open(char const*, int, mode_t)" };
    }
    this->__name = _name;

    if (ftruncate(this->__fl_dtr, _n_elem * sizeof(T)) != 0)
    {
        shm_unlink(this->__name);
        throw i_op::error_msg{ errno, "i_op::shared_memory<T, bool>::shared_memory(char const*, size_t)", "ftruncate(int, size_t)" };
    }
    this->__len = _n_elem;

    this->__ptr = (T*)mmap(nullptr, this->__len, PROT_READ | PROT_WRITE, MAP_SHARED, this->__fl_dtr, 0);
    if (this->__ptr == MAP_FAILED)
    {
        shm_unlink(this->__name);
        throw i_op::error_msg{ errno, "i_op::shared_memory<T, bool>::shared_memory(char const*, size_t)", "mmap(void*, size_t, int, int, int, off_t)" };
    }
#endif
}

template<typename T, bool _cr_new>
i_op::shared_memory<T, _cr_new>::~shared_memory()
{
    try { if (this->__ptr != nullptr) { this->unmap(); } }
    catch (i_op::error_msg const&) {}

    try { if (this->__fl_hdl != nullptr) { this->close(); } }
    catch (i_op::error_msg const&) {}
}

template<typename T, bool _cr_new>
void i_op::shared_memory<T, _cr_new>::unmap()
{
#ifdef OS_WIN32
    if (!!UnmapViewOfFile(this->__ptr) == false)
    {
        throw i_op::error_msg{ GetLastError(), "i_op::shared_memory<T, bool>::unmap()", "UnmapViewOfFile(LPCVOID)" };
    }
#endif
#ifdef OS_LINUX
    if (munmap(this->__ptr, this->__len) != 0)
    {
        throw i_op::error_msg{ errno, "i_op::shared_memory<T, bool>::unmap()", "munmap(void*, size_t)" };
    }
#endif
    this->__ptr == nullptr;
    this->__len = 0;
}

template<typename T, bool _cr_new>
void i_op::shared_memory<T, _cr_new>::close()
{
#ifdef OS_WIN32
    if (!!CloseHandle(this->__fl_hdl) == false)
    {
        throw i_op::error_msg{ GetLastError(), "i_op::shared_memory<T, bool>::close()", "CloseHandle(HANDLE)" };
    }
    this->__fl_hdl == nullptr;
#endif
#ifdef OS_LINUX
    int e[3]{ 0 };
    if (munmap(this->__ptr, this->__len * sizeof(T)) != 0)
    {
        e[0] = errno;
    }
    if (shm_unlink(this->__name) != 0)
    {
        e[1] = errno;
    }
    if (::close(this->__fl_dtr) != 0)
    {
        e[2] = errno;
    }
    this->__fl_dtr = -1;
#endif
    this->__name = nullptr;
#ifdef OS_LINUX
    const char* m = "i_op::shared_memory<T, bool>::close()";
    if (e[0] != 0)
    {
        throw i_op::error_msg{ e[0], m, "munmap(void*, size_t)" };
    }
    if (e[1] != 0)
    {
        throw i_op::error_msg{ e[1], m, "shm_unlink(char const*)" };
    }
    if (e[2] != 0)
    {
        throw i_op::error_msg{ e[2], m, "close(int)" };
    }
#endif
}

template<typename T, bool _cr_new>
T* const i_op::shared_memory<T, _cr_new>::ptr() { return this->__ptr; }

template<typename T, bool _cr_new>
size_t i_op::shared_memory<T, _cr_new>::len() { return this->__len; }
