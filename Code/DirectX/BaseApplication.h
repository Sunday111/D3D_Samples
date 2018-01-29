#pragma once

#include <vector>
#include <memory>

#include "ISystem.h"
#include "IApplication.h"
#include "EverydayTools\Exception\CallAndRethrow.h"

class BaseApplication :
    public IApplication
{
public:

    void Run() {
        CallAndRethrow("BaseApplication::Run", [&]() {
            while (Update());
        });
    }

    virtual bool Update() {
        return CallAndRethrow("BaseApplication::Update", [&]() {
            bool finished = false;
            for (auto& system : m_systems) {
                if (!system->Update(this)) {
                    finished = true;
                }
            }
            return !finished;
        });
    }

    ISystem* AddSystem(std::unique_ptr<ISystem> system) {
        m_systems.push_back(std::move(system));
        return m_systems.back().get();
    }

private:
    std::vector<std::unique_ptr<ISystem>> m_systems;

};