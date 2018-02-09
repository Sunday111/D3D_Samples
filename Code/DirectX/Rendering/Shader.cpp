#include "Shader.h"

ComPtr<ID3DBlob> CompileShaderToBlob(const char* code, const char* entryPoint, ShaderType shaderType, ShaderVersion shaderVersion) {
    return CallAndRethrowM + [&] {
        auto datasize = std::char_traits<char>::length(code);
        D3D_SHADER_MACRO macro[] = { nullptr, nullptr };
        auto shaderTarget = ShaderTypeToShaderTarget(shaderType, shaderVersion);
        ComPtr<ID3DBlob> shaderBlob;
        ComPtr<ID3DBlob> errorBlob;
        // This lambda combines described windows error code with d3d compiler error
        // and throws and exception
        auto onCompileError = [&](std::string errorMessage) {
            if (errorBlob.Get()) {
                errorMessage += ": ";
                errorMessage.append(
                    (char*)errorBlob->GetBufferPointer(),
                    errorBlob->GetBufferSize());
            }
            throw std::runtime_error(std::move(errorMessage));
        };

        UINT flags1 = 0
        #ifdef _DEBUG
            | D3DCOMPILE_DEBUG
            | D3DCOMPILE_SKIP_OPTIMIZATION
            | D3DCOMPILE_WARNINGS_ARE_ERRORS
            | D3DCOMPILE_ALL_RESOURCES_BOUND
        #endif;
            ;

        WinAPI<char>::HandleError(D3DCompile(
            code,
            datasize,
            nullptr,              // May be used for debugging
            macro,                // Null-terminated array of macro definitions
            nullptr,              // Includes
            entryPoint,           // Main function of shader
            shaderTarget,         // shader target
            flags1,               // Flags for compile constants
            0,                    // Flags for compile effects constants
            shaderBlob.Receive(), // Output compiled shader
            errorBlob.Receive()   // Compile error messages
        ), onCompileError);
        return shaderBlob;
    };
}

const char* ShaderTypeToShaderTarget(ShaderType shaderType, ShaderVersion shaderVersion) {
    return CallAndRethrowM + [&] {
        switch (shaderVersion)
        {
        case ShaderVersion::_5_0:
            switch (shaderType)
            {
            case ShaderType::Compute: return "cs_5_0";
            case ShaderType::Domain:        return "ds_5_0";
            case ShaderType::Geometry:      return "gs_5_0";
            case ShaderType::Hull:          return "hs_5_0";
            case ShaderType::Pixel:         return "ps_5_0";
            case ShaderType::Vertex:        return "vs_5_0";
            default: throw std::invalid_argument("This version (5.0) does not support this shader type");
            }
        case ShaderVersion::_4_1:
            switch (shaderType)
            {
            case ShaderType::Compute: return "cs_4_1";
            case ShaderType::Geometry:      return "gs_4_1";
            case ShaderType::Pixel:         return "ps_4_1";
            case ShaderType::Vertex:        return "vs_4_1";
            default: throw std::invalid_argument("This version (4.1) does not support this shader type");
            }
        case ShaderVersion::_4_0:
            switch (shaderType)
            {
            case ShaderType::Compute: return "cs_4_0";
            case ShaderType::Geometry:      return "gs_4_0";
            case ShaderType::Pixel:         return "ps_4_0";
            case ShaderType::Vertex:        return "vs_4_0";
            default: throw std::invalid_argument("This version (4.0) does not support this shader type");
            }
        default:
            throw std::invalid_argument("This version is not supported");
        }
    };
}
