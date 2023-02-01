#pragma once

#include <stdio.h>
#include <vector>
#include <cstdint>
#include <fstream>

namespace bmp
{
    constexpr uint8_t Bytes_Per_Px = 3;
    
#pragma pack(push, 1)
    typedef struct file_header
    {
        uint8_t type_1 = 'B';
        uint8_t type_2 = 'M';
        uint32_t file_size;
        uint16_t reserved_1 = 0;
        uint16_t reserved_2 = 0;
        uint32_t img_offset;
    } file_header_t;
#pragma pack(pop)

#pragma pack(push, 1)
    typedef struct info_header
    {
        uint32_t header_size;
        int32_t img_width;
        int32_t img_height;
        uint16_t color_planes = 1;
        uint16_t color_depth = 8 * Bytes_Per_Px;
        uint32_t compression_type = 0;
        uint32_t img_size = 0;
        int32_t img_h_res = 0;
        int32_t img_v_res = 0;
        uint32_t color_palette_n = 0;
        uint32_t important_color_n = 0;
    } info_header_t;
#pragma pack(pop)

#pragma pack(push, 1)
    typedef struct pixel
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    } pixel_t;
#pragma pack(pop)

    uint32_t little_end(uint32_t _num)
    {
        short int word = 0x0001;
        char* byte = (char*)&word;
        if (byte[0]) { return _num; }
        _num = ((_num << 8) & 0xFF00FF00) | ((_num >> 8) & 0xFF00FF);
        return (_num << 16) | (_num >> 16);
    }

    int32_t little_end(int32_t _num)
    {
        short int word = 0x0001;
        char* byte = (char*)&word;
        if (byte[0]) { return _num; }
        _num = ((_num << 8) & 0xFF00FF00) | ((_num >> 8) & 0xFF00FF);
        return (_num << 16) | ((_num >> 16) & 0xFFFF);
    }

    uint16_t little_end(uint16_t _num)
    {
        short int word = 0x0001;
        char* byte = (char*)&word;
        if (byte[0]) { return _num; }
        return (_num << 8) | (_num >> 8);
    }

    file_header_t file_header(uint32_t _height, uint32_t _stride)
    {
        uint32_t f_off = sizeof(file_header_t) + sizeof(info_header_t);
        uint32_t f_size = f_off + (_height * _stride);

        file_header_t fh{};

        fh.file_size = little_end(f_size);
        fh.img_offset = little_end(f_off);

        return fh;
    }

    /**
     * @brief Saves monochromatic bmp image to file
     *
     * @param _img `vector` that contains image (vector[height][width])
     * @param _path file path of output
     */
    info_header_t info_header(uint32_t _height, uint32_t _width)
    {
        info_header_t ih{};

        ih.header_size = little_end((uint32_t)sizeof(info_header_t));
        ih.img_width = little_end((int32_t)_width);
        ih.img_height = little_end((int32_t)_height);

        return ih;
    }

    /**
     * @brief Saves monochromatic bmp image to file
     * 
     * @param _img `vector` that contains image (vector[height][width], root in left-bottom corner)
     * @param _path file path of output
     */
    void save_bitmap_mono(std::vector<std::vector<uint8_t>> _img, char const* _path)
    {
        uint32_t width_bytes = _img[0].size() * Bytes_Per_Px;

        uint8_t pad[] = { 0, 0, 0 };
        uint8_t pad_len = (4 - width_bytes % 4) % 4;

        std::fstream img(_path, img.binary | img.out);
        
        auto fh = file_header(_img.size(), width_bytes + pad_len);
        img.write((char*)&fh, sizeof(fh));
        
        auto ih = info_header(_img.size(), _img[0].size());
        img.write((char*)&ih, sizeof(ih));

        for (auto row : _img)
        {
            for (auto px : row)
            {
                pixel_t pix{ px, px, px };
                img.write((char*)&pix, sizeof(pix));
            }
            img.write((char*)pad, pad_len);
        }
    }


    /**
     * @brief Saves colour bmp image to file
     *
     * @param _img `vector` that contains image (vector[height][width], root in left-bottom corner)
     * @param _path file path of output
     */
    void save_bitmap_rgb(std::vector<std::vector<pixel_t>> _img, char const* _path)
    {
        uint32_t width_bytes = _img[0].size() * Bytes_Per_Px;

        uint8_t pad[] = { 0, 0, 0 };
        uint8_t pad_len = (4 - width_bytes % 4) % 4;

        std::fstream img(_path, img.binary | img.out);

        auto fh = file_header(_img.size(), width_bytes + pad_len);
        img.write((char*)&fh, sizeof(fh));

        auto ih = info_header(_img.size(), _img[0].size());
        img.write((char*)&ih, sizeof(ih));

        for (auto row : _img)
        {
            for (auto px : row)
            {
                img.write((char*)&px, sizeof(px));
            }
            img.write((char*)pad, pad_len);
        }
    }
}
