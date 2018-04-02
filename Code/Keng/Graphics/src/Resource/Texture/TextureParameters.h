#pragma once

#include <string>
#include "Keng/Base/Serialization/Serialization.h"
#include "keng/Graphics/TextureUsage.h"

namespace keng::graphics
{
    class TextureParameters
    {
    public:
        void serialize(Archive& ar);

        std::string sourceFile;
        uint32_t width = 0;
        uint32_t height = 0;
        TextureUsage usage = TextureUsage::ShaderResource;
    };
}