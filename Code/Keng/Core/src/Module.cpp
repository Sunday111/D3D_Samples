#include "Module.h"
#include "Keng/Core/ISystem.h"
#include "Keng/Core/IGlobalSystem.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "GlobalEnvironment.h"

namespace keng::core
{
    Module::Module(std::string_view name) {
        CallAndRethrowM + [&]() {
            m_handle = Load(name);
            CreateSystem();
        };
    }

    Module::~Module() {
        m_system = nullptr;
        if (m_handle) {
            FreeLibrary(m_handle);
            m_handle = nullptr;
        }
    }

    const ISystemPtr& Module::GetSystem() const {
        return m_system;
    }

    void Module::Check() const {
        edt::ThrowIfFailed(m_handle != nullptr, "Trying to call Module method when handle is null!");
    }

    HMODULE Module::Load(std::string_view name) {
        return CallAndRethrowM + [&]() {
            auto handle = WinAPI<char>::LoadLibrary_(name.data());
            edt::ThrowIfFailed(handle != nullptr, "Failed to load \"", name, "\"");
            return handle;
        };
    }

    void Module::CreateSystem() {
        return CallAndRethrowM + [&]() {
            auto createSystem = (CreateSystemSignature)GetFunctionAddress("CreateSystem");
            void* ptr = nullptr;
            createSystem(&ptr);
            edt::ThrowIfFailed(ptr != nullptr, "Failed to create system");
            m_system = ISystemPtr(reinterpret_cast<ISystem*>(ptr));
            if (auto globalSystem = std::dynamic_pointer_cast<IGlobalSystem>(m_system)) {
                GlobalEnvironment::PrivateInstance().RegisterSystem(globalSystem);
            }
        };
    }

    void* Module::GetFunctionAddress(std::string_view name) const {
        return CallAndRethrowM + [&]() {
            Check();
            return GetProcAddress(m_handle, name.data());
        };
    }
}
