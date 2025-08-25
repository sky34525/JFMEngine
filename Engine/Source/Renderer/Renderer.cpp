//
// Renderer.cpp - 核心渲染器实现
//

#include "JFMEngine/Renderer/Renderer.h"
#include "JFMEngine/Renderer/RenderCommand.h"
#include "JFMEngine/Utils/Log.h"

namespace JFM {

    std::unique_ptr<SceneData> Renderer::s_SceneData = std::make_unique<SceneData>();

    void Renderer::Init() {
        RenderCommand::Init();
    }

    void Renderer::Shutdown() {
        // 清理渲染器资源
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
        RenderCommand::SetViewport(0, 0, width, height);
    }

    void Renderer::BeginScene(const Camera& camera) {
        s_SceneData->ViewProjectionMatrix = camera.GetProjectionMatrix() * camera.GetViewMatrix();
    }

    void Renderer::EndScene() {
        // 结束场景渲染
        // 在这里可以执行后处理或提交渲染命令
    }

    void Renderer::Submit(const std::shared_ptr<Shader>& shader,
                         const std::shared_ptr<VertexArray>& vertexArray) {

        if (!shader) {
            return;
        }

        if (!vertexArray) {
            return;
        }

        shader->Bind();
        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }

}
