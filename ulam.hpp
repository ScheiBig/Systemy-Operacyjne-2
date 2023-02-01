#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>

#include "util/memory.hpp"
#include "util/bmp.hpp"
#include "sieving_strategies.hpp"

namespace ulam
{
    /**
     * @brief Maps 2D coordinates to 1D
     */
    int map(int _x, int _y, int _width)
    {
        return _x + _y * _width;
    }

    /**
     * @brief Prints Ulam Spiral in simple text format
     * 
     * @param _path file path of output
     * @param _sieve already calculated, sequential memory containing numbers in range 0..n
     */
    void print(char const* _path, util::mem<char>& _sieve)
    {
        /* ==================================================================================== */
        /* Calculate smallest possible, odd-numbered side length of square that can hold spiral */
        /* ==================================================================================== */
        int side = std::ceil(std::sqrt(_sieve.len() - 1));
        side += side % 2 == 0 ? 1 : 0;

        auto result = util::mem<char>::calloc(side * side);

        {
            int x = side / 2;
            int y = side / 2;
            int steps_before_turn = 1;
            bool change_half = false;
            int direction = 0;

            for (int i = 1; i <= side * side; i++)
            {
                /* ====================================== */
                /* Print next byte, or blank if none left */
                /* ====================================== */
                if (i <= (signed)_sieve.len())
                {
                    result.ptr()[map(x, y, side)] = _sieve.ptr()[i];
                }
                else
                {
                    result.ptr()[map(x, y, side)] = ' ';
                }
                /*=================================================================================== */
                /* Change direction if necessary - each two changes of direction length before change */
                /* gets longer by 1                                                                                   */
                /*=================================================================================== */
                switch (direction)
                {
                case 0:
                    x++;
                    break;
                case 1:
                    y--;
                    break;
                case 2:
                    x--;
                    break;
                case 3:
                    y++;
                    break;
                }
                if (i % steps_before_turn == 0)
                {
                    direction = (direction + 1) % 4;
                    change_half = !change_half;
                    if (change_half)
                    {
                        steps_before_turn++;
                    }
                }
            }
        }

        /* ============= */
        /* Print to file */
        /* ============= */
        std::fstream out(_path, out.out);
        for (int y = 0; y < side; y++)
        {
            for (int x = 0; x < side; x++)
            {
                out << result.ptr()[map(x, y, side)];
            }
            out << "\n";
        }

        result.free();
    }

    /**
     * @brief Saves Ulam Spiral in bmp image format
     *
     * @param _path file path of output
     * @param _sieve already calculated, sequential memory containing numbers in range 0..n
     */
    void picture(char const* _path, util::mem<char>& _sieve)
    {
        /* ==================================================================================== */
        /* Calculate smallest possible, odd-numbered side length of square that can hold spiral */
        /* ==================================================================================== */
        int side = std::ceil(std::sqrt(_sieve.len() - 1));
        side += side % 2 == 0 ? 1 : 0;

        std::vector<std::vector<uint8_t>> result{ (size_t)side };
        for (auto& row : result)
        {
            row = std::vector<uint8_t>((size_t)side);
        }

        {
            int x = side / 2;
            int y = side / 2;
            int steps_before_turn = 1;
            bool change_half = false;
            int direction = 0;

            for (int i = 1; i <= side * side; i++)
            {
                /* ======================================= */
                /* Print next pixel, or blank if none left */
                /* ======================================= */
                if (i <= (signed)_sieve.len())
                {
                    switch (_sieve.ptr()[i])
                    {
                    case sieving_strategy::num::Root:
                        result[y][x] = 0xaa;
                        break;
                    case sieving_strategy::num::Prime:
                        result[y][x] = 0x00;
                        break;
                    case sieving_strategy::num::Composite:
                        result[y][x] = 0xff;
                        break;
                    }
                }
                else
                {
                    result[y][x] = (uint8_t)0x55;
                }
                /*=================================================================================== */
                /* Change direction if necessary - each two changes of direction length before change */
                /* gets longer by 1                                                                                   */
                /*=================================================================================== */
                switch (direction)
                {
                case 0:
                    x++;
                    break;
                case 1:
                    y--;
                    break;
                case 2:
                    x--;
                    break;
                case 3:
                    y++;
                    break;
                }
                if (i % steps_before_turn == 0)
                {
                    direction = (direction + 1) % 4;
                    change_half = !change_half;
                    if (change_half)
                    {
                        steps_before_turn++;
                    }
                }
            }
        }

        /* ============== */
        /* Print to image */
        /* ============== */
        std::reverse(result.begin(), result.end());
        bmp::save_bitmap_mono(result, _path);
    }
}
