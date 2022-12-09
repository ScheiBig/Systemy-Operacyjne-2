/* ========================================================================== */
/* Author: Marcin Jeznach || plz no steal 😭                                  */
/*                                                                            */
/* `C++ STL` containers utility and factory functions                         */
/* ========================================================================== */
#pragma once

#include <map>

/**
 * @brief Create a map object by mapping input array
 * 
 * @tparam I type of input elements
 * @tparam K type of resulting keys
 * @tparam V type of resulting values
 * @param _input_arr null-terminated array of input values
 * @param _key_gen function generating keys from inputs
 * @param _val_gen function generating values from inputs
 */
template<typename I, typename K, typename V>
std::map<K, V> create_map(I* _input_arr, K(*_key_gen)(I _from), V (*_val_gen)(I _from))
{
    std::map<K, V> result;
    for (I* f = _input_arr; *f != nullptr; f++)
    {
        K key = _key_gen(*f);
        V val = _val_gen(*f);
        result[key] = val;
    }

    return result;
}

/**
 * @brief Create a map object by mapping input array
 *
 * @tparam I type of input elements
 * @tparam K type of resulting keys
 * @tparam V type of resulting values
 * @param _input_arr null-terminated array of input values
 * @param _pair_gen function generating pairs from inputs
 */
template<typename I, typename K, typename V>
std::map<K, V> create_map(I* _input_arr, std::pair<K, V>(*_pair_gen)(I _from))
{
    std::map<K, V> result;
    for (I* f = _input_arr; *f != nullptr; f++)
    {
        std::pair<K, V> pair = _pair_gen(*f);
        result[pair.first] = pair.second;
    }

    return result;
}

/**
 * @brief Create a map object by mapping input array
 *
 * @tparam I type of input elements
 * @tparam K type of resulting keys
 * @tparam V type of resulting values
 * @param _input_arr array of input values
 * @param _n_elem number of elements to pull from input array
 * @param _key_gen function generating keys from inputs
 * @param _val_gen function generating values from inputs
 */
template<typename I, typename K, typename V>
std::map<K, V> create_map(I* _input_arr, std::size_t _n_elem, K(*_key_gen)(I _from), V(*_val_gen)(I _from))
{
    std::map<K, V> result;
    for (I* f = _input_arr; *f != _input_arr + _n_elem; f++)
    {
        K key = _key_gen(*f);
        V val = _val_gen(*f);
        result[key] = val;
    }

    return result;
}

/**
 * @brief Create a map object by mapping input array
 *
 * @tparam I type of input elements
 * @tparam K type of resulting keys
 * @tparam V type of resulting values
 * @param _input_arr null-terminated array of input values
 * @param _n_elem number of elements to pull from input array
 * @param _pair_gen function generating pairs from inputs
 */
template<typename I, typename K, typename V>
std::map<K, V> create_map(I* _input_arr, std::size_t _n_elem, std::pair<K, V>(*_pair_gen)(I _from))
{
    std::map<K, V> result;
    for (I* f = _input_arr; *f != _input_arr + _n_elem; f++)
    {
        std::pair<K, V> pair = _pair_gen(*f);
        result[pair.first] = pair.second;
    }

    return result;
}
