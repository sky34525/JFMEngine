//
// OpenGLBuffer.cpp - OpenGL缓冲区对象具体实现
//

#include "JFMEngine/Renderer/OpenGLBuffer.h"
#include "JFMEngine/Core/Core.h"
#include "JFMEngine/Utils/Log.h"
#include <glad/glad.h>

namespace JFM {

    /////////////////////////////////////////////////////////////////////////////
    // VertexBuffer /////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////

    OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size) {
        JFM_GL_CALL(glGenBuffers(1, &m_RendererID));
        JFM_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
        JFM_GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
    }

    OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size) {
        JFM_GL_CALL(glGenBuffers(1, &m_RendererID));
        JFM_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
        JFM_GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW));
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer() {
        JFM_GL_CALL(glDeleteBuffers(1, &m_RendererID));
    }

    void OpenGLVertexBuffer::Bind() const {
        JFM_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    }

    void OpenGLVertexBuffer::Unbind() const {
        JFM_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

    void OpenGLVertexBuffer::SetData(const void* data, uint32_t size) {
        JFM_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
        JFM_GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
    }

    /////////////////////////////////////////////////////////////////////////////
    // IndexBuffer //////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////

    OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
        : m_Count(count) {
        JFM_GL_CALL(glGenBuffers(1, &m_RendererID));
        JFM_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
        JFM_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW));
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer() {
        JFM_GL_CALL(glDeleteBuffers(1, &m_RendererID));
    }

    void OpenGLIndexBuffer::Bind() const {
        JFM_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
    }

    void OpenGLIndexBuffer::Unbind() const {
        JFM_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

}
