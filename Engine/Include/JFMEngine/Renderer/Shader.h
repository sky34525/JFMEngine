//
// Shader.h - 着色器抽象接口
// 支持顶点着色器、片段着色器等
//
#pragma once

#include "JFMEngine/Core/Core.h"
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace JFM {

    class JFM_API Shader {
    public:
        virtual ~Shader() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetInt(const std::string& name, int value) = 0;
        virtual void SetBool(const std::string& name, bool value) = 0;
        virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
        virtual void SetFloat(const std::string& name, float value) = 0;
        virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
        virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
        virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
        virtual void SetMat3(const std::string& name, const glm::mat3& value) = 0;
        virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

        virtual const std::string& GetName() const = 0;

        static std::shared_ptr<Shader> Create(const std::string& filepath);
        static std::shared_ptr<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
    };

    class JFM_API ShaderLibrary {
    public:
        void Add(const std::string& name, const std::shared_ptr<Shader>& shader);
        void Add(const std::shared_ptr<Shader>& shader);
        std::shared_ptr<Shader> Load(const std::string& filepath);
        std::shared_ptr<Shader> Load(const std::string& name, const std::string& filepath);

        std::shared_ptr<Shader> Get(const std::string& name);

        bool Exists(const std::string& name) const;

    private:
        std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
    };

}
