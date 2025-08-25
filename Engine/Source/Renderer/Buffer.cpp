//
// Buffer.cpp - 缓冲区对象抽象实现
//

#include "JFMEngine/Renderer/Buffer.h"
#include "JFMEngine/Renderer/RendererAPI.h"
#include "JFMEngine/Renderer/OpenGLBuffer.h"
namespace JFM {

    std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:
                return nullptr;
            case RendererAPI::API::OpenGL:
                return std::make_shared<OpenGLVertexBuffer>(size);
        }
        return nullptr;
    }

    std::shared_ptr<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:
                return nullptr;
            case RendererAPI::API::OpenGL:
                return std::make_shared<OpenGLVertexBuffer>(vertices, size);
        }
        return nullptr;
    }

    std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:
                return nullptr;
            case RendererAPI::API::OpenGL:
                return std::make_shared<OpenGLIndexBuffer>(indices, count);
        }
        return nullptr;
    }

}

