#pragma once

class System {
public:
    virtual bool Update() = 0;
    virtual ~System() = default;
};