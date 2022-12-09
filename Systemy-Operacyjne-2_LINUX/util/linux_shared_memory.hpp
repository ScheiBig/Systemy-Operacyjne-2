/* ========================================================================== */
/* Author: Marcin Jeznach || plz no steal 😭                                  */
/*                                                                            */
/* Wrapper for Linux's shared memory                                          */
/* ========================================================================== */

#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

#include "memory.hpp"

#include "bformat.hpp"

namespace util
{
    template<typename T, bool _auto_unlink, bool _create>
    class shared_memory final
    {
    private:
        T* __mem_ptr;
        std::size_t __mem_len;
        const char* __name;
        int __fd;
    public:
        shared_memory(const char* _name, bool _read, bool _write, bool _new, mode_t _mode, std::size_t _n_elem) noexcept(false)
        {
            int _o_flags{};
            if (_read && _write) { _o_flags |= O_RDWR; }
            else if (_read) { _o_flags |= O_RDONLY; }
            else if (_write) { _o_flags |= O_WRONLY; }

            if (_create)
            {
                _o_flags |= O_CREAT;
                if (_new) { _o_flags |= O_EXCL; }
            }


            this->__fd = shm_open(_name, _o_flags, _mode);
            if (this->__fd == -1)
            {
                throw std::system_error(std::make_error_code((std::errc)errno), "shm_open");
            }
            this->__name = _name;

            std::size_t size = sizeof(T) * _n_elem;

            if (_create)
            {
                int result = ftruncate(this->__fd, size);
                if (result != 0)
                {
                    throw std::system_error(std::make_error_code((std::errc)errno), "ftruncate");
                }
            }

            int _prot_flags{ PROT_NONE };
            if (_read) { _prot_flags |= PROT_READ; }
            if (_write) { _prot_flags |= PROT_WRITE; }

            this->__mem_ptr = (T*)mmap(
                nullptr, size,
                _prot_flags, MAP_SHARED,
                this->__fd, 0
            );

            this->__mem_len = _n_elem;
        }

        ~shared_memory() noexcept(false)
        {
            if (_auto_unlink)
            {
                this->unlink();
            }
        }

        void unlink() noexcept(false)
        {
            int result = shm_unlink(this->__name);
            if (result != 0)
            {
                throw std::system_error(std::make_error_code((std::errc)errno),
                    util::bformat("$, name:$, pid:$", "shm_unlink", this->__name, getpid())
                );
            }
        }

        T* ptr() { return this->__mem_ptr; }
        std::size_t len() { return this->__mem_len; }
    };
}
