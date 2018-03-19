#include "Keng/Systems/ResourceSystem/ResourceSystem.h"
#include "Keng/Systems/WindowSystem/WindowSystem.h"
#include "Keng/Systems/GraphicsSystem/GraphicsSystem.h"

namespace keng
{
    const char* ResourceSystem::GetGUID() {
        return "8BA11029-9DE9-473C-925A-5FD0D7B36141";
    }

    void ResourceSystem::Initialize(IApplication* app)
    {
        UnusedVar(app);
        m_parameters = ReadDefaultParams();
    }

    bool ResourceSystem::Update() {
        return CallAndRethrowM + [&] {
            using namespace std::chrono;
            using namespace std::chrono_literals;
            high_resolution_clock::time_point zero(0ms);
            auto dt = high_resolution_clock::now() - zero;
            auto now = duration_cast<milliseconds>(dt).count();
            float nowMs = static_cast<float>(now);

            auto iResourceInfo = m_resources.begin();
            while (iResourceInfo != m_resources.end()) {
                if (iResourceInfo->second.ShouldBeReleased(nowMs)) {
                    iResourceInfo = m_resources.erase(iResourceInfo);
                }
                else {
                    ++iResourceInfo;
                }
            }

            return true;
        };
    }

    IntrusivePtr<IResource> ResourceSystem::GetResource(std::string_view filename) {
        return CallAndRethrowM + [&] {
            std::string filename_copy(filename);
            auto resource_it = m_resources.find(filename_copy);
            if (resource_it != m_resources.end()) {
                return resource_it->second.resource;
            }

            auto doc = IntrusivePtr<XmlDocument>::MakeInstance(filename);
            auto resource_node = doc->GetFirstNode("resource");
            auto typeNode = resource_node->GetFirstNode("type");
            auto resourceTypeName = std::string(typeNode->GetValue());
            auto fabric_it = m_fabrics.find(resourceTypeName);

            edt::ThrowIfFailed(
                fabric_it != m_fabrics.end(),
                "Fabric for resource \"", resourceTypeName, "\" is not registered");

            auto exactNode = resource_node->GetFirstNode(fabric_it->second->GetNodeName());
            auto result = fabric_it->second->LoadResource(this, exactNode);

            ResourceInfo info;
            info.resource = result;
            m_resources.insert(std::make_pair(std::move(filename_copy), std::move(info)));
            return result;
        };
    }

    void ResourceSystem::RegisterResourceFabric(IntrusivePtr<IResourceFabric> fabric) {
        CallAndRethrowM + [&] {
            auto resourceTypeName = fabric->GetResourceType();
            auto res = m_fabrics.insert(std::make_pair(std::string(resourceTypeName), fabric));
            edt::ThrowIfFailed(res.second, "Fabric \"", resourceTypeName, "\" already registered");
        };
    }

    void ResourceSystem::UnregisterFabric(IntrusivePtr<IResourceFabric> fabric) {
        CallAndRethrowM + [&] {
            auto resourceTypeName = fabric->GetResourceType();
            auto eraseCount = m_fabrics.erase(std::string(resourceTypeName));
            edt::ThrowIfFailed(eraseCount < 1, "Fabric \"", resourceTypeName, "\" not found");
        };
    }

    ResourceSystem::SystemParams ResourceSystem::ReadDefaultParams() {
        return CallAndRethrowM + [&] {
            SystemParams result;

            try {
                XmlDocument configDoc("Configs/resource_system.xml");
                auto resourceSystemNode = configDoc.GetFirstNode("resource_system");
                if (auto resourceNode = resourceSystemNode->FindFirstNode("resource")) {
                    if (auto releaseDelayNode = resourceNode->FindFirstNode("release_delay")) {
                        auto releaseDelayText = releaseDelayNode->GetValue();
                        result.defaultResourceParams.releaseDelay = std::stof(releaseDelayText.data());
                    }
                }
            }
            catch (...)
            { }

            return result;
        };
    }
}