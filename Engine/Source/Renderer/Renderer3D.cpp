//
// Renderer3D.cpp - 3D渲染器实现
//

#include "JFMEngine/Renderer/Renderer3D.h"
#include "JFMEngine/Renderer/RenderCommand.h"
#include "JFMEngine/Utils//Log.h"
#include <algorithm>

namespace JFM {

    // 静态成员变量定义
    bool Renderer3D::s_ShadowsEnabled = false;
    bool Renderer3D::s_PostProcessingEnabled = false;
    uint32_t Renderer3D::s_ShadowMapSize = 1024;
    float Renderer3D::s_Exposure = 1.0f;
    float Renderer3D::s_Gamma = 2.2f;

    Renderer3DStats Renderer3D::s_Stats;
    std::vector<RenderItem> Renderer3D::s_OpaqueQueue;
    std::vector<RenderItem> Renderer3D::s_TransparentQueue;

    std::shared_ptr<Shader> Renderer3D::s_DefaultShader;
    std::shared_ptr<Shader> Renderer3D::s_ShadowShader;
    std::shared_ptr<Shader> Renderer3D::s_SkyboxShader;
    std::shared_ptr<Shader> Renderer3D::s_PostProcessShader;
    std::shared_ptr<Texture> Renderer3D::s_Skybox;

    // 添加缺失的静态成员变量定义
    Camera Renderer3D::s_Camera(45.0f, 16.0f/9.0f, 0.1f, 100.0f);
    std::vector<Light> Renderer3D::s_Lights;

    void Renderer3D::Init() {
        s_Stats = {};
        s_OpaqueQueue.clear();
        s_TransparentQueue.clear();
        s_ShadowsEnabled = false;
        s_PostProcessingEnabled = false;
    }

    void Renderer3D::Shutdown() {
        s_OpaqueQueue.clear();
        s_TransparentQueue.clear();
    }

    void Renderer3D::BeginScene(const Camera& camera, const std::vector<Light>& lights) {
        s_OpaqueQueue.clear();
        s_TransparentQueue.clear();

        // 重置统计信息
        s_Stats.DrawCalls = 0;
        s_Stats.VertexCount = 0;
        s_Stats.IndexCount = 0;
        s_Stats.ModelCount = 0;
    }

    void Renderer3D::EndScene() {
        // 渲染队列中的所有物体
        for (const auto& item : s_OpaqueQueue) {
            s_Stats.DrawCalls++;
            s_Stats.ModelCount++;
        }

        for (const auto& item : s_TransparentQueue) {
            s_Stats.DrawCalls++;
            s_Stats.ModelCount++;
        }

        s_OpaqueQueue.clear();
        s_TransparentQueue.clear();
    }

    void Renderer3D::Submit(const std::shared_ptr<Model>& model, const glm::mat4& transform) {
        Submit(model, transform, nullptr);
    }

    void Renderer3D::Submit(const std::shared_ptr<Model>& model, const glm::mat4& transform,
                           const std::shared_ptr<Material>& material) {
        if (!model) {
            return;
        }

        RenderItem item;
        item.Model = model;
        item.Transform = transform;
        item.Material = material;
        item.DistanceToCamera = 0.0f; // 简化实现

        // 简单地添加到不透明队列
        s_OpaqueQueue.push_back(item);
    }

    void Renderer3D::EnableShadows(bool enable) {
        s_ShadowsEnabled = enable;
    }

    void Renderer3D::SetShadowMapSize(uint32_t size) {
        s_ShadowMapSize = size;
    }

    void Renderer3D::SetWireframeMode(bool enabled) {
        if (enabled) {
            RenderCommand::SetPolygonMode(PolygonMode::Line);
        } else {
            RenderCommand::SetPolygonMode(PolygonMode::Fill);
        }
    }

    void Renderer3D::EnablePostProcessing(bool enable) {
        s_PostProcessingEnabled = enable;
    }

    void Renderer3D::SetExposure(float exposure) {
        s_Exposure = exposure;
    }

    void Renderer3D::SetGamma(float gamma) {
        s_Gamma = gamma;
    }

    const Renderer3DStats& Renderer3D::GetStats() {
        return s_Stats;
    }

    void Renderer3D::ResetStats() {
        s_Stats = {};
    }

    // 基础几何体渲染的简化实现
    void Renderer3D::DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color) {
        // 简化实现 - 实际绘制逻辑需要在此实现
    }

    void Renderer3D::DrawSphere(const glm::vec3& position, float radius, const glm::vec4& color) {
        // 简化实现 - 实际绘制逻辑需要在此实现
    }

    void Renderer3D::DrawPlane(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
        // 简化实现 - 实际绘制逻辑需要在此实现
    }

    void Renderer3D::DrawInstanced(const std::shared_ptr<Model>& model, const std::vector<glm::mat4>& transforms) {
        // 简化实现 - 实际绘制逻辑需要在此实现
    }

    void Renderer3D::SetSkybox(const std::shared_ptr<Texture>& skybox) {
        s_Skybox = skybox;
    }

    void Renderer3D::DrawSkybox() {
        if (s_Skybox) {
            // 简化实现 - 实际绘制逻辑需要在此实现
        }
    }

    void Renderer3D::SetCullingMode(bool enable) {
        // 实际设置面剔除状态的逻辑需要在此实现
    }

    void Renderer3D::SetDepthTesting(bool enable) {
        // 实际设置深度测试状态的逻辑需要在此实现
    }

} // namespace JFM
