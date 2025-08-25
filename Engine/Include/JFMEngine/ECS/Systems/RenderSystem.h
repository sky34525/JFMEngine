//
// RenderSystem.h - 渲染系统
// 处理网格渲染、光照和相机管理（集成Renderer3D）
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/ECS/System.h"
#include "JFMEngine/ECS/Components.h"
#include "JFMEngine/Renderer/Camera.h"
#include "JFMEngine/Renderer/Light.h"
#include "JFMEngine/Renderer/Model.h"
#include "JFMEngine/Renderer/Mesh.h"
#include <vector>

namespace JFM {
    namespace ECS {

        class JFM_API RenderSystem : public System {
        public:
            void Update(float deltaTime) override;
            void Render() override;

            // 渲染配置
            void SetWireframeMode(bool enable);
            void SetShadowsEnabled(bool enable);
            void SetPostProcessingEnabled(bool enable);

        private:
            void CollectRenderables();
            std::vector<Light> CollectLights();
            void RenderMeshes();

            // 转换方法
            Camera CreateCameraFromComponent(const TransformComponent& transform,
                                           const CameraComponent& cameraComp);
            Light ConvertToRendererLight(const TransformComponent& transform,
                                       const LightComponent& lightComp);
            std::shared_ptr<Model> CreateModelFromMesh(std::shared_ptr<Mesh> mesh);

            // 工具方法
            float GetAspectRatio();

            struct RenderableObject {
                Entity EntityID;
                TransformComponent* Transform;
                MeshRendererComponent* MeshRenderer;
                float DistanceToCamera;
            };

            std::vector<RenderableObject> m_RenderQueue;
            Entity m_ActiveCamera = Entity(NULL_ENTITY);
        };

    } // namespace ECS
} // namespace JFM
