//
// Skybox.h - 天空盒系统
// 用于渲染环境背景
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/Renderer/Shader.h"
#include "JFMEngine/Renderer/VertexArray.h"
#include "JFMEngine/Renderer/Texture.h"
#include "JFMEngine/Renderer/Camera.h"
#include <memory>
#include <array>

namespace JFM {

    class JFM_API Cubemap : public Texture {
    public:
        virtual ~Cubemap() = default;
        static std::shared_ptr<Cubemap> Create(const std::array<std::string, 6>& faces);
    };

    class JFM_API Skybox {
    public:
        Skybox(const std::array<std::string, 6>& faces);
        ~Skybox() = default;

        void Render(const Camera& camera);

        void SetCubemap(std::shared_ptr<Cubemap> cubemap) { m_Cubemap = cubemap; }
        std::shared_ptr<Cubemap> GetCubemap() const { return m_Cubemap; }

    private:
        std::shared_ptr<Cubemap> m_Cubemap;
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<VertexArray> m_VAO;

        void SetupSkyboxGeometry();
    };

}
