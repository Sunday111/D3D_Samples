#pragma once

#include "ISystem.h"
#include "Renderer.h"
#include "MainWindow.h"
#include "FileResource.h"

class Application;

class GraphicsSystem :
    public ISystem,
    public MainWindow<char>::IObserver
{
public:
    using CreateParams = Renderer::CreateParams;
    GraphicsSystem(Application* app, Renderer::CreateParams& params);
    bool Update(IApplication* app) override;
    void OnWindowResize(int w, int h) override;

    template<ShaderType shaderType>
    decltype(auto) CreateShaderFromFile(const char* fileName, const char* entryPoint, ShaderVersion shaderVersion) {
        return CallAndRethrow("GraphicsSystem::CreateShaderFromFile<>", [&]() {
            auto resourceSystem = m_app->GetResourceSystem();
            auto fileResource = resourceSystem->GetResource<FileResource>(fileName);
            return m_renderer.CreateShader<shaderType>(fileResource->GetDataView(), entryPoint, shaderVersion);
        });
    }

    Renderer& GetRenderer() {
        return m_renderer;
    }

private:
    Application* m_app = nullptr;
    Renderer m_renderer;
    ComPtr<ID3D11Buffer> m_vertices;
    ComPtr<ID3D11InputLayout> m_inputLayout;
    Shader<ShaderType::Vertex> m_vertexShader;
    Shader<ShaderType::Pixel> m_pixelShader;
};