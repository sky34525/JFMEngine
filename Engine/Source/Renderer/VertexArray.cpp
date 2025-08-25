//
// VertexArray.cpp - 顶点数组对象抽象实现
//

#include "JFMEngine/Renderer/VertexArray.h"
#include "JFMEngine/Renderer/RendererAPI.h"
#include "JFMEngine/Renderer/OpenGLVertexArray.h"

namespace JFM {

    std::shared_ptr<VertexArray> VertexArray::Create() {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:
                return nullptr;
            case RendererAPI::API::OpenGL:
                return std::make_shared<OpenGLVertexArray>();
        }
        return nullptr;
    }

}
