//
// RenderCommand.cpp - 渲染命令实现
//

#include "JFMEngine/Renderer/RenderCommand.h"
#include "JFMEngine/Utils/Log.h"

namespace JFM {

    std::unique_ptr<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();

    void RenderCommand::Init() {
        if (s_RendererAPI) {
            s_RendererAPI->Init();
        }
    }

    void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
        if (s_RendererAPI) {
            s_RendererAPI->SetViewport(x, y, width, height);
        }
    }

    void RenderCommand::SetClearColor(const glm::vec4& color) {
        if (s_RendererAPI) {
            s_RendererAPI->SetClearColor(color);
        }
    }

    void RenderCommand::Clear() {
        if (s_RendererAPI) {
            s_RendererAPI->Clear();
        }
    }

    void RenderCommand::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount) {
        if (!vertexArray) {
            return;
        }

        if (s_RendererAPI) {
            s_RendererAPI->DrawIndexed(vertexArray, indexCount);
        }
    }

    void RenderCommand::DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) {
        if (!vertexArray) {
            return;
        }
        
        if (s_RendererAPI) {
            s_RendererAPI->DrawArrays(vertexArray, vertexCount);
        }
    }

    void RenderCommand::SetPolygonMode(PolygonMode mode) {
        if (s_RendererAPI) {
            s_RendererAPI->SetPolygonMode(mode);
        }
    }

}
