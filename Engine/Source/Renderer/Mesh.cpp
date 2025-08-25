//
// Mesh.cpp - 网格数据实现
//

#include "JFMEngine/Renderer/Mesh.h"
#include "JFMEngine/Utils/Log.h"
#include <glad/glad.h>

namespace JFM {

    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
        : Vertices(vertices), Indices(indices) {
        SetupMesh();
    }

    // 添加带纹理的构造函数
    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices,
               const std::vector<std::shared_ptr<Texture>>& textures)
        : Vertices(vertices), Indices(indices), Textures(textures) {
        SetupMesh();
    }

    Mesh::~Mesh() {
        if (m_IsSetup) {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }
    }

    void Mesh::Draw() const {
        if (!m_IsSetup) {
            return;
        }

        // 绑定所有纹理到对应的纹理单元
        for (size_t i = 0; i < Textures.size(); ++i) {
            if (Textures[i]) {
                Textures[i]->Bind(static_cast<uint32_t>(i));
            }
        }


        // 解绑所有纹理（可选，用于清理状态）
        for (size_t i = 0; i < Textures.size(); ++i) {
            if (Textures[i]) {
                Textures[i]->Unbind();
            }
        }
        glBindVertexArray(VAO);

        if (!Indices.empty()) {
            // 使用索引绘制
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(Indices.size()), GL_UNSIGNED_INT, 0);
        } else {
            // 使用顶点数组绘制
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(Vertices.size()));
        }

        glBindVertexArray(0);
    }

    void Mesh::SetupMesh() {
        if (m_IsSetup) {
            return; // 已经设置过了
        }

        // 生成并绑定VAO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        if (!Indices.empty()) {
            glGenBuffers(1, &EBO);
        }

        glBindVertexArray(VAO);

        // 绑定VBO并上传顶点数据
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), Vertices.data(), GL_STATIC_DRAW);

        // 如果有索引数据，绑定EBO并上传索引数据
        if (!Indices.empty()) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(uint32_t), Indices.data(), GL_STATIC_DRAW);
        }

        // 设置顶点属性指针
        // 位置属性 (location = 0)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

        // 法线属性 (location = 1)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        // 纹理坐标属性 (location = 2)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        // 解绑VAO
        glBindVertexArray(0);

        m_IsSetup = true;
    }

} // namespace JFM
