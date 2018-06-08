#include "GPUSystem.h"

#include "Keng/Base/Serialization/OpenArchiveJSON.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "Keng/FileSystem/ReadFileToBuffer.h"
#include "Keng/WindowSystem/IWindow.h"
#include "Keng/WindowSystem/IWindowSystem.h"

#include "keng/GPU/DeviceParameters.h"
#include "keng/GPU/Resource/TextureParameters.h"
#include "Resource/Texture/Texture.h"

#include "Keng/GPU/RenderTarget/SwapChainParameters.h"
#include "RenderTarget/SwapChain.h"
#include "Keng/GPU/ViewportParameters.h"

#include "Shader/Shader.h"

#include "Annotation.h"

#include "EverydayTools/Array/ArrayViewVector.h"
#include "EverydayTools/Exception/CheckedCast.h"
#include "EverydayTools/Geom/Vector.h"

#include <algorithm>

#include "yasli/JSONIArchive.h"
#include "EnumConverter.h"
#include "Keng/Core/IApplication.h"

namespace keng::gpu
{
    namespace
    {
        struct Vertex
        {
            edt::geom::Vector<float, 3> pos;
            edt::geom::Vector<float, 4> col;
            edt::geom::Vector<float, 2> tex;
        };

        struct SystemParams
        {
            void serialize(Archive& ar) {
                ar(debugDevice, "debug_device");
                ar(deviceMultithreading, "device_multithreading");
            }
            bool debugDevice = false;
            bool deviceMultithreading = false;
        };
    }

    GPUSystem::GPUSystem() = default;

    GPUSystem::~GPUSystem() = default;

    void GPUSystem::Initialize(const core::IApplicationPtr& app) {
        UnusedVar(app);
    }

    bool GPUSystem::Update() {
        return true;
    }

    void GPUSystem::Shutdown() {
    }

    core::Ptr<IDevice> GPUSystem::CreateDevice(const DeviceParameters& parameters)
    {
        return DevicePtr::MakeInstance(parameters);
    }

    const char* GPUSystem::GetSystemName() const {
        return SystemName();
    }

    bool GPUSystem::ForEachDependency(const edt::Delegate<bool(const char*)>& delegate) const {
        return CallAndRethrowM + [&]() -> bool {
            std::string_view dependencies[] =
            {
                window_system::IWindowSystem::SystemName()
            };

            for (auto& systemName : dependencies) {
                if (delegate.Invoke(systemName.data())) {
                    return true;
                }
            }

            return false;
        };
    }
}
