//
// Renderer3D.h - 专门的3D渲染器
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Model.h"
#include "Light.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace JFM {

    // 3D渲染统计信息
    struct Renderer3DStats {
        uint32_t DrawCalls = 0;
        uint32_t VertexCount = 0;
        uint32_t IndexCount = 0;
        uint32_t ModelCount = 0;
    };

    // 渲染队列项
    struct RenderItem {
        std::shared_ptr<Model> Model;
        glm::mat4 Transform;
        std::shared_ptr<Material> Material;
        float DistanceToCamera;
    };

    class JFM_API Renderer3D {
    public:
        static void Init();
        static void Shutdown();

        // 场景渲染
        static void BeginScene(const Camera& camera, const std::vector<Light>& lights = {});
        static void EndScene();

        // 模型提交
        static void Submit(const std::shared_ptr<Model>& model, const glm::mat4& transform);
        static void Submit(const std::shared_ptr<Model>& model, const glm::mat4& transform,
                          const std::shared_ptr<Material>& material);

        // 基础几何体渲染
        static void DrawCube(const glm::vec3& position, const glm::vec3& size,
                           const glm::vec4& color = glm::vec4(1.0f));
        static void DrawSphere(const glm::vec3& position, float radius,
                             const glm::vec4& color = glm::vec4(1.0f));
        static void DrawPlane(const glm::vec3& position, const glm::vec2& size,
                            const glm::vec4& color = glm::vec4(1.0f));

        // 实例化渲染
        static void DrawInstanced(const std::shared_ptr<Model>& model,
                                const std::vector<glm::mat4>& transforms);

        // 天空盒渲染
        static void SetSkybox(const std::shared_ptr<Texture>& skybox);
        static void DrawSkybox();

        // 阴影渲染
        static void EnableShadows(bool enable);
        static void SetShadowMapSize(uint32_t size);

        // 后处理效果
        static void EnablePostProcessing(bool enable);
        static void SetExposure(float exposure);
        static void SetGamma(float gamma);

        // 调试和性能
        static const Renderer3DStats& GetStats();
        static void ResetStats();

        // 渲染设置
        static void SetWireframeMode(bool enable);
        static void SetCullingMode(bool enable);
        static void SetDepthTesting(bool enable);

    private:
        static void InitDefaultShaders();
        static void InitFramebuffers();
        static void RenderShadowMap(const std::vector<Light>& lights);
        static void RenderOpaqueObjects();
        static void RenderTransparentObjects();
        static void RenderPostProcessing();

        static Renderer3DStats s_Stats;
        static std::vector<RenderItem> s_OpaqueQueue;
        static std::vector<RenderItem> s_TransparentQueue;

        static std::shared_ptr<Shader> s_DefaultShader;
        static std::shared_ptr<Shader> s_ShadowShader;
        static std::shared_ptr<Shader> s_SkyboxShader;
        static std::shared_ptr<Shader> s_PostProcessShader;

        static std::shared_ptr<Texture> s_Skybox;
        static Camera s_Camera;
        static std::vector<Light> s_Lights;

        static bool s_ShadowsEnabled;
        static bool s_PostProcessingEnabled;
        static uint32_t s_ShadowMapSize;
        static float s_Exposure;
        static float s_Gamma;
    };

}
