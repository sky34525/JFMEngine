//
// Vertex.h - 顶点数据结构定义
//

#pragma once

#include <glm/glm.hpp>

namespace JFM {

    // 标准顶点结构，包含位置、法线、纹理坐标和切线信息
    struct Vertex {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec3 Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec2 TexCoords = glm::vec2(0.0f);
        glm::vec3 Tangent = glm::vec3(0.0f);
        glm::vec3 Bitangent = glm::vec3(0.0f);

        Vertex() = default;

        Vertex(const glm::vec3& position)
            : Position(position) {}

        Vertex(const glm::vec3& position, const glm::vec3& normal)
            : Position(position), Normal(normal) {}

        Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& texCoords)
            : Position(position), Normal(normal), TexCoords(texCoords) {}
    };

    // 简化的顶点结构（仅位置和颜色）
    struct SimpleVertex {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec4 Color = glm::vec4(1.0f);

        SimpleVertex() = default;

        SimpleVertex(const glm::vec3& position)
            : Position(position) {}

        SimpleVertex(const glm::vec3& position, const glm::vec4& color)
            : Position(position), Color(color) {}
    };

} // namespace JFM
