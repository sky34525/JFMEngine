//
// Renderer.h - 核心渲染器接口
// 提供统一的渲染API抽象
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "RendererAPI.h"
#include "RenderCommand.h"
#include "Shader.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "Camera.h"
#include <memory>

namespace JFM {

    struct SceneData {
        glm::mat4 ViewProjectionMatrix;
    };
    class JFM_API Renderer {
    public:
        static void Init();
        static void Shutdown();

        static void OnWindowResize(uint32_t width, uint32_t height);

        // 新增：支持传入Camera
        static void BeginScene(const Camera& camera);
        static void EndScene();

        static void Submit(const std::shared_ptr<Shader>& shader,
                          const std::shared_ptr<VertexArray>& vertexArray);

        static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

        // 新增：获取当前场景数据（只读）
        static const SceneData& GetSceneData() { return *s_SceneData; }

    private:
        static std::unique_ptr<SceneData> s_SceneData;
    };

}
