//
// RenderCommand.h - 渲染命令接口
// 提供静态渲染命令API，实现渲染命令的解耦和平台无关性
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "RendererAPI.h"
#include <glm/glm.hpp>

namespace JFM {

    // 多边形模式枚举
    enum class PolygonMode {
        Fill,  // 填充模式
        Line,  // 线框模式
        Point  // 点模式
    };

    class JFM_API RenderCommand {
    public:
        static void Init();
        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
        static void SetClearColor(const glm::vec4& color);
        static void Clear();
        static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0);
        static void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount);
        static void SetPolygonMode(PolygonMode mode);

    private:
        static std::unique_ptr<RendererAPI> s_RendererAPI;
    };

}
