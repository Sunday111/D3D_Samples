#include "CompileShader.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "d3dcompiler.h"
#include "WinWrappers/WinWrappers.h"
#include <string_view>
#include <vector>

namespace keng::graphics::gpu
{
    ComPtr<ID3DBlob> CompileShaderToBlob(ShaderType shaderType, ShaderVersion shaderVersion, const ShaderParameters& parameters)
    {
        return CallAndRethrowM + [&] {
            std::string_view code(parameters.code);
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

            std::vector<D3D_SHADER_MACRO> definitions;
            D3D_SHADER_MACRO nullDefinition{};
            const D3D_SHADER_MACRO* definitionsPtr = &nullDefinition;

            auto definitionsCount = parameters.definitions.GetSize();
            if (definitionsCount > 0) {
                definitions.reserve(definitionsCount + 1);
                for (auto& definition : parameters.definitions) {
                    definitions.push_back(D3D_SHADER_MACRO{ definition.name, definition.value });
                }
                definitions.push_back(nullDefinition);
                definitionsPtr = definitions.data();
            }

            WinAPI<char>::HandleError(D3DCompile(
                code.data(),
                code.size(),
                nullptr,                // May be used for debugging
                definitionsPtr,         // Null-terminated array of macro definitions
                nullptr,                // Includes
                parameters.entry_point, // Main function of shader
                shaderTarget,           // shader target
                flags1,                 // Flags for compile constants
                0,                      // Flags for compile effects constants
                shaderBlob.Receive(),   // Output compiled shader
                errorBlob.Receive()     // Compile error messages
            ), onCompileError);
            return shaderBlob;
        };
    }
}
