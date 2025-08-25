//
// GeometryGenerator.cpp - 基础几何体生成器实现
//

#include "JFMEngine/Renderer/GeometryGenerator.h"
#include "JFMEngine/Renderer/Buffer.h"
#include "JFMEngine/Renderer/LightingManager.h"
#include "JFMEngine/Utils/Log.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <cmath>

namespace JFM {

    GeometryData GeometryGenerator::CreateCube(float size) {
        GeometryData data;
        float halfSize = size * 0.5f;
        
        // 立方体顶点数据 (位置 + 法线 + 纹理坐标)
        std::vector<float> vertices = {
            // 前面 (z = +halfSize)
            -halfSize, -halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, // 0左下
             halfSize, -halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f, // 1右下
             halfSize,  halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, // 2右上
            -halfSize,  halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, // 3左上
            
            // 后面 (z = -halfSize)
            -halfSize, -halfSize, -halfSize,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, // 4
             halfSize, -halfSize, -halfSize,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // 5
             halfSize,  halfSize, -halfSize,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f, // 6
            -halfSize,  halfSize, -halfSize,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // 7
            
            // 左面 (x = -halfSize)
            -halfSize, -halfSize, -halfSize, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, // 8
            -halfSize, -halfSize,  halfSize, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // 9
            -halfSize,  halfSize,  halfSize, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, // 10
            -halfSize,  halfSize, -halfSize, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // 11
            
            // 右面 (x = +halfSize)
             halfSize, -halfSize,  halfSize,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, // 12
             halfSize, -halfSize, -halfSize,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // 13
             halfSize,  halfSize, -halfSize,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, // 14
             halfSize,  halfSize,  halfSize,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // 15
            
            // 上面 (y = +halfSize)
            -halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, // 16
             halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, // 17
             halfSize,  halfSize, -halfSize,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, // 18
            -halfSize,  halfSize, -halfSize,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, // 19
            
            // 下面 (y = -halfSize)
            -halfSize, -halfSize, -halfSize,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, // 20
             halfSize, -halfSize, -halfSize,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f, // 21
             halfSize, -halfSize,  halfSize,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f, // 22
            -halfSize, -halfSize,  halfSize,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f  // 23
        };
        
        std::vector<uint32_t> indices = {
            // 前面 (逆时针从外面看)
            0,  1,  2,   2,  3,  0,
            // 后面 (逆时针从外面看) - 修正绕序
            4,  5,  6,   6,  7,  4,
            // 左面 (逆时针从外面看)
            8,  9,  10,  10, 11, 8,
            // 右面 (逆时针从外面看)
            12, 15, 14,  14, 13, 12,
            // 上面 (逆时针从外面看)
            16, 17, 18,  18, 19, 16,
            // 下面 (逆时针从外面看)
            20, 21, 22,  22, 23, 20
        };
        
        
        data.vertices = std::move(vertices);
        data.indices = std::move(indices);
        data.stride = 8; // 3位置 + 3法线 + 2纹理坐标
        
        
        return data;
    }

