//
// Light.h - 光照系统
// 支持方向光、点光源、聚光灯等
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace JFM {

    enum class LightType {
        Directional = 0,  // 方向光
        Point = 1,        // 点光源
        Spot = 2          // 聚光灯
    };

    struct JFM_API DirectionalLight {
        glm::vec3 Direction = glm::vec3(-0.2f, -1.0f, -0.3f);
        glm::vec3 Ambient = glm::vec3(0.05f, 0.05f, 0.05f);
        glm::vec3 Diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
        glm::vec3 Specular = glm::vec3(0.5f, 0.5f, 0.5f);
    };

    struct JFM_API PointLight {
        glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);

        glm::vec3 Ambient = glm::vec3(0.05f, 0.05f, 0.05f);
        glm::vec3 Diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
        glm::vec3 Specular = glm::vec3(1.0f, 1.0f, 1.0f);

        float Constant = 1.0f;
        float Linear = 0.09f;
        float Quadratic = 0.032f;
    };

    struct JFM_API SpotLight {
        glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Direction = glm::vec3(0.0f, 0.0f, -1.0f);

        glm::vec3 Ambient = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 Specular = glm::vec3(1.0f, 1.0f, 1.0f);

        float Constant = 1.0f;
        float Linear = 0.09f;
        float Quadratic = 0.032f;

        float CutOff = 12.5f;      // 内锥角
        float OuterCutOff = 15.0f; // 外锥角
    };

    class JFM_API LightManager {
    public:
        static LightManager& GetInstance() {
            static LightManager instance;
            return instance;
        }

        void SetDirectionalLight(const DirectionalLight& light) { m_DirectionalLight = light; }
        void AddPointLight(const PointLight& light) { m_PointLights.push_back(light); }
        void AddSpotLight(const SpotLight& light) { m_SpotLights.push_back(light); }

        void ClearPointLights() { m_PointLights.clear(); }
        void ClearSpotLights() { m_SpotLights.clear(); }

        const DirectionalLight& GetDirectionalLight() const { return m_DirectionalLight; }
        const std::vector<PointLight>& GetPointLights() const { return m_PointLights; }
        const std::vector<SpotLight>& GetSpotLights() const { return m_SpotLights; }

        size_t GetPointLightCount() const { return m_PointLights.size(); }
        size_t GetSpotLightCount() const { return m_SpotLights.size(); }

    private:
        DirectionalLight m_DirectionalLight;
        std::vector<PointLight> m_PointLights;
        std::vector<SpotLight> m_SpotLights;
    };

    // 统一的光源类 - 用于ECS系统集成
    class JFM_API Light {
    public:
        LightType Type = LightType::Directional;
        glm::vec3 Color = glm::vec3(1.0f);
        float Intensity = 1.0f;

        // 位置和方向
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec3 Direction = glm::vec3(0.0f, -1.0f, 0.0f);

        // 点光源和聚光灯属性
        float Range = 10.0f;
        float Constant = 1.0f;
        float Linear = 0.09f;
        float Quadratic = 0.032f;

        // 聚光灯属性
        float InnerConeAngle = 12.5f;
        float OuterConeAngle = 15.0f;

        // 阴影
        bool CastShadows = true;

        // 转换为具体的光源类型
        DirectionalLight ToDirectionalLight() const {
            DirectionalLight light;
            light.Direction = Direction;
            light.Diffuse = Color * Intensity;
            light.Specular = Color * Intensity;
            return light;
        }

        PointLight ToPointLight() const {
            PointLight light;
            light.Position = Position;
            light.Diffuse = Color * Intensity;
            light.Specular = Color * Intensity;
            light.Constant = Constant;
            light.Linear = Linear;
            light.Quadratic = Quadratic;
            return light;
        }

        SpotLight ToSpotLight() const {
            SpotLight light;
            light.Position = Position;
            light.Direction = Direction;
            light.Diffuse = Color * Intensity;
            light.Specular = Color * Intensity;
            light.Constant = Constant;
            light.Linear = Linear;
            light.Quadratic = Quadratic;
            light.CutOff = InnerConeAngle;
            light.OuterCutOff = OuterConeAngle;
            return light;
        }
    };

}
