#pragma once

namespace keng::graphics
{
    class IGraphicsListener
    {
    public:
        virtual void BeforeRendering() = 0;
        virtual void AfterRendering() = 0;
        virtual ~IGraphicsListener() = default;
    };
}