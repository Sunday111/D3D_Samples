#include "GPUSystem.h"

#include "Keng/Base/Serialization/OpenArchiveJSON.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "Keng/FileSystem/ReadFileToBuffer.h"
#include "Keng/WindowSystem/IWindow.h"
#include "Keng/WindowSystem/IWindowSystem.h"

#include "keng/GPU/DeviceParameters.h"
#include "keng/GPU/Resource/TextureParameters.h"
#include "Resource/Texture/Texture.h"

#include "Keng/GPU/RenderTarget/WindowRenderTargetParameters.h"
#include "RenderTarget/WindowRenderTarget.h"
#include "Keng/GPU/RenderTarget/TextureRenderTargetParameters.h"
#include "RenderTarget/TextureRenderTarget.h"
#include "Keng/GPU/RenderTarget/DepthStencilParameters.h"
#include "RenderTarget/DepthStencil.h"
#include "Keng/GPU/RenderTarget/DepthStencilParameters.h"
#include "RenderTarget/DepthStencil.h"
#include "Keng/GPU/RenderTarget/SwapChainParameters.h"
#include "RenderTarget/SwapChain.h"
#include "Keng/GPU/ViewportParameters.h"

#include "Shader/Shader.h"

#include "Annotation.h"

#include "EverydayTools/Array/ArrayViewVector.h"
#include "EverydayTools/Exception/CheckedCast.h"
#include "EverydayTools/Geom/Vector.h"

#include "PiplineInput/Sampler.h"

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

        SystemParams ReadDefaultParams() {
            return CallAndRethrowM + [&] {
                struct ConfigFile
                {
                    void serialize(Archive& ar) { ar(params, "graphics_api_system"); }
                    SystemParams params;
                };

                ConfigFile file;

#ifdef _DEBUG
                file.params.deviceMultithreading = true;
                file.params.debugDevice = true;
#endif

                try {
                    auto filename = "Configs/graphics_api_system.json";
                    using FileView = edt::DenseArrayView<const uint8_t>;
                    auto onFileRead = [&](FileView fileView) {
                        yasli::JSONIArchive ar;
                        OpenArchiveJSON(fileView, ar);
                        ar(file);
                    };
                    edt::Delegate<void(FileView)> delegate;
                    delegate.Bind(onFileRead);
                    filesystem::HandleFileData(filename, delegate);
                } catch (...) {
                }

                return file.params;
            };
        }
    }

    GPUSystem::GPUSystem() = default;

    GPUSystem::~GPUSystem() = default;

    void GPUSystem::Initialize(const core::IApplicationPtr& app) {
        CallAndRethrowM + [&] {
            m_app = app;

            auto params = ReadDefaultParams();

            {// Initialize device
                DeviceParameters deviceParams;
                deviceParams.debugDevice = params.debugDevice;
                deviceParams.noDeviceMultithreading = !params.deviceMultithreading;
                m_device = DevicePtr::MakeInstance(deviceParams);
            }
        };
    }

    bool GPUSystem::Update() {
        return true;
    }

    void GPUSystem::Shutdown() {
    }

    IWindowRenderTargetPtr GPUSystem::CreateWindowRenderTarget(const WindowRenderTargetParameters& params) {
        return WindowRenderTargetPtr::MakeInstance(*m_device, params);
    }

    ITextureRenderTargetPtr GPUSystem::CreateTextureRenderTarget(const TextureRenderTargetParameters& params) {
        return TextureRenderTargetPtr::MakeInstance(*m_device, params);
    }

    IDepthStencilPtr GPUSystem::CreateDepthStencil(const DepthStencilParameters& params) {
        return DepthStencilPtr::MakeInstance(*m_device, params);
    }

    core::Ptr<IDevice> GPUSystem::GetDevice() {
        return m_device;
    }

    ISwapChainPtr GPUSystem::CreateSwapChain(const SwapChainParameters& params) {
        return SwapChainPtr::MakeInstance(*m_device, params);
    }

    ISamplerPtr GPUSystem::CreateSampler(const SamplerParameters& params) {
        return SamplerPtr::MakeInstance(*m_device, params);
    }

    void GPUSystem::Draw(size_t vertices, size_t offset) {
        m_device->GetContext()->Draw(
            edt::CheckedCast<uint32_t>(vertices),
            edt::CheckedCast<uint32_t>(offset));
    }

    void GPUSystem::SetViewport(const ViewportParameters& p) {
        D3D11_VIEWPORT v[1]{};
        v[0].TopLeftX = p.Position.x();
        v[0].TopLeftY = p.Position.y();
        v[0].Width = p.Size.x();
        v[0].Height = p.Size.y();
        m_device->SetViewports(edt::MakeArrayView(v));
    }

    IAnnotationPtr GPUSystem::CreateAnnotation() {
        return AnnotationPtr::MakeInstance(*m_device);
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
