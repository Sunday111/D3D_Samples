#pragma once

#include <vector>
#include <memory>

#include "System.h"
#include "EverydayTools\Exception\CallAndRethrow.h"

class BaseApplication {
public:

    void Run() {
        CallAndRethrow("BaseApplication::Run", [&]() {
            while (Update());
        });
    }

    bool Update() {
        return CallAndRethrow("BaseApplication::Update", [&]() {
            bool finished = false;
            for (auto& system : m_systems) {
                if (!system->Update()) {
                    finished = true;
                }
            }
            return !finished;
        });
    }

    void AddSystem(std::unique_ptr<System> system) {
        m_systems.push_back(std::move(system));
    }

private:
    std::vector<std::unique_ptr<System>> m_systems;

};