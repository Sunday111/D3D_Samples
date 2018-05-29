#pragma once

#include "EverydayTools/Array/ArrayView.h"
#include "Keng/Graphics/ShaderType.h"
#include "Shader.h"

namespace keng::graphics
{
    template<ShaderType shaderType>
    decltype(auto) CreateShader(edt::DenseArrayView<const uint8_t> code, const char* entryPoint, ShaderVersion shaderVersion) {
        return CreateShader<shaderType>((const char*)code.GetData(), entryPoint, shaderVersion);
    }

    template<ShaderType shaderType>
    decltype(auto) CreateShader(std::istream& code, const char* entryPoint, ShaderVersion shaderVersion) {
        return CallAndRethrowM + [&] {
            auto startpos = code.tellg();
            code.seekg(0, std::ios::end);
            auto endpos = code.tellg();
            auto size = endpos - startpos;
            code.seekg(startpos);
            std::string readcode;
            readcode.resize(size);
            code.read(&readcode[0], size);
            return CreateShader<shaderType>(readcode.data(), entryPoint, shaderVersion);
        };
    }
}