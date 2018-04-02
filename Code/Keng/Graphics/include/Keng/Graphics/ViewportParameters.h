#pragma once

#include "EverydayTools\Geom\Vector.h"

namespace keng::graphics
{
    class ViewportParameters
    {
    public:
        edt::geom::Vector<float, 2> Position;
        edt::geom::Vector<float, 2> Size;
    };
}
