#include "ProjectECS.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include "EverydayTools/Exception/CheckedCast.h"
#include "EverydayTools/Geom/Vector.h"
#include "Keng/Core/IApplication.h"
#include "Keng/Core/SystemEvent.h"
#include "Keng/GPU/PipelineInput/ISampler.h"
#include "Keng/GPU/DeviceBufferMapper.h"
#include "Keng/GPU/RenderTarget/IWindowRenderTarget.h"
#include "Keng/GPU/RenderTarget/ITextureRenderTarget.h"
#include "Keng/GPU/RenderTarget/IDepthStencil.h"
#include "Keng/GPU/ScopedAnnotation.h"
#include "Keng/GraphicsCommon/DepthStencilClearFlags.h"
#include "Keng/GraphicsCommon/DepthStencilParameters.h"
#include "Keng/GraphicsCommon/DeviceBufferParameters.h"
#include "Keng/GraphicsCommon/PrimitiveTopology.h"
#include "Keng/GraphicsCommon/SamplerParameters.h"
#include "Keng/GraphicsCommon/DeviceTextureParameters.h"
#include "Keng/GraphicsCommon/TextureRenderTargetParameters.h"
#include "Keng/GraphicsCommon/ViewportParameters.h"
#include "Keng/GraphicsCommon/WindowRenderTargetParameters.h"
#include "Keng/Graphics/IDevice.h"
#include "Keng/Graphics/Resource/ITexture.h"
#include "Keng/Graphics/Resource/IEffect.h"
#include "Keng/WindowSystem/IWindow.h"
#include "CppReflection/Type.h"

namespace project_ecs
{
    ProjectECS::ProjectECS() = default;

    ProjectECS::~ProjectECS() = default;

    bool ProjectECS::Update() {
        using namespace keng;
        using namespace graphics;
        return CallAndRethrowM + [&] {

			return true;
        };
    }

    void ProjectECS::OnSystemEvent(const keng::core::IApplicationPtr& app, const keng::core::SystemEvent& e) {
        return CallAndRethrowM + [&] {
            switch (e)
            {
            case keng::core::SystemEvent::Initialize:
                Initialize(app);
                break;
            case keng::core::SystemEvent::Update:
                Update();
                break;
            }
        };
    }

    void ProjectECS::Initialize(const keng::core::IApplicationPtr& app) {
        using namespace keng;
        using namespace graphics;
        using namespace resource;
        using namespace window_system;

        CallAndRethrowM + [&] {

        };
    }

}
