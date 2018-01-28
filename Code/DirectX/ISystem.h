#pragma once

class IApplication;

class ISystem {
public:
    virtual bool Update(IApplication*) = 0;
    virtual ~ISystem() = default;
};