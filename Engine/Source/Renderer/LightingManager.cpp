//
// LightingManager.cpp - 光照系统管理器实现
//

#include "JFMEngine/Renderer/LightingManager.h"

namespace JFM {

    void LightingManager::ApplyLighting(std::shared_ptr<Shader> shader, const glm::vec3& viewPos) {
        if (!shader) return;

        shader->Bind();

        // 设置方向光 - 使用引擎已有的DirectionalLight结构体字段名
        shader->SetFloat3("u_DirLight.direction", m_DirectionalLight.Direction);
        shader->SetFloat3("u_DirLight.ambient", m_DirectionalLight.Ambient);
        shader->SetFloat3("u_DirLight.diffuse", m_DirectionalLight.Diffuse);
        shader->SetFloat3("u_DirLight.specular", m_DirectionalLight.Specular);

        // 设置点光源 - 使用引擎已有的PointLight结构体字段名
        shader->SetInt("u_NumPointLights", static_cast<int>(m_PointLights.size()));
        for (size_t i = 0; i < m_PointLights.size() && i < 4; ++i) {
            std::string base = "u_PointLights[" + std::to_string(i) + "]";
            shader->SetFloat3(base + ".position", m_PointLights[i].Position);
            shader->SetFloat3(base + ".ambient", m_PointLights[i].Ambient);
            shader->SetFloat3(base + ".diffuse", m_PointLights[i].Diffuse);
            shader->SetFloat3(base + ".specular", m_PointLights[i].Specular);
            shader->SetFloat(base + ".constant", m_PointLights[i].Constant);
            shader->SetFloat(base + ".linear", m_PointLights[i].Linear);
            shader->SetFloat(base + ".quadratic", m_PointLights[i].Quadratic);
        }

        // 设置观察者位置
        shader->SetFloat3("u_ViewPos", viewPos);
    }

    void LightingManager::ApplyMaterial(std::shared_ptr<Shader> shader, const MaterialProperties& material) {
        if (!shader) return;

        shader->Bind();
        // 使用引擎已有的MaterialProperties结构体字段名
        shader->SetFloat3("u_Material.ambient", material.Ambient);
        shader->SetFloat3("u_Material.diffuse", material.Diffuse);
        shader->SetFloat3("u_Material.specular", material.Specular);
        shader->SetFloat("u_Material.shininess", material.Shininess);
        shader->SetFloat("u_Material.alpha", 1.0f); // MaterialProperties没有alpha字段，使用默认值
    }

    void LightingManager::AddPointLight(const PointLight& light) {
        if (m_PointLights.size() < 4) { // 最多4个点光源
            m_PointLights.push_back(light);
        }
    }

    void LightingManager::SetupDefaultLighting() {
        // 设置默认方向光（模拟太阳光）- 使用引擎已有的DirectionalLight结构体
        m_DirectionalLight = DirectionalLight{
            glm::vec3(-0.2f, -1.0f, -0.3f),  // Direction
            glm::vec3(0.2f, 0.2f, 0.2f),     // Ambient
            glm::vec3(0.8f, 0.8f, 0.8f),     // Diffuse
            glm::vec3(1.0f, 1.0f, 1.0f)      // Specular
        };

        // 清除之前的点光源
        m_PointLights.clear();

        // 添加一个默认点光源 - 使用引擎已有的PointLight结构体
        AddPointLight(PointLight{
            glm::vec3(2.0f, 4.0f, 2.0f),     // Position
            glm::vec3(0.05f, 0.05f, 0.05f),  // Ambient
            glm::vec3(0.8f, 0.8f, 0.8f),     // Diffuse
            glm::vec3(1.0f, 1.0f, 1.0f),     // Specular
            1.0f, 0.09f, 0.032f               // Constant, Linear, Quadratic
        });
    }

}
