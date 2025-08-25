//
// Mesh.h - 网格数据定义
// 包含顶点、法线、索引等几何数据
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "Vertex.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace JFM {

    // Mesh 类定义
    class JFM_API Mesh {
    public:
        std::vector<Vertex> Vertices;
        std::vector<uint32_t> Indices;
        std::vector<std::shared_ptr<Texture>> Textures; // 添加纹理支持

        // OpenGL 渲染相关
        uint32_t VAO = 0, VBO = 0, EBO = 0;

        // 更新构造函数以支持纹理
        Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices,
             const std::vector<std::shared_ptr<Texture>>& textures);//将纹理存贮纹理引用，实际的纹理绑定在渲染时发生
        ~Mesh();

        void Draw() const;
        void SetupMesh();

    private:
        bool m_IsSetup = false;
    };

    class JFM_API MeshGenerator {
    public:
        // 生成带法线的立方体
        static std::vector<Vertex> GenerateCubeVertices() {
            return {
                // 前面
                {{-0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
                {{ 0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
                {{ 0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
                {{-0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},

                // 后面
                {{-0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
                {{-0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
                {{ 0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},
                {{ 0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},

                // 左面
                {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
                {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
                {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
                {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},

                // 右面
                {{ 0.5f,  0.5f,  0.5f}, {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
                {{ 0.5f, -0.5f,  0.5f}, {1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
                {{ 0.5f, -0.5f, -0.5f}, {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
                {{ 0.5f,  0.5f, -0.5f}, {1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},

                // 下面
                {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
                {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
                {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}},
                {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},

                // 上面
                {{-0.5f,  0.5f, -0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
                {{-0.5f,  0.5f,  0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
                {{ 0.5f,  0.5f,  0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
                {{ 0.5f,  0.5f, -0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}}
            };
        }

        static std::vector<uint32_t> GenerateCubeIndices() {
            return {
                0,  1,  2,      0,  2,  3,    // 前面
                4,  5,  6,      4,  6,  7,    // 后面
                8,  9,  10,     8,  10, 11,   // 左面
                12, 13, 14,     12, 14, 15,   // 右面
                16, 17, 18,     16, 18, 19,   // 下面
                20, 21, 22,     20, 22, 23    // 上面
            };
        }

        // 生成球体
        static std::vector<Vertex> GenerateSphereVertices(float radius = 1.0f, int segments = 32, int rings = 16) {
            std::vector<Vertex> vertices;

            for (int ring = 0; ring <= rings; ++ring) {
                float phi = M_PI * ring / rings;
                float y = cos(phi);
                float sinPhi = sin(phi);

                for (int segment = 0; segment <= segments; ++segment) {
                    float theta = 2.0f * M_PI * segment / segments;
                    float x = sinPhi * cos(theta);
                    float z = sinPhi * sin(theta);

                    Vertex vertex;
                    vertex.Position = glm::vec3(x, y, z) * radius;
                    vertex.Normal = glm::vec3(x, y, z);
                    vertex.TexCoords = glm::vec2((float)segment / segments, (float)ring / rings);
                    vertices.push_back(vertex);
                }
            }

            return vertices;
        }

        static std::vector<uint32_t> GenerateSphereIndices(int segments = 32, int rings = 16) {
            std::vector<uint32_t> indices;

            for (int ring = 0; ring < rings; ++ring) {
                for (int segment = 0; segment < segments; ++segment) {
                    int current = ring * (segments + 1) + segment;
                    int next = current + segments + 1;

                    indices.push_back(current);
                    indices.push_back(next);
                    indices.push_back(current + 1);

                    indices.push_back(current + 1);
                    indices.push_back(next);
                    indices.push_back(next + 1);
                }
            }

            return indices;
        }

        // 生成平面
        static std::vector<Vertex> GeneratePlaneVertices(float width = 1.0f, float height = 1.0f) {
            float halfWidth = width * 0.5f;
            float halfHeight = height * 0.5f;

            return {
                {{-halfWidth, 0.0f, -halfHeight}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
                {{ halfWidth, 0.0f, -halfHeight}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                {{ halfWidth, 0.0f,  halfHeight}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
                {{-halfWidth, 0.0f,  halfHeight}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
            };
        }

        static std::vector<uint32_t> GeneratePlaneIndices() {
            return {0, 1, 2, 0, 2, 3};
        }
    };

}
