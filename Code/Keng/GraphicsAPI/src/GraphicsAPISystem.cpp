#include "GraphicsAPISystem.h"

#include "Keng/Base/Serialization/OpenArchiveJSON.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "Keng/FileSystem/ReadFileToBuffer.h"
#include "Keng/WindowSystem/IWindow.h"
#include "Keng/WindowSystem/IWindowSystem.h"

#include "keng/GraphicsAPI/DeviceParameters.h"
#include "keng/GraphicsAPI/Resource/TextureParameters.h"
#include "Resource/Texture/Texture.h"

#include "Keng/GraphicsAPI/RenderTarget/WindowRenderTargetParameters.h"
#include "RenderTarget/WindowRenderTarget.h"
#include "Keng/GraphicsAPI/RenderTarget/TextureRenderTargetParameters.h"
#include "RenderTarget/TextureRenderTarget.h"
#include "Keng/GraphicsAPI/RenderTarget/DepthStencilParameters.h"
#include "RenderTarget/DepthStencil.h"
#include "Keng/GraphicsAPI/RenderTarget/DepthStencilParameters.h"
#include "RenderTarget/DepthStencil.h"
#include "Keng/GraphicsAPI/RenderTarget/SwapChainParameters.h"
#include "RenderTarget/SwapChain.h"
#include "Keng/GraphicsAPI/ViewportParameters.h"

#include "Shader/Shader.h"

#include "Annotation.h"

#include "EverydayTools/Array/ArrayViewVector.h"
#include "EverydayTools/Exception/CheckedCast.h"
#include "EverydayTools/Geom/Vector.h"
#include "DeviceBuffer.h"

#include "Sampler.h"

#include <algorithm>

#include "yasli/JSONIArchive.h"
#include "D3D_11/EnumConverter.h"
#include "Keng/Core/IApplication.h"

namespace keng::graphics_api
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

    GraphicsAPISystem::GraphicsAPISystem() = default;

    GraphicsAPISystem::~GraphicsAPISystem() = default;

    void GraphicsAPISystem::Initialize(const core::IApplicationPtr& app) {
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

    bool GraphicsAPISystem::Update() {
        return true;
    }

    void GraphicsAPISystem::Shutdown() {
    }

    IWindowRenderTargetPtr GraphicsAPISystem::CreateWindowRenderTarget(const WindowRenderTargetParameters& params) {
        return WindowRenderTargetPtr::MakeInstance(*m_device, params);
    }

    ITextureRenderTargetPtr GraphicsAPISystem::CreateTextureRenderTarget(const TextureRenderTargetParameters& params) {
        return TextureRenderTargetPtr::MakeInstance(*m_device, params);
    }

    IDepthStencilPtr GraphicsAPISystem::CreateDepthStencil(const DepthStencilParameters& params) {
        return DepthStencilPtr::MakeInstance(*m_device, params);
    }

    core::Ptr<IDevice> GraphicsAPISystem::GetDevice() {
        return m_device;
    }

    IDeviceBufferPtr GraphicsAPISystem::CreateDeviceBuffer(const DeviceBufferParameters& params, edt::DenseArrayView<const uint8_t> data) {
        return CallAndRethrowM + [&] {
            return DeviceBufferPtr::MakeInstance(*m_device, params, data);
        };
    }

    ISwapChainPtr GraphicsAPISystem::CreateSwapChain(const SwapChainParameters& params) {
        return SwapChainPtr::MakeInstance(*m_device, params);
    }

    ISamplerPtr GraphicsAPISystem::CreateSampler(const SamplerParameters& params) {
        return SamplerPtr::MakeInstance(*m_device, params);
    }

    void GraphicsAPISystem::SetTopology(PrimitiveTopology topo) {
        m_device->GetContext()->IASetPrimitiveTopology(d3d::ConvertTopology(topo));
    }

    void GraphicsAPISystem::Draw(size_t vertices, size_t offset) {
        m_device->GetContext()->Draw(
            edt::CheckedCast<uint32_t>(vertices),
            edt::CheckedCast<uint32_t>(offset));
    }

    void GraphicsAPISystem::SetViewport(const ViewportParameters& p) {
        D3D11_VIEWPORT v[1]{};
        v[0].TopLeftX = p.Position.x();
        v[0].TopLeftY = p.Position.y();
        v[0].Width = p.Size.x();
        v[0].Height = p.Size.y();
        m_device->SetViewports(edt::MakeArrayView(v));
    }

    IAnnotationPtr GraphicsAPISystem::CreateAnnotation() {
        return AnnotationPtr::MakeInstance(*m_device);
    }

    const char* GraphicsAPISystem::GetSystemName() const {
        return SystemName();
    }

    bool GraphicsAPISystem::ForEachDependency(const edt::Delegate<bool(const char*)>& delegate) const {
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
