#pragma once

#include <memory>

namespace keng::graphics::free_type
{
    class GlyphParameters
    {
    public:
        // 1 point equals 1/72th of an inch in digital typography
        // Size of one point in inches
        static constexpr auto PointSize = 1.f / 72.f;

        inline size_t GetMaxWidthPx() const {
            return static_cast<size_t>(width_pt * PointSize * x_dpi + 1);
        }

        inline size_t GetMaxHeightPx() const {
            return static_cast<size_t>(height_pt * PointSize * y_dpi + 1);
        }

        uint32_t unicode = '?';
        size_t x_dpi = 300;
        size_t y_dpi = 300;
        size_t height_pt = 20;
        size_t width_pt = 20;
    };

    class GlyphData
    {
    public:
        uint32_t unicode;

        size_t horizontalBearingX;
        size_t horizontalBearingY;

        size_t verticalBearingX;
        size_t verticalBearingY;

        size_t width;
        size_t height;

        size_t advanceX;
        size_t advanceY;

        std::unique_ptr<uint8_t[]> buffer;
    };
}
