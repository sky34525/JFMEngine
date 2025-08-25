//
// Material.h - 材质系统
// 定义物体表面的光学属性
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include <glm/glm.hpp>
#include <memory>

namespace JFM {

    class Shader;

    struct JFM_API MaterialProperties {
        // 传统Phong光照属性
        glm::vec3 Ambient = glm::vec3(1.0f, 0.5f, 0.31f);   // 环境光反射
        glm::vec3 Diffuse = glm::vec3(1.0f, 0.5f, 0.31f);   // 漫反射
        glm::vec3 Specular = glm::vec3(0.5f, 0.5f, 0.5f);   // 镜面反射
        float Shininess = 32.0f;                              // 光泽度

        // PBR材质属性
        glm::vec3 Albedo = glm::vec3(1.0f);                   // 反照率/基础颜色
        float Metallic = 0.0f;                                // 金属度 (0.0 = 非金属, 1.0 = 金属)
        float Roughness = 0.5f;                               // 粗糙度 (0.0 = 光滑, 1.0 = 粗糙)
        float AO = 1.0f;                                      // 环境遮蔽
    };

    class JFM_API Material {
    public:
        Material(const MaterialProperties& properties = MaterialProperties{});
        ~Material() = default;

        void Bind(std::shared_ptr<Shader> shader) const;

        // Phong光照属性设置器
        void SetAmbient(const glm::vec3& ambient) { m_Properties.Ambient = ambient; }
        void SetDiffuse(const glm::vec3& diffuse) { m_Properties.Diffuse = diffuse; }
        void SetSpecular(const glm::vec3& specular) { m_Properties.Specular = specular; }
        void SetShininess(float shininess) { m_Properties.Shininess = shininess; }

        // PBR材质属性设置器
        void SetAlbedo(const glm::vec3& albedo) { m_Properties.Albedo = albedo; }
        void SetMetallic(float metallic) { m_Properties.Metallic = metallic; }
        void SetRoughness(float roughness) { m_Properties.Roughness = roughness; }
        void SetAO(float ao) { m_Properties.AO = ao; }

        const MaterialProperties& GetProperties() const { return m_Properties; }

    private:
        MaterialProperties m_Properties;
    };

    // 预定义材质
    class JFM_API Materials {
    public:
        static MaterialProperties Gold() {
            MaterialProperties mat;
            mat.Ambient = glm::vec3(0.24725f, 0.1995f, 0.0745f);
            mat.Diffuse = glm::vec3(0.75164f, 0.60648f, 0.22648f);
            mat.Specular = glm::vec3(0.628281f, 0.555802f, 0.366065f);
            mat.Shininess = 51.2f;
            return mat;
        }

        static MaterialProperties Silver() {
            MaterialProperties mat;
            mat.Ambient = glm::vec3(0.19225f, 0.19225f, 0.19225f);
            mat.Diffuse = glm::vec3(0.50754f, 0.50754f, 0.50754f);
            mat.Specular = glm::vec3(0.508273f, 0.508273f, 0.508273f);
            mat.Shininess = 51.2f;
            return mat;
        }

        static MaterialProperties Ruby() {
            MaterialProperties mat;
            mat.Ambient = glm::vec3(0.1745f, 0.01175f, 0.01175f);
            mat.Diffuse = glm::vec3(0.61424f, 0.04136f, 0.04136f);
            mat.Specular = glm::vec3(0.727811f, 0.626959f, 0.626959f);
            mat.Shininess = 76.8f;
            return mat;
        }

        static MaterialProperties Emerald() {
            MaterialProperties mat;
            mat.Ambient = glm::vec3(0.0215f, 0.1745f, 0.0215f);
            mat.Diffuse = glm::vec3(0.07568f, 0.61424f, 0.07568f);
            mat.Specular = glm::vec3(0.633f, 0.727811f, 0.633f);
            mat.Shininess = 76.8f;
            return mat;
        }
    };

}
