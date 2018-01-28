#pragma once

#include "ISystem.h"
#include "Renderer.h"
#include "MainWindow.h"

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

private:
    Renderer m_renderer;
    D3DPtr<ID3D11Buffer> m_vertices;
    D3DPtr<ID3D11InputLayout> m_inputLayout;
    Shader<ShaderType::Vertex> m_vertexShader;
    Shader<ShaderType::Pixel> m_pixelShader;
};