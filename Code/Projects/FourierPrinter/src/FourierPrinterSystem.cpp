﻿#include "FourierPrinterSystem.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include "EverydayTools/Exception/CheckedCast.h"
#include "Keng/Core/IApplication.h"
#include "Keng/Core/SystemEvent.h"
#include "Keng/GPU/DeviceBufferMapper.h"
#include "Keng/GPU/ScopedAnnotation.h"
#include "Keng/GPU/RenderTarget/IWindowRenderTarget.h"
#include "Keng/GPU/RenderTarget/ITextureRenderTarget.h"
#include "Keng/GPU/RenderTarget/IDepthStencil.h"
#include "Keng/GraphicsCommon/DepthStencilClearFlags.h"
#include "Keng/GraphicsCommon/DepthStencilParameters.h"
#include "Keng/GraphicsCommon/DeviceBufferParameters.h"
#include "Keng/GraphicsCommon/PrimitiveTopology.h"
#include "Keng/GraphicsCommon/DeviceTextureParameters.h"
#include "Keng/GraphicsCommon/TextureRenderTargetParameters.h"
#include "Keng/GraphicsCommon/ViewportParameters.h"
#include "Keng/GraphicsCommon/WindowRenderTargetParameters.h"
#include "Keng/Graphics/IDevice.h"
#include "Keng/Graphics/IGraphicsSystem.h"
#include "Keng/Graphics/Resource/ITexture.h"
#include "Keng/Graphics/Resource/IEffect.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/WindowSystem/IWindowSystem.h"
#include "Keng/WindowSystem/IWindow.h"

#include "FourierSeries.h"
#include "RangedFunction.h"
#include "SampledFunction.h"
#include "WaveRangedFunction.h"
#include "EverydayTools/Functional/Wrap.h"

namespace simple_quad_sample
{
    namespace
    {
        class FromFunctionSettings
        {
        public:
            static constexpr size_t samplesCount = 1024;
            static constexpr size_t sampleRate = 44100;
            using Sample = int32_t;

            static float TargetFunction(float x) {
                auto k = std::numeric_limits<Sample>::max() - 10;
                auto val = std::sin(x);
                return val * k;
            };
        };

        class Settings : public FromFunctionSettings
        {
        public:
            static constexpr bool SignalFromFile = true;
            static constexpr auto TargetFile = "D:\\untitled.wav";
            static constexpr auto OutputFile = "D:\\untitled_restored.wav";
            static constexpr bool InterpolateSampledFunction = false;
            static constexpr size_t CoefficientsCount = 256;
            static constexpr size_t DrawingSamplesCount = 1024;
            static constexpr size_t IntegrationPrecision = 44100;
        };

        template<typename T>
        edt::geom::Matrix<T, 4, 4> MakeTranslationMatrix(edt::geom::Vector<T, 3> vec) {
            auto m = edt::geom::Matrix<float, 4, 4>::Identity();

            m.At(3, 0) = vec.Elem(0);
            m.At(3, 1) = vec.Elem(1);
            m.At(3, 2) = vec.Elem(2);

            return m;
        }

        template<typename T>
        edt::geom::Matrix<T, 4, 4> MakeScaleMatrix(edt::geom::Vector<T, 3> scale) {
            auto m = edt::geom::Matrix<float, 4, 4>::Identity();

            m.At(0, 0) = scale.Elem(0);
            m.At(1, 1) = scale.Elem(1);
            m.At(2, 2) = scale.Elem(2);

            return m;
        }

        WaveRangedFunction<float, true> GetTargetFunction(float argumentBegin, float argumentRange) {
            return CallAndRethrowM + [&] {
                std::shared_ptr<WaveBuffer> soundBuffer;

                if constexpr (Settings::SignalFromFile) {
                    soundBuffer = std::make_shared<WaveBuffer>(Settings::TargetFile);
                } else {
                    soundBuffer = std::make_shared<WaveBuffer>(MakeWaveBuffer<float, typename Settings::Sample>(
                        1, Settings::sampleRate, argumentBegin, argumentRange, Settings::samplesCount, Settings::TargetFunction));
                }

                edt::ThrowIfFailed(soundBuffer->GetChannelsCount() == 1, "Only mono sounds supported");
                WaveRangedFunction<float, true> fn(argumentBegin, argumentBegin + argumentRange, soundBuffer);
                return fn;
            };
        }

        template<typename T>
        std::function<T(T)> WrapClamped(T min, T max, const std::function<T(T)>& fn) {
            return edt::WrapFunction(fn, [min, max](T val) {
                val = std::min(max, val);
                val = std::max(min, val);
                return val;
            });
        }
    }

    FourierPrinterSystem::FourierPrinterSystem() = default;

    FourierPrinterSystem::~FourierPrinterSystem() = default;

    bool FourierPrinterSystem::Update() {
        using namespace keng;
        using namespace graphics;

        return CallAndRethrowM + [&] {
            return Annotate(m_annotation, L"Frame", [&] {
                auto api_device = GetSystem<IGraphicsSystem>().GetDevice()->GetApiDevice();

                float clearColor[4]{
                    0.0f, 0.2f, 0.4f, 1.0f
                };

                static float angle = 0.f;
                constexpr float delta_angle = 0.001f;
                angle += delta_angle;

                m_textureRT->Clear(clearColor);
                m_depthStencil->Clear(DepthStencilClearFlags::ClearDepth | DepthStencilClearFlags::ClearStencil, 1.0f, 0);
                m_textureRT->AssignToPipeline(m_depthStencil);

                for (auto& model : m_models) {
                    //Annotate(m_annotation, L"Update model", [&] {
                    //    gpu::DeviceBufferMapper mapper;
                    //    model.constantBuffer->MakeMapper(mapper);
                    //    auto cbView = mapper.GetTypedView<SimpleModel::CB>();
                    //    auto& cb = cbView[0];
                    //    v3f scale {};
                    //    scale.Fill(1.0f / (pi<float> * 2.0f));
                    //    v3f translation { std::sin(angle), 0.f, 0.0f };
                    //    cb.transform = MakeScaleMatrix(scale) * MakeTranslationMatrix(translation);
                    //});

                    Annotate(m_annotation, L"Draw model", [&] {
                        gpu::VertexBufferAssignParameters vbAssignParams {};
                        vbAssignParams.slot = 0;
                        vbAssignParams.stride = sizeof(SimpleModel::Vertex);
                        
                        gpu::ConstantBufferAssignParameters cbAssignParams {};
                        cbAssignParams.slot = 0;
                        cbAssignParams.stride = sizeof(SimpleModel::CB);
                        cbAssignParams.shaderType = ShaderType::Vertex;
                        
                        model.effect->AssignToPipeline();
                        model.vertices->AssignToPipeline(vbAssignParams);
                        api_device->SetTopology(model.topology);
                        model.constantBuffer->AssignToPipeline(cbAssignParams);
                        api_device->Draw(model.elementsCount, 0);
                    });
                }

                Annotate(m_annotation, L"Copy texture to swap chain texture", [&] {
                    m_windowRT->CopyFrom(m_textureRT->GetTexture());
                });

                m_windowRT->Present();

                return true;
            });
        };
    }

    void FourierPrinterSystem::OnSystemEvent(const keng::core::IApplicationPtr& app, const keng::core::SystemEvent& e) {
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

    void FourierPrinterSystem::MakeFunctionModel(const std::function<float(float)>& function, const SimpleModel::CB& cb,
        float argumentBegin, float argumentRange, size_t samplesCount, const v4f& color)
    {
        using namespace keng;
        using namespace graphics;
        using namespace resource;
        using namespace window_system;
        
        CallAndRethrowM + [&] {
            SimpleModel m;

            // Generate vertices for sampled function drawing
            std::vector<SimpleModel::Vertex> vertices;
            const float dx = argumentRange / samplesCount;

            vertices.push_back(
                SimpleModel::Vertex{
                    { pi<float>,    0,    0.0f, 1.0f },
                    { 1.0f, 0.0f, 1.0f, 1.0f }
                }
            );

            vertices.push_back(
                SimpleModel::Vertex{
                    { -pi<float>,    0,    0.0f, 1.0f },
                    { 1.0f, 0.0f, 1.0f, 1.0f }
                }
            );

            for (size_t sampleIndex = 0; sampleIndex < samplesCount; ++sampleIndex) {
                auto x = argumentBegin + dx * sampleIndex;
                auto y = function(x);
                
                SimpleModel::Vertex vertex {
                    { x,    y,    0.0f, 1.0f },
                    color
                };
                vertices.push_back(vertex);
            }

            auto resource_device = GetSystem<IGraphicsSystem>().GetDevice();
            auto api_device = resource_device->GetApiDevice();
            
            {// Read and compile shaders
                std::string_view effectName = "Assets/Effects/FlatColor.json";
                m.effect = std::static_pointer_cast<IEffect>(GetSystem<IResourceSystem>().GetResource(effectName.data(), resource_device));
                m.effect->InitDefaultInputLayout();
            }
            
            {// Device vertex buffer for sampled function lines
                DeviceBufferParameters params {};
                params.size = vertices.size() * sizeof(SimpleModel::Vertex);
                params.usage = DeviceBufferUsage::Dynamic;
                params.bindFlags = DeviceBufferBindFlags::VertexBuffer;
                params.accessFlags = CpuAccessFlags::Write;
                m.vertices = api_device->CreateDeviceBuffer(params, edt::DenseArrayView<uint8_t>((uint8_t*)vertices.data(), params.size));
                m.topology = PrimitiveTopology::LineStrip;
                m.elementsCount = vertices.size();
            }
            
            {// Device constant buffer for sampled function lines
                DeviceBufferParameters params {};
                params.size = sizeof(cb);
                params.usage = DeviceBufferUsage::Dynamic;
                params.bindFlags = DeviceBufferBindFlags::ConstantBuffer;
                params.accessFlags = CpuAccessFlags::Write;
                m.constantBuffer = api_device->CreateDeviceBuffer(params, edt::DenseArrayView<uint8_t>((uint8_t*)&cb, sizeof(cb)));
            }
            
            m.topology = graphics::PrimitiveTopology::LineStrip;
            m_models.push_back(std::move(m));
        };
    }

    void FourierPrinterSystem::Initialize(const keng::core::IApplicationPtr& app) {
        using namespace keng;
        using namespace graphics;
        using namespace resource;
        using namespace window_system;

        CallAndRethrowM + [&] {
            StoreDependencies(*app);
            auto resource_device = GetSystem<IGraphicsSystem>().GetDevice();
            auto api_device = resource_device->GetApiDevice();
            m_annotation = api_device->CreateAnnotation();

            auto window = GetSystem<IWindowSystem>().GetWindow();
            size_t w, h;
            window->GetClientSize(&w, &h);

            {// Initialize viewport
                ViewportParameters v{};
                v.Position.rx() = 0.f;
                v.Position.ry() = 0.f;
                v.Size.rx() = edt::CheckedCast<float>(w);
                v.Size.ry() = edt::CheckedCast<float>(h);
                api_device->SetViewport(v);
            }

            {// Create window render target
                WindowRenderTargetParameters window_rt_params;
                window_rt_params.swapChain.format = FragmentFormat::R8_G8_B8_A8_UNORM;
                window_rt_params.swapChain.buffers = 2;
                m_windowRT = api_device->CreateWindowRenderTarget(window_rt_params, *window);
            }

            {// Create texture render target
                TextureRenderTargetParameters texture_rt_params{};
                DeviceTextureParameters rtTextureParams{};
                rtTextureParams.format = FragmentFormat::R8_G8_B8_A8_UNORM;
                rtTextureParams.width = w;
                rtTextureParams.height = h;
                rtTextureParams.bindFlags = DeviceBufferBindFlags::ShaderResource | DeviceBufferBindFlags::RenderTarget;
                auto renderTragetTexture = GetSystem<IGraphicsSystem>().CreateTexture(rtTextureParams)->GetApiTexture();
                m_textureRT = api_device->CreateTextureRenderTarget(texture_rt_params, *renderTragetTexture);
            }

            {// Create depth stencil
                DepthStencilParameters depthStencilParams{};
                DeviceTextureParameters dsTextureParams{};
                dsTextureParams.format = FragmentFormat::R24_G8_TYPELESS;
                dsTextureParams.width = w;
                dsTextureParams.height = h;
                dsTextureParams.bindFlags = DeviceBufferBindFlags::ShaderResource | DeviceBufferBindFlags::DepthStencil;

                depthStencilParams.format = FragmentFormat::D24_UNORM_S8_UINT;
                auto depthStencilTexture = GetSystem<IGraphicsSystem>().CreateTexture(dsTextureParams)->GetApiTexture();
                m_depthStencil = api_device->CreateDepthStencil(depthStencilParams, *depthStencilTexture);
            }

            const float signalArgumentBegin = -pi<float>;
            const float signalArgumentRange = 2 * pi<float>;

            {
                SimpleModel sampledFunctionModel;

                {// Read and compile shaders
                    std::string_view effectName = "Assets/Effects/FlatColor.json";
                    sampledFunctionModel.effect = std::static_pointer_cast<IEffect>(GetSystem<IResourceSystem>().GetResource(effectName.data(), resource_device));
                    sampledFunctionModel.effect->InitDefaultInputLayout();
                }

                const size_t coefficientsCount = Settings::CoefficientsCount;
                const size_t drawingSamplesCount = Settings::DrawingSamplesCount;
                const size_t integrationPrecision = Settings::IntegrationPrecision;

                std::vector<float> an, bn;
                auto signalFunction = GetTargetFunction(signalArgumentBegin, signalArgumentRange);
                auto a0 = ComputeFourierSeriesCoefficientA0<float>(integrationPrecision, signalFunction);
                ComputeFourierSeriesCoefficientsA<float>(coefficientsCount, integrationPrecision, std::back_inserter(an), signalFunction);
                ComputeFourierSeriesCoefficientsB<float>(coefficientsCount, integrationPrecision, std::back_inserter(bn), signalFunction);

                {// Draw restored function components
                    auto minCoeffiecient = std::min(*std::min_element(an.begin(), an.end()), *std::min_element(bn.begin(), bn.end()));
                    auto maxCoeffiecient = std::max(*std::max_element(an.begin(), an.end()), *std::max_element(bn.begin(), bn.end()));
                    auto coefficientsRange = maxCoeffiecient - minCoeffiecient;
                    auto makeConstantBuffer = [&]() {
                        SimpleModel::CB cb;
                        edt::geom::Vector<float, 3> scale {
                            1.0f / (pi<float> * 2.0f),
                            1.0f / coefficientsRange,
                            1.0f
                        };
                        edt::geom::Vector<float, 3> translation { +0.5f, +0.5f, 0.0f };
                        cb.transform = MakeScaleMatrix(scale) * MakeTranslationMatrix(translation);
                        return cb;
                    };

                    // Prepare constant buffer which will be the same for all components
                    const auto cb = makeConstantBuffer();

                    const v4f sinColor { 0.0f, 1.0f, 0.0f, 0.2f };
                    const v4f cosColor { 1.0f, 0.0f, 0.0f, 0.2f };

                    auto makeComponentModel = [&](std::vector<float>& coefficients , size_t n, v4f color, float(*realFunction)(float)) {
                        float coefficient = coefficients[n];
                        if (std::abs(coefficient) < maxCoeffiecient * 0.05f) {
                            return;
                        }
                        auto function =  [coefficient, realFunction, n, a0](float arg) {
                            return a0 + coefficient * realFunction(arg * n);
                        };
                        MakeFunctionModel(function, cb, signalArgumentBegin, signalArgumentRange, drawingSamplesCount, color);
                    };

                    for (size_t coefficientIndex = 0; coefficientIndex < coefficientsCount; ++coefficientIndex) {
                        makeComponentModel(an, coefficientIndex, sinColor, std::sin);
                        makeComponentModel(bn, coefficientIndex, cosColor, std::cos);
                    }
                }

                // Make restored function
                auto rawRestoredFunction = RestoreFromFourierCoefficients(coefficientsCount, std::move(an), std::move(bn), a0);
                auto restoredFunction = WrapClamped(
                    static_cast<float>(std::numeric_limits<Settings::Sample>::lowest()),
                    static_cast<float>(std::numeric_limits<Settings::Sample>::max()), rawRestoredFunction);

                auto computeRange = [&](auto& function) {
                    auto min = std::numeric_limits<float>::max();
                    auto max = std::numeric_limits<float>::lowest();
                    for (size_t sampleIndex = 0; sampleIndex < drawingSamplesCount; ++sampleIndex) {
                        auto arg = signalArgumentBegin + sampleIndex * signalArgumentRange / drawingSamplesCount;
                        auto value = function(arg);
                        min = std::min(min, value);
                        max = std::max(max, value);
                    }

                    return max - min;
                };

                {// Draw sampled and restored functions
                    const v4f sampledColor {1.0f, 0.0f, 0.0f, 1.0f};
                    SimpleModel::CB cb;
                    edt::geom::Vector<float, 3> scale {
                        0.9f / (pi<float> * 2.0f),
                        0.9f / computeRange(signalFunction),
                        1.0f
                    };
                    edt::geom::Vector<float, 3> translation { -0.5f, +0.5f, 0.0f };
                    cb.transform = MakeScaleMatrix(scale) * MakeTranslationMatrix(translation);
                    MakeFunctionModel(signalFunction, cb, signalArgumentBegin, signalArgumentRange, drawingSamplesCount, sampledColor);

                    const v4f restoredColor {1.0f, 1.0f, 0.0f, 1.0f};
                    translation.ry() += 0.01f;
                    scale = edt::geom::Vector<float, 3> {
                        0.9f / (pi<float> * 2.0f),
                        0.9f / computeRange(restoredFunction),
                        1.0f
                    };
                    cb.transform = MakeScaleMatrix(scale) * MakeTranslationMatrix(translation);
                    MakeFunctionModel(restoredFunction, cb, signalArgumentBegin, signalArgumentRange, drawingSamplesCount, restoredColor);
                }

                // Save
                {
                    auto waveBuffer = MakeWaveBuffer<float, typename Settings::Sample>(1, signalFunction.GetWaveBuffer().GetSampleRate(),
                        signalArgumentBegin, signalArgumentRange, signalFunction.GetSamplesCount(), restoredFunction);
                    waveBuffer.SaveToFile(Settings::OutputFile);
                }
            }
        };
    }
}
