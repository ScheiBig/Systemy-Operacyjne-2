/* ========================================================================== */
/* Author: Marcin Jeznach || plz no steal 😭                                  */
/*                                                                            */
/* Utilities for working with dynamicly-allocated arrays                      */
/* ========================================================================== */
#pragma once

#include <cstddef>
#include <stdexcept>

#include "bformat.hpp"

/**
 * @brief Fixed-lenght, dynamicly-allocated array, that lives for current scope
 *
 * @tparam T type of elements
 */
template<typename T>
class varray
{
private:
    T* data;
    std::size_t n_elem;
public:
    /**
     * @brief Construct a new varray object
     *
     * @param _n_elem number of elements
     */
    varray(std::size_t _n_elem) : n_elem(_n_elem), data(new T[_n_elem]) {}

    /**
     * @brief Destroy the varray object, deallocating its internal memory
     */
    ~varray()
    {
        if (this->data != nullptr)
        {
            delete[] this->data;
            this->data = nullptr;
        }
    }

    /**
     * @return Number of elements held in this array
     */
    std::size_t length() { return this->n_elem; }

    /**
     * @brief Accesses element of array
     *
     * @param _index index of element that should be accessed
     * @return Reference to element
     * @throw std::out_of_range – when `_index` >= `this->length()`
     */
    T& operator[] (std::size_t _index)
    {
        if (_index >= this->n_elem)
        {
            throw std::out_of_range(util::bformat("Index $ out of range (0..$)", _index, this->n_elem - 1));
        }
        return this->data[_index];
    }

    //=// Doesn't make sense in context of this class
    // /**
    //  * @brief Accesses element of array.
    //  * This array is `const`, and so its elements should be treated as such.
    //  *
    //  * @param _index index of element that should be accessed
    //  * @return Reference to element
    //  * @throw std::out_of_range – when `_index` >= `this->length()`
    //  */
    // const T& operator[] (std::size_t _index) const
    // {
    //     if (_index >= this->n_elem)
    //     {
    //         throw std::out_of_range(util::bformat("Index $ out of range (0..$)", _index, _n_elem - 1));
    //     }
    //     return this->data[_index];
    // }

    /**
     * @brief Simple forward-iterator over `varray`, that allows modification
     */
    class iterator
    {
    private:
        T* ptr;
    public:
        explicit iterator(T* _ptr) : ptr(_ptr) {}

        iterator& operator++()
        {
            this->ptr++;
            return *this;
        }

        iterator operator++(int)
        {
            iterator result = *this;
            ++(*this);
            return result;
        }

        bool operator==(iterator _other) const { return this->ptr == _other.ptr; }

        bool operator!=(iterator _other) const { return !(*this == _other); }

        T& operator*() const { return this->ptr; }
    };

    /**
     * @return iterator to the first element of this array
     */
    iterator begin() { return iterator{ this->data }; }

    /**
     * @return iterator to the next-after last element of this array
     */
    iterator end() { return iterator{ this->data + this->n_elem }; }
};
