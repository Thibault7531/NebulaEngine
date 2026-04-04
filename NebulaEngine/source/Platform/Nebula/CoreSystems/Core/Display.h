#pragma once

#include <cstdint>
#include <cstring>

namespace Nebula::Platform
{
    struct Display
    {
        int32_t x, y;
        uint32_t width, height;
        int32_t workX, workY;
        uint32_t workWidth, workHeight;
        bool primary;

        Display() { memset(this, 0, sizeof(*this)); }
    };
}
