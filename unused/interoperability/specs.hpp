#pragma once

namespace i_op
{
    enum struct specs
    {
        /** @brief No flags */
        _ = 0x0,
        /** @brief Create only */
        C_ = 0x1,
        /** @brief Create, Read */
        C_R = 0x1 | 0x2,
        /** @brief Create, Write */
        C_W = 0x1 | 0x4,
        /** @brief Create, Read & Write */
        C_RW = 0x1 | 0x2 | 0x4,
        /** @brief Read */
        _R = 0x2,
        /** @brief Write */
        _W = 0x4,
        /** @brief Read & Write */
        _RW = 0x2 | 0x4
    };
}