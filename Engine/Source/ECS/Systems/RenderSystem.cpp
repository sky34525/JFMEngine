//
// RenderSystem.cpp - 渲染系统实现（集成Renderer3D）
//

#include "JFMEngine/ECS/Systems/RenderSystem.h"
#include "JFMEngine/ECS/World.h"
#include "JFMEngine/Renderer/Renderer3D.h"
#include "JFMEngine/Renderer/Light.h"
#include "JFMEngine/Renderer/Model.h"
#include "JFMEngine/Renderer/Mesh.h"
#include "JFMEngine/Renderer/RenderCommand.h"
#include "JFMEngine/Utils/Log.h"
#include <algorithm>

namespace JFM {
    namespace ECS {

        void RenderSystem::Update(float deltaTime) {
            (void)deltaTime; // 避免未使用参数警告

            // 查找活跃相机
            auto cameraEntities = GetEntities<TransformComponent, CameraComponent>();
            m_ActiveCamera = Entity(NULL_ENTITY);

            for (Entity entity : cameraEntities) {
                const auto& camera = m_World->GetComponent<CameraComponent>(entity);
                if (camera.IsPrimary) {
                    m_ActiveCamera = entity;
                    break;
                }
            }

            // 如果没有主相机，使用第一个找到的相机
            if (m_ActiveCamera == Entity(NULL_ENTITY) && !cameraEntities.empty()) {
                m_ActiveCamera = cameraEntities[0];
            }
        }

        void RenderSystem::Render() {
            if (m_ActiveCamera == Entity(NULL_ENTITY)) {
                return;
            }

            // 收集渲染对象
            CollectRenderables();

            // 收集光源
            auto lights = CollectLights();

            // 设置相机并开始场景渲染
            const auto& cameraTransform = m_World->GetComponent<TransformComponent>(m_ActiveCamera);
            const auto& cameraComponent = m_World->GetComponent<CameraComponent>(m_ActiveCamera);

            // 创建相机对象
            Camera camera = CreateCameraFromComponent(cameraTransform, cameraComponent);

            // 开始3D渲染
            Renderer3D::BeginScene(camera, lights);

            // 渲染所有对象
            RenderMeshes();

            // 结束场景渲染
            Renderer3D::EndScene();
        }

        void RenderSystem::CollectRenderables() {
            m_RenderQueue.clear();

            auto renderableEntities = GetEntities<TransformComponent, MeshRendererComponent>();

            // 获取相机位置用于距离排序
            glm::vec3 cameraPos(0.0f);
            if (m_ActiveCamera != NULL_ENTITY) {
                const auto& cameraTransform = m_World->GetComponent<TransformComponent>(m_ActiveCamera);
                cameraPos = cameraTransform.Position;
            }

            for (Entity entity : renderableEntities) {
                auto& transform = m_World->GetComponent<TransformComponent>(entity);
                auto& meshRenderer = m_World->GetComponent<MeshRendererComponent>(entity);

                // 跳过没有网格的对象
                if (!meshRenderer.MeshData) {
                    continue;
                }

                // 检查对象是否活跃
                if (m_World->HasComponent<ActiveComponent>(entity)) {
                    const auto& active = m_World->GetComponent<ActiveComponent>(entity);
                    if (!active.IsActive) {
                        continue;
                    }
                }

                RenderableObject obj;
                obj.EntityID = entity;
                obj.Transform = &transform;
                obj.MeshRenderer = &meshRenderer;
                obj.DistanceToCamera = glm::length(transform.Position - cameraPos);

                m_RenderQueue.push_back(obj);
            }

            // 按距离排序（从近到远，用于不透明物体）
            std::sort(m_RenderQueue.begin(), m_RenderQueue.end(),
                     [](const RenderableObject& a, const RenderableObject& b) {
                         return a.DistanceToCamera < b.DistanceToCamera;
                     });
        }

