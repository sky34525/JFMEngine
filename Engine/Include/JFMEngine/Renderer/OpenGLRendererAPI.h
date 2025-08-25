//
// OpenGLRendererAPI.h - OpenGL渲染API具体实现
//

#pragma once

#include "JFMEngine/Renderer/RendererAPI.h"

namespace JFM {

    class OpenGLRendererAPI : public RendererAPI {
    public:
        virtual void Init() override;
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        virtual void SetClearColor(const glm::vec4& color) override;
        virtual void Clear() override;

        virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
        virtual void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) override;

        virtual void SetPolygonMode(PolygonMode mode) override;
    };

}
