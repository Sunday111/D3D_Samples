#include "ShaderVersion.h"
#include "EverydayTools/Exception/CallAndRethrow.h"

namespace keng::graphics_api
{
    const char* ShaderTypeToShaderTarget(ShaderType shaderType, ShaderVersion shaderVersion) {
        return CallAndRethrowM + [&] {
            switch (shaderVersion) {
            case ShaderVersion::_5_0:
                switch (shaderType) {
                case ShaderType::Compute:       return "cs_5_0";
                case ShaderType::Domain:        return "ds_5_0";
                case ShaderType::Geometry:      return "gs_5_0";
                case ShaderType::Hull:          return "hs_5_0";
                case ShaderType::Fragment:      return "ps_5_0";
                case ShaderType::Vertex:        return "vs_5_0";
                default: throw std::invalid_argument("This version (5.0) does not support this shader type");
                }
            case ShaderVersion::_4_1:
                switch (shaderType) {
                case ShaderType::Compute:       return "cs_4_1";
                case ShaderType::Geometry:      return "gs_4_1";
                case ShaderType::Fragment:      return "ps_4_1";
                case ShaderType::Vertex:        return "vs_4_1";
                default: throw std::invalid_argument("This version (4.1) does not support this shader type");
                }
            case ShaderVersion::_4_0:
                switch (shaderType) {
                case ShaderType::Compute:       return "cs_4_0";
                case ShaderType::Geometry:      return "gs_4_0";
                case ShaderType::Fragment:      return "ps_4_0";
                case ShaderType::Vertex:        return "vs_4_0";
                default: throw std::invalid_argument("This version (4.0) does not support this shader type");
                }
            default:
                throw std::invalid_argument("This version is not supported");
            }
        };
    }
}
