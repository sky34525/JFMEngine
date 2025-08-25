//
// LightingManager.h - 光照系统管理器
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/Renderer/Shader.h"
#include "JFMEngine/Renderer/Light.h"        // 使用已有的光源定义
#include "JFMEngine/Renderer/Material.h"     // 使用已有的材质定义
#include <glm/glm.hpp>
#include <memory>

namespace JFM {

    class JFM_API LightingManager {
    public:
        static LightingManager& GetInstance() {
            static LightingManager instance;
            return instance;
        }

        // 设置光照到着色器
        void ApplyLighting(std::shared_ptr<Shader> shader, const glm::vec3& viewPos);

        // 设置材质到着色器 - 使用引擎已有的MaterialProperties
        void ApplyMaterial(std::shared_ptr<Shader> shader, const MaterialProperties& material);

        // 光源管理 - 使用引擎已有的光源结构体
        void SetDirectionalLight(const DirectionalLight& light) { m_DirectionalLight = light; }
        const DirectionalLight& GetDirectionalLight() const { return m_DirectionalLight; }

        void AddPointLight(const PointLight& light);
        void ClearPointLights() { m_PointLights.clear(); }

        // 设置默认光照场景
        void SetupDefaultLighting();

    private:
        DirectionalLight m_DirectionalLight;
        std::vector<PointLight> m_PointLights;

        LightingManager() = default;
    };

}
