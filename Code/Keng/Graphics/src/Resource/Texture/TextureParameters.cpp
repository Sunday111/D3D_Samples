#include "TextureParameters.h"
#include "yasli/STL.h"

namespace keng::graphics
{

    void TextureParameters::serialize(Archive& ar)
    {
        ar(sourceFile, "file");
        ar(width, "width");
        ar(height, "height");
    }

}