    //UV球体
    GeometryData GeometryGenerator::CreateSphere(float radius, uint32_t rings, uint32_t sectors) {
        GeometryData data;
        
        // 生成球体顶点
        for (uint32_t r = 0; r <= rings; ++r) {
            float phi = static_cast<float>(M_PI) * r / rings;
            float y = radius * std::cos(phi);
            float ringRadius = radius * std::sin(phi);
            
            for (uint32_t s = 0; s <= sectors; ++s) {
                float theta = 2.0f * static_cast<float>(M_PI) * s / sectors;
                float x = ringRadius * std::cos(theta);
                float z = ringRadius * std::sin(theta);
                
                // 位置
                data.vertices.push_back(x);
                data.vertices.push_back(y);
                data.vertices.push_back(z);
                
                // 法线（对于球体，法线就是归一化的位置向量）
                glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
                data.vertices.push_back(normal.x);
                data.vertices.push_back(normal.y);
                data.vertices.push_back(normal.z);
                
                // 纹理坐标
                float u = static_cast<float>(s) / sectors;
                float v = static_cast<float>(r) / rings;
                data.vertices.push_back(u);
                data.vertices.push_back(v);
            }
        }
        
        // 生成索引
        for (uint32_t r = 0; r < rings; ++r) {
            for (uint32_t s = 0; s < sectors; ++s) {
                uint32_t current = r * (sectors + 1) + s;
                uint32_t next = current + sectors + 1;
                
                // 第一个三角形
                data.indices.push_back(current);
                data.indices.push_back(next);
                data.indices.push_back(current + 1);
                
                // 第二个三角形
                data.indices.push_back(current + 1);
                data.indices.push_back(next);
                data.indices.push_back(next + 1);
            }
        }
        
        data.stride = 8; // 3位置 + 3法线 + 2纹理坐标
        
        
        return data;
    }
    
    GeometryData GeometryGenerator::CreatePlane(float width, float height) {
        GeometryData data;
        
        float halfWidth = width * 0.5f;
        float halfHeight = height * 0.5f;
        
        // 平面顶点数据（Y轴向上）
        std::vector<float> vertices = {
            // 位置                法线           纹理坐标
            -halfWidth, 0.0f, -halfHeight,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // 左下
             halfWidth, 0.0f, -halfHeight,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // 右下
             halfWidth, 0.0f,  halfHeight,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // 右上
            -halfWidth, 0.0f,  halfHeight,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f  // 左上
        };
        
        std::vector<uint32_t> indices = {
            0, 1, 2,
            2, 3, 0
        };
        
        data.vertices = std::move(vertices);
        data.indices = std::move(indices);
        data.stride = 8;
        
        
        return data;
    }
    
    GeometryData GeometryGenerator::CreateCylinder(float bottomRadius, float topRadius, 
                                                  float height, uint32_t slices) {
        GeometryData data;
        
        float halfHeight = height * 0.5f;

        
        // 生成侧面顶点
        for (uint32_t i = 0; i <= slices; ++i) {
            float theta = 2.0f * static_cast<float>(M_PI) * i / slices;
            float cosTheta = std::cos(theta);
            float sinTheta = std::sin(theta);
            
            // 底部顶点
            data.vertices.push_back(bottomRadius * cosTheta); // x
            data.vertices.push_back(-halfHeight);             // y
            data.vertices.push_back(bottomRadius * sinTheta); // z
            
            // 法线（侧面法线）
            glm::vec3 normal = glm::normalize(glm::vec3(cosTheta, 0.0f, sinTheta));
            data.vertices.push_back(normal.x);
            data.vertices.push_back(normal.y);
            data.vertices.push_back(normal.z);
            
            // 纹理坐标
            float u = static_cast<float>(i) / slices;
            data.vertices.push_back(u);
            data.vertices.push_back(0.0f);
            
            // 顶部顶点
            data.vertices.push_back(topRadius * cosTheta);    // x
            data.vertices.push_back(halfHeight);              // y
            data.vertices.push_back(topRadius * sinTheta);    // z
            
            // 法线
            data.vertices.push_back(normal.x);
            data.vertices.push_back(normal.y);
            data.vertices.push_back(normal.z);
            
            // 纹理坐标
            data.vertices.push_back(u);
            data.vertices.push_back(1.0f);
        }
        
        // 生成侧面索引
        for (uint32_t i = 0; i < slices; ++i) {
            uint32_t bottomLeft = i * 2;
            uint32_t bottomRight = (i + 1) * 2;
            uint32_t topLeft = bottomLeft + 1;
            uint32_t topRight = bottomRight + 1;
            
            // 第一个三角形
            data.indices.push_back(bottomLeft);
            data.indices.push_back(bottomRight);
            data.indices.push_back(topLeft);
            
            // 第二个三角形
            data.indices.push_back(topLeft);
            data.indices.push_back(bottomRight);
            data.indices.push_back(topRight);
        }
        
        data.stride = 8;
        
        
        return data;
    }
    
