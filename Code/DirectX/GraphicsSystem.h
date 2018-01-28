#pragma once

#include "System.h"
#include "Renderer.h"

class GraphicsSystem :
    public System,
    public Renderer
{
public:

    GraphicsSystem(Renderer::CreateParams& params) :
        Renderer(params)
    {
    }

    bool Update() override {
        Render();
        return true;
    }

private:
};