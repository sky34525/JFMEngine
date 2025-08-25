#include "JFMEngine/Renderer/LightingRenderer.h"
#include "JFMEngine/Renderer/RenderCommand.h"
#include "JFMEngine/Renderer/VertexArray.h"
#include "JFMEngine/Renderer/Light.h"
#include <glm/gtc/matrix_transform.hpp>

namespace JFM {

    LightingRenderer::SceneData* LightingRenderer::s_SceneData = nullptr;

    void LightingRenderer::Init() {
        s_SceneData = new SceneData;
    }

    void LightingRenderer::Shutdown() {
        delete s_SceneData;
        s_SceneData = nullptr;
    }

    void LightingRenderer::BeginScene(const Camera& camera) {
        s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
        s_SceneData->ViewPosition = camera.GetPosition();
    }

    void LightingRenderer::EndScene() {
        // 可以在这里执行批处理渲染等优化
    }

    void LightingRenderer::Submit(const std::shared_ptr<Shader>& shader,
                                 const std::shared_ptr<VertexArray>& vertexArray,
                                 const Material& material,
                                 const glm::mat4& transform) {
        shader->Bind();

        // 设置变换矩阵
        shader->SetMat4("u_ViewProjectionMatrix", s_SceneData->ViewProjectionMatrix);
        shader->SetMat4("u_ModelMatrix", transform);

        // 计算法线矩阵 - 修复法线矩阵的设置方式
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
        // 使用SetMat4来设置法线矩阵，扩展为4x4矩阵
        glm::mat4 normalMatrix4 = glm::mat4(normalMatrix);
        shader->SetMat4("u_NormalMatrix", normalMatrix4);

        // 设置观察位置
        shader->SetFloat3("u_ViewPos", s_SceneData->ViewPosition);

        // 绑定材质
        material.Bind(shader);

        // 设置光照
        SetLightingUniforms(shader, s_SceneData->ViewPosition);

        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }

    void LightingRenderer::SetLightingUniforms(std::shared_ptr<Shader> shader, const glm::vec3& viewPos) {
        auto& lightManager = LightManager::GetInstance();

        // 设置方向光
        const auto& dirLight = lightManager.GetDirectionalLight();
        shader->SetFloat3("u_DirLight.direction", dirLight.Direction);
        shader->SetFloat3("u_DirLight.ambient", dirLight.Ambient);
        shader->SetFloat3("u_DirLight.diffuse", dirLight.Diffuse);
        shader->SetFloat3("u_DirLight.specular", dirLight.Specular);

        // 设置点光源
        const auto& pointLights = lightManager.GetPointLights();
        for (size_t i = 0; i < pointLights.size() && i < 4; ++i) {
            std::string base = "u_PointLights[" + std::to_string(i) + "]";
            shader->SetFloat3(base + ".position", pointLights[i].Position);
            shader->SetFloat3(base + ".ambient", pointLights[i].Ambient);
            shader->SetFloat3(base + ".diffuse", pointLights[i].Diffuse);
            shader->SetFloat3(base + ".specular", pointLights[i].Specular);
            shader->SetFloat(base + ".constant", pointLights[i].Constant);
            shader->SetFloat(base + ".linear", pointLights[i].Linear);
            shader->SetFloat(base + ".quadratic", pointLights[i].Quadratic);
        }

        // 设置聚光灯
        const auto& spotLights = lightManager.GetSpotLights();
        if (!spotLights.empty()) {
            const auto& spotLight = spotLights[0]; // 使用第一个聚光灯
            shader->SetFloat3("u_SpotLight.position", spotLight.Position);
            shader->SetFloat3("u_SpotLight.direction", spotLight.Direction);
            shader->SetFloat3("u_SpotLight.ambient", spotLight.Ambient);
            shader->SetFloat3("u_SpotLight.diffuse", spotLight.Diffuse);
            shader->SetFloat3("u_SpotLight.specular", spotLight.Specular);
            shader->SetFloat("u_SpotLight.constant", spotLight.Constant);
            shader->SetFloat("u_SpotLight.linear", spotLight.Linear);
            shader->SetFloat("u_SpotLight.quadratic", spotLight.Quadratic);
            shader->SetFloat("u_SpotLight.cutOff", glm::cos(glm::radians(spotLight.CutOff)));
            shader->SetFloat("u_SpotLight.outerCutOff", glm::cos(glm::radians(spotLight.OuterCutOff)));
        }
    }

}