        std::vector<Light> RenderSystem::CollectLights() {
            std::vector<Light> lights;
            auto lightEntities = GetEntities<TransformComponent, LightComponent>();

            for (Entity entity : lightEntities) {
                const auto& transform = m_World->GetComponent<TransformComponent>(entity);
                const auto& lightComp = m_World->GetComponent<LightComponent>(entity);

                // 检查光源是否活跃
                if (m_World->HasComponent<ActiveComponent>(entity)) {
                    const auto& active = m_World->GetComponent<ActiveComponent>(entity);
                    if (!active.IsActive) {
                        continue;
                    }
                }

                // 转换ECS光源组件为渲染器光源对象
                Light light = ConvertToRendererLight(transform, lightComp);
                lights.push_back(light);
            }

            return lights;
        }

        void RenderSystem::RenderMeshes() {
            for (const auto& obj : m_RenderQueue) {
                if (!obj.MeshRenderer->MeshData) {
                    continue;
                }

                // 暂时跳过Model创建，直接使用Mesh渲染
                // TODO: 实现直接使用Mesh的渲染方法，或者修改Model类支持从Mesh创建

                // 注释掉有问题的代码，等Model类支持后再启用
                /*
                std::shared_ptr<Model> model = CreateModelFromMesh(obj.MeshRenderer->MeshData);
                if (!model) {
                    continue;
                }

                if (obj.MeshRenderer->MaterialData) {
                    Renderer3D::Submit(model, obj.Transform->WorldMatrix, obj.MeshRenderer->MaterialData);
                } else {
                    Renderer3D::Submit(model, obj.Transform->WorldMatrix);
                }
                */
            }
        }

        Camera RenderSystem::CreateCameraFromComponent(const TransformComponent& transform,
                                                       const CameraComponent& cameraComp) {
            // 从四元数计算pitch和yaw角度
            glm::vec3 forward = transform.GetForward();
            float pitch = glm::degrees(asin(forward.y));
            float yaw = glm::degrees(atan2(forward.z, forward.x));

            // 获取宽高比
            float aspectRatio = GetAspectRatio();

            // 使用现有Camera构造函数
            Camera camera(
                cameraComp.FOV,
                aspectRatio,
                cameraComp.NearPlane,
                cameraComp.FarPlane
            );

            // 设置位置和旋转
            camera.SetPosition(transform.Position);
            camera.SetRotation(pitch, yaw);

            return camera;
        }

        Light RenderSystem::ConvertToRendererLight(const TransformComponent& transform,
                                                   const LightComponent& lightComp) {
            JFM::Light light;
            light.Type = lightComp.Type;
            light.Color = lightComp.Color;
            light.Intensity = lightComp.Intensity;
            light.Position = transform.Position;
            light.Direction = transform.GetForward();
            light.Range = lightComp.Range;
            light.InnerConeAngle = lightComp.InnerConeAngle;
            light.OuterConeAngle = lightComp.OuterConeAngle;
            light.CastShadows = lightComp.CastShadows;

            return light;
        }

        std::shared_ptr<Model> RenderSystem::CreateModelFromMesh(std::shared_ptr<Mesh> mesh) {
            // 由于Model类需要文件路径参数，我们需要创建一个临时的模型
            // 这里暂时返回nullptr，实际使用中需要根据具体需求调整
            // TODO: 实现从Mesh创建Model的逻辑，或者直接使用Mesh进行渲染
            (void)mesh; // 避免未使用参数警告
            return nullptr;
        }

        float RenderSystem::GetAspectRatio() {
            // TODO: 从Application或Window获取实际的宽高比
            // 这里暂时返回常见的16:9比例
            return 16.0f / 9.0f;
        }

        void RenderSystem::SetWireframeMode(bool enable) {
            Renderer3D::SetWireframeMode(enable);
        }

        void RenderSystem::SetShadowsEnabled(bool enable) {
            Renderer3D::EnableShadows(enable);
        }

        void RenderSystem::SetPostProcessingEnabled(bool enable) {
            Renderer3D::EnablePostProcessing(enable);
        }

    } // namespace ECS
} // namespace JFM
