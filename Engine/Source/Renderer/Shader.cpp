//
// Shader.cpp - 着色器抽象实现
//

#include "JFMEngine/Renderer/Shader.h"
#include "JFMEngine/Renderer/RendererAPI.h"
#include "JFMEngine/Renderer/OpenGLShader.h"

namespace JFM {

    std::shared_ptr<Shader> Shader::Create(const std::string& filepath) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:
                return nullptr;
            case RendererAPI::API::OpenGL:
                return std::make_shared<OpenGLShader>(filepath);
        }
        return nullptr;
    }

    std::shared_ptr<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:
                return nullptr;
            case RendererAPI::API::OpenGL:
                return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
        }
        return nullptr;
    }

    void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader) {
        m_Shaders[name] = shader;
    }

    void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader) {
        auto& name = shader->GetName();
        Add(name, shader);
    }

    std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& filepath) {
        auto shader = Shader::Create(filepath);
        Add(shader);
        return shader;
    }

    std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath) {
        auto shader = Shader::Create(filepath);
        Add(name, shader);
        return shader;
    }

    std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name) {
        auto it = m_Shaders.find(name);
        if (it != m_Shaders.end()) {
            return it->second;
        }
        return nullptr;
    }

    bool ShaderLibrary::Exists(const std::string& name) const {
        return m_Shaders.find(name) != m_Shaders.end();
    }

}
