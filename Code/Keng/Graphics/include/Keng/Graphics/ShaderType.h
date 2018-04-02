#pragma once

#include "yasli/Enum.h"

namespace keng::graphics
{
    enum class ShaderType
    {
        Compute,
        Domain,
        Geometry,
        Hull,
        Fragment,
        Vertex
    };

    YASLI_ENUM_BEGIN(ShaderType, "ShaderType")
        YASLI_ENUM(ShaderType::Compute, "Compute", "Compute")
        YASLI_ENUM(ShaderType::Domain, "Domain", "Domain")
        YASLI_ENUM(ShaderType::Geometry, "Geometry", "Geometry")
        YASLI_ENUM(ShaderType::Hull, "Hull", "Hull")
        YASLI_ENUM(ShaderType::Fragment, "Fragment", "Fragment")
        YASLI_ENUM(ShaderType::Vertex, "Vertex", "Vertex")
    YASLI_ENUM_END()
}
