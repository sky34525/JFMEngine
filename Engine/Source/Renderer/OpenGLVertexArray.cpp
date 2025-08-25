//
// OpenGLVertexArray.cpp - OpenGL顶点数组对象具体实现
//

#include "JFMEngine/Renderer/OpenGLVertexArray.h"
#include "JFMEngine/Core/Core.h"
#include "JFMEngine/Utils/Log.h"
#include <glad/glad.h>

namespace JFM {

    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Float:    return GL_FLOAT;
            case ShaderDataType::Float2:   return GL_FLOAT;
            case ShaderDataType::Float3:   return GL_FLOAT;
            case ShaderDataType::Float4:   return GL_FLOAT;
            case ShaderDataType::Mat3:     return GL_FLOAT;
            case ShaderDataType::Mat4:     return GL_FLOAT;
            case ShaderDataType::Int:      return GL_INT;
            case ShaderDataType::Int2:     return GL_INT;
            case ShaderDataType::Int3:     return GL_INT;
            case ShaderDataType::Int4:     return GL_INT;
            case ShaderDataType::Bool:     return GL_BOOL;
        }
        return 0;
    }

    OpenGLVertexArray::OpenGLVertexArray() {
        JFM_GL_CALL(glGenVertexArrays(1, &m_RendererID));
    }

    OpenGLVertexArray::~OpenGLVertexArray() {
        JFM_GL_CALL(glDeleteVertexArrays(1, &m_RendererID));
    }

    void OpenGLVertexArray::Bind() const {
        JFM_GL_CALL(glBindVertexArray(m_RendererID));
    }

    void OpenGLVertexArray::Unbind() const {
        JFM_GL_CALL(glBindVertexArray(0));
    }

    void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {
        if (!vertexBuffer) {
            return;
        }

        const auto& layout = vertexBuffer->GetLayout();
        if (layout.GetElements().empty()) {
            return;
        }

        Bind();

        vertexBuffer->Bind();

        uint32_t index = 0;
        for (const auto& element : layout.GetElements()) {
            switch (element.Type) {
                case ShaderDataType::Float:
                case ShaderDataType::Float2:
                case ShaderDataType::Float3:
                case ShaderDataType::Float4: {
                    glEnableVertexAttribArray(index);
                    glVertexAttribPointer(index,
                                        element.GetComponentCount(),
                                        ShaderDataTypeToOpenGLBaseType(element.Type),
                                        element.Normalized ? GL_TRUE : GL_FALSE,
                                        layout.GetStride(),
                                        (const void*)element.Offset);
                    index++;
                    break;
                }
                case ShaderDataType::Int:
                case ShaderDataType::Int2:
                case ShaderDataType::Int3:
                case ShaderDataType::Int4:
                case ShaderDataType::Bool: {
                    glEnableVertexAttribArray(index);
                    glVertexAttribIPointer(index,
                                         element.GetComponentCount(),
                                         ShaderDataTypeToOpenGLBaseType(element.Type),
                                         layout.GetStride(),
                                         (const void*)element.Offset);
                    index++;
                    break;
                }
                case ShaderDataType::Mat3:
                case ShaderDataType::Mat4: {
                    uint8_t count = element.GetComponentCount();
                    for (uint8_t i = 0; i < count; i++) {
                        glEnableVertexAttribArray(index);
                        glVertexAttribPointer(index,
                                            count,
                                            ShaderDataTypeToOpenGLBaseType(element.Type),
                                            element.Normalized ? GL_TRUE : GL_FALSE,
                                            layout.GetStride(),
                                            (const void*)(element.Offset + sizeof(float) * count * i));
                        glVertexAttribDivisor(index, 1);
                        index++;
                    }
                    break;
                }
            }
        }

        m_VertexBuffers.push_back(vertexBuffer);

        Unbind();
    }

    void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
        if (!indexBuffer) {
            return;
        }
        Bind();
        indexBuffer->Bind();

        m_IndexBuffer = indexBuffer;
        Unbind();
    }

}
