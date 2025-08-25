//
// ECS.cpp - ECS工厂和查询类实现
//

#include "JFMEngine/ECS/ECS.h"
#include "JFMEngine/Utils/Log.h"

namespace JFM {
    namespace ECS {

        // ========== ECSFactory 实现 ==========

        Entity ECSFactory::CreateCube(World* world, const std::string& name) {
            Entity entity = world->CreateEntity();

            world->AddComponent<TagComponent>(entity, name);
            world->AddComponent<TransformComponent>(entity);
            world->AddComponent<ActiveComponent>(entity, true);

            // TODO: 创建立方体网格和默认材质
            // auto& meshRenderer = world->AddComponent<MeshRendererComponent>(entity);
            // meshRenderer.MeshData = MeshLibrary::GetCubeMesh();
            // meshRenderer.MaterialData = MaterialLibrary::GetDefaultMaterial();

            return entity;
        }

        Entity ECSFactory::CreateSphere(World* world, const std::string& name) {
            Entity entity = world->CreateEntity();

            world->AddComponent<TagComponent>(entity, name);
            world->AddComponent<TransformComponent>(entity);
            world->AddComponent<ActiveComponent>(entity, true);

            // TODO: 创建球体网格和默认材质
            // auto& meshRenderer = world->AddComponent<MeshRendererComponent>(entity);
            // meshRenderer.MeshData = MeshLibrary::GetSphereMesh();
            // meshRenderer.MaterialData = MaterialLibrary::GetDefaultMaterial();

            return entity;
        }

        Entity ECSFactory::CreatePlane(World* world, const std::string& name) {
            Entity entity = world->CreateEntity();

            world->AddComponent<TagComponent>(entity, name);
            world->AddComponent<TransformComponent>(entity);
            world->AddComponent<ActiveComponent>(entity, true);

            // TODO: 创建平面网格和默认材质
            // auto& meshRenderer = world->AddComponent<MeshRendererComponent>(entity);
            // meshRenderer.MeshData = MeshLibrary::GetPlaneMesh();
            // meshRenderer.MaterialData = MaterialLibrary::GetDefaultMaterial();

            return entity;
        }

        Entity ECSFactory::CreateDirectionalLight(World* world, const glm::vec3& direction) {
            Entity entity = world->CreateEntity();

            world->AddComponent<TagComponent>(entity, "Directional Light");
            auto& transform = world->AddComponent<TransformComponent>(entity);
            world->AddComponent<ActiveComponent>(entity, true);

            // 设置光源组件
            auto& light = world->AddComponent<LightComponent>(entity);
            light.Type = LightType::Directional;
            light.Color = glm::vec3(1.0f, 1.0f, 1.0f);
            light.Intensity = 1.0f;

            // 设置方向
            glm::vec3 normalizedDir = glm::normalize(direction);
            transform.Rotation = glm::quatLookAt(normalizedDir, glm::vec3(0, 1, 0));

            return entity;
        }

        Entity ECSFactory::CreatePointLight(World* world, const glm::vec3& position) {
            Entity entity = world->CreateEntity();

            world->AddComponent<TagComponent>(entity, "Point Light");
            auto& transform = world->AddComponent<TransformComponent>(entity);
            world->AddComponent<ActiveComponent>(entity, true);

            // 设置光源组件
            auto& light = world->AddComponent<LightComponent>(entity);
            light.Type = LightType::Point;
            light.Color = glm::vec3(1.0f, 1.0f, 1.0f);
            light.Intensity = 1.0f;
            light.Range = 10.0f;

            // 设置位置
            transform.Position = position;

            return entity;
        }

        Entity ECSFactory::CreateSpotLight(World* world, const glm::vec3& position) {
            Entity entity = world->CreateEntity();

            world->AddComponent<TagComponent>(entity, "Spot Light");
            auto& transform = world->AddComponent<TransformComponent>(entity);
            world->AddComponent<ActiveComponent>(entity, true);

            // 设置光源组件
            auto& light = world->AddComponent<LightComponent>(entity);
            light.Type = LightType::Spot;
            light.Color = glm::vec3(1.0f, 1.0f, 1.0f);
            light.Intensity = 1.0f;
            light.Range = 10.0f;
            light.InnerConeAngle = 30.0f;
            light.OuterConeAngle = 45.0f;

            // 设置位置
            transform.Position = position;

            return entity;
        }

        Entity ECSFactory::CreatePerspectiveCamera(World* world, float fov) {
            Entity entity = world->CreateEntity();

            world->AddComponent<TagComponent>(entity, "Camera");
            world->AddComponent<TransformComponent>(entity);
            world->AddComponent<ActiveComponent>(entity, true);

            // 设置相机组件
            auto& camera = world->AddComponent<CameraComponent>(entity);
            camera.Type = CameraComponent::ProjectionType::Perspective;
            camera.FOV = fov;
            camera.NearPlane = 0.1f;
            camera.FarPlane = 1000.0f;
            camera.IsPrimary = false; // 需要手动设置为主相机

            return entity;
        }

        Entity ECSFactory::CreateOrthographicCamera(World* world, float size) {
            Entity entity = world->CreateEntity();

            world->AddComponent<TagComponent>(entity, "Orthographic Camera");
            world->AddComponent<TransformComponent>(entity);
            world->AddComponent<ActiveComponent>(entity, true);

            // 设置相机组件
            auto& camera = world->AddComponent<CameraComponent>(entity);
            camera.Type = CameraComponent::ProjectionType::Orthographic;
            camera.OrthographicSize = size;
            camera.NearPlane = 0.1f;
            camera.FarPlane = 1000.0f;
            camera.IsPrimary = false; // 需要手动设置为主相机

            return entity;
        }

        Entity ECSFactory::CreateEmpty(World* world, const std::string& name) {
            Entity entity = world->CreateEntity();

            world->AddComponent<TagComponent>(entity, name);
            world->AddComponent<TransformComponent>(entity);
            world->AddComponent<ActiveComponent>(entity, true);

            return entity;
        }

        // ========== ECSQuery 实现 ==========

        std::vector<Entity> ECSQuery::Execute() const {
            if (!m_World) {
                return {};
            }

            std::vector<Entity> allEntities = m_World->GetEntitiesWithComponents(m_RequiredMask);

            if (m_ExcludedMask == 0) {
                return allEntities;
            }

            // 过滤掉包含排除组件的实体
            std::vector<Entity> filteredEntities;
            for (Entity entity : allEntities) {
                ComponentMask entityMask = m_World->GetEntityComponentMask(entity);
                if ((entityMask & m_ExcludedMask) == 0) {
                    filteredEntities.push_back(entity);
                }
            }

            return filteredEntities;
        }

    } // namespace ECS
} // namespace JFM
