//
// VertexArray.h - 顶点数组对象抽象接口
// 管理顶点缓冲区和索引缓冲区的绑定
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "Buffer.h"
#include <memory>
#include <vector>

namespace JFM {

    class JFM_API VertexArray {
    public:
        virtual ~VertexArray() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;
        virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

        virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const = 0;
        virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const = 0;

        static std::shared_ptr<VertexArray> Create();
    };

}
