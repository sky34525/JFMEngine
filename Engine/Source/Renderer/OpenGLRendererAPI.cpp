//
// OpenGLRendererAPI.cpp - OpenGL渲染API具体实现
//

#include "JFMEngine/Renderer/OpenGLRendererAPI.h"
#include "JFMEngine/Renderer/VertexArray.h"
#include "JFMEngine/Utils/Log.h"
#include <glad/glad.h>

#include "JFMEngine/Renderer/RenderCommand.h"

namespace JFM {

    void OpenGLRendererAPI::Init() {
        // 获取OpenGL版本信息
        const char* version = (const char*)glGetString(GL_VERSION);
        const char* vendor = (const char*)glGetString(GL_VENDOR);
        const char* renderer = (const char*)glGetString(GL_RENDERER);

        glEnable(GL_BLEND);//允许新绘制的像素与帧缓冲区的像素进行混合
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//设置混合函数

        glEnable(GL_DEPTH_TEST);//启动深度测试，实现正确的3D物体遮挡效果
    }

    void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
        glViewport(x, y, width, height);//定义渲染输出的屏幕区域
    }

    void OpenGLRendererAPI::SetClearColor(const glm::vec4& color) {
        glClearColor(color.r, color.g, color.b, color.a);//设置清屏颜色，glClear(GL_COLOR_BUFFER_BIT)时将清屏颜色填充整个屏幕
    }

    void OpenGLRendererAPI::Clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清除颜色缓冲区和深度缓冲区
    }

    void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount) {
        if (!vertexArray) {
            return;
        }

        if (!vertexArray->GetIndexBuffer()) {
            return;
        }

        uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    }// DrawIndexed方法使用索引缓冲区绘制图形

    void OpenGLRendererAPI::DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) {
        if (!vertexArray) {
            return;
        }
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }

    void OpenGLRendererAPI::SetPolygonMode(PolygonMode mode) {
        GLenum glMode;

        switch (mode) {
            case PolygonMode::Fill:
                glMode = GL_FILL;
                break;
            case PolygonMode::Line:
                glMode = GL_LINE;
                break;
            case PolygonMode::Point:
                glMode = GL_POINT;
                break;
            default:
                return;
        }
        glPolygonMode(GL_FRONT_AND_BACK, glMode);
    }

}
