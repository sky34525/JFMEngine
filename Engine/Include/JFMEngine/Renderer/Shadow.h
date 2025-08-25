//
// Shadow.h - 阴影渲染系统
// 支持阴影贴图和级联阴影贴图
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/Renderer/Shader.h"
#include "JFMEngine/Renderer/Texture.h"
#include "JFMEngine/Renderer/Light.h"
#include "JFMEngine/Renderer/Camera.h"
#include <memory>

namespace JFM {

    class JFM_API ShadowMap {
    public:
        ShadowMap(uint32_t width = 1024, uint32_t height = 1024);
        ~ShadowMap();

        void BeginShadowPass();
        void EndShadowPass();

        void BindShadowMap(uint32_t slot = 0) const;
        uint32_t GetShadowMapID() const { return m_ShadowMapTexture; }

        glm::mat4 GetLightSpaceMatrix() const { return m_LightSpaceMatrix; }
        void SetLightSpaceMatrix(const glm::mat4& matrix) { m_LightSpaceMatrix = matrix; }

    private:
        uint32_t m_FBO;
        uint32_t m_ShadowMapTexture;
        uint32_t m_Width, m_Height;
        glm::mat4 m_LightSpaceMatrix;
    };

    class JFM_API ShadowRenderer {
    public:
        static void Init();
        static void Shutdown();

        static void BeginShadowPass(const DirectionalLight& light, const glm::vec3& sceneCenter, float sceneRadius);
        static void EndShadowPass();

        static void RenderShadowCasters(std::shared_ptr<Shader> shader);
        static glm::mat4 GetLightSpaceMatrix();
        static void BindShadowMap(uint32_t slot = 1);

    private:
        static std::unique_ptr<ShadowMap> s_ShadowMap;
        static std::shared_ptr<Shader> s_ShadowShader;
    };

}
