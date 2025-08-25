//
// RendererAPI.cpp - 渲染API抽象实现
//

#include "JFMEngine/Renderer/RendererAPI.h"
#include "JFMEngine/Renderer/OpenGLRendererAPI.h"

namespace JFM {

    RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

    std::unique_ptr<RendererAPI> RendererAPI::Create() {
        switch (s_API) {
            case RendererAPI::API::None:
                return nullptr;
            case RendererAPI::API::OpenGL:
                return std::make_unique<OpenGLRendererAPI>();
            case RendererAPI::API::Vulkan:
                // TODO: 实现Vulkan支持
                return nullptr;
            case RendererAPI::API::DirectX11:
                // TODO: 实现DirectX11支持
                return nullptr;
            case RendererAPI::API::DirectX12:
                // TODO: 实现DirectX12支持
                return nullptr;
        }
        return nullptr;
    }

}