    GeometryData GeometryGenerator::CreateCone(float radius, float height, uint32_t slices) {
        return CreateCylinder(radius, 0.0f, height, slices);
    }
    
    std::shared_ptr<VertexArray> GeometryGenerator::CreateVertexArray(const GeometryData& geometry) {
        auto vertexArray = VertexArray::Create();
        
        auto vertexBuffer = VertexBuffer::Create(const_cast<float*>(geometry.vertices.data()), 
                                               geometry.vertices.size() * sizeof(float));
        
        BufferLayout layout = {
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Normal" },
            { ShaderDataType::Float2, "a_TexCoord" }
        };
        vertexBuffer->SetLayout(layout);
        
        auto indexBuffer = IndexBuffer::Create(const_cast<uint32_t*>(geometry.indices.data()), geometry.indices.size());
        
        vertexArray->AddVertexBuffer(vertexBuffer);
        vertexArray->SetIndexBuffer(indexBuffer);
        
        return vertexArray;
    }
    
    void GeometryGenerator::RenderGeometry(const std::shared_ptr<VertexArray>& va, uint32_t indexCount,
                                          const std::shared_ptr<Shader>& shader, 
                                          const glm::vec3& position, const glm::vec3& scale,
                                          const glm::vec3& color, const glm::mat4& viewProjection) {
        if (!va || !shader) {
            return;
        }

        // 创建模型矩阵
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, scale);
        

        // 设置必要的矩阵uniform
        shader->SetMat4("u_ModelMatrix", model);
        shader->SetMat4("u_ViewProjectionMatrix", viewProjection);
        
        // 只对需要高级功能的着色器设置这些uniform（忽略警告）
        shader->SetMat3("u_NormalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        
        // 设置材质属性（对于不需要的着色器会产生警告但不影响功能）
        MaterialProperties material = {
            color * 0.3f,      // 环境光
            color,             // 漫反射
            glm::vec3(1.0f),   // 镜面反射
            32.0f              // 光泽度
        };
        LightingManager::GetInstance().ApplyMaterial(shader, material);

        // 绑定并渲染
        va->Bind();
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        
    }
    
    glm::vec3 GeometryGenerator::GetSpherePoint(float radius, float phi, float theta) {
        float x = radius * std::sin(phi) * std::cos(theta);
        float y = radius * std::cos(phi);
        float z = radius * std::sin(phi) * std::sin(theta);
        return glm::vec3(x, y, z);
    }
    
    glm::vec3 GeometryGenerator::CalculateNormal(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
        glm::vec3 v1 = p2 - p1;
        glm::vec3 v2 = p3 - p1;
        return glm::normalize(glm::cross(v1, v2));
    }
    
    void GeometryGenerator::AddTriangle(GeometryData& data, 
                                      const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                                      const glm::vec3& n0, const glm::vec3& n1, const glm::vec3& n2,
                                      const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec2& uv2) {
        uint32_t baseIndex = static_cast<uint32_t>(data.vertices.size() / data.stride);
        
        // 添加第一个顶点
        data.vertices.insert(data.vertices.end(), {v0.x, v0.y, v0.z, n0.x, n0.y, n0.z, uv0.x, uv0.y});
        // 添加第二个顶点
        data.vertices.insert(data.vertices.end(), {v1.x, v1.y, v1.z, n1.x, n1.y, n1.z, uv1.x, uv1.y});
        // 添加第三个顶点
        data.vertices.insert(data.vertices.end(), {v2.x, v2.y, v2.z, n2.x, n2.y, n2.z, uv2.x, uv2.y});
        
        // 添加索引
        data.indices.insert(data.indices.end(), {baseIndex, baseIndex + 1, baseIndex + 2});
    }

}