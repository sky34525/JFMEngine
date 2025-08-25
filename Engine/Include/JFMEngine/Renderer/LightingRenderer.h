//
// LightingRenderer.h - 光照渲染器
// 整合光照系统与渲染管线
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/Renderer/Light.h"
#include "JFMEngine/Renderer/Material.h"
#include "JFMEngine/Renderer/Camera.h"
#include "JFMEngine/Renderer/Shader.h"
#include "JFMEngine/Renderer/VertexArray.h"
#include <glm/glm.hpp>
#include <memory>

namespace JFM {

    class JFM_API LightingRenderer {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const Camera& camera);
        static void EndScene();

        // 渲染带光照的物体
        static void Submit(const std::shared_ptr<Shader>& shader,
                          const std::shared_ptr<VertexArray>& vertexArray,
                          const Material& material,
                          const glm::mat4& transform = glm::mat4(1.0f));

        // 设置光照参数到着色器
        static void SetLightingUniforms(std::shared_ptr<Shader> shader, const glm::vec3& viewPos);

    private:
        struct SceneData {
            glm::mat4 ViewProjectionMatrix;
            glm::vec3 ViewPosition;
        };

        static SceneData* s_SceneData;
    };

}
