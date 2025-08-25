//
// GeometryGenerator.h - 基础几何体生成器
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/Renderer/VertexArray.h"
#include "JFMEngine/Renderer/Shader.h"
#include <memory>
#include <glm/glm.hpp>

namespace JFM {

    // 几何体数据结构
    struct GeometryData {
        std::vector<float> vertices;      // 顶点数据 (位置 + 法线 + 纹理坐标)
        std::vector<uint32_t> indices;   // 索引数据
        uint32_t stride = 8;              // 每个顶点的浮点数数量 (3位置 + 3法线 + 2纹理坐标)
    };

    class JFM_API GeometryGenerator {
    public:
        // 创建立方体几何体
        static GeometryData CreateCube(float size = 1.0f);
        
        // 创建球体几何体
        static GeometryData CreateSphere(float radius = 0.5f, uint32_t rings = 20, uint32_t sectors = 20);
        
        // 创建平面几何体
        static GeometryData CreatePlane(float width = 1.0f, float height = 1.0f);
        
        // 创建圆柱体几何体
        static GeometryData CreateCylinder(float bottomRadius = 0.5f, float topRadius = 0.5f, 
                                         float height = 1.0f, uint32_t slices = 20);
        
        // 创建圆锥体几何体
        static GeometryData CreateCone(float radius = 0.5f, float height = 1.0f, uint32_t slices = 20);
        
        // 从几何体数据创建VertexArray
        static std::shared_ptr<VertexArray> CreateVertexArray(const GeometryData& geometry);

        // 渲染几何体方法
        static void RenderGeometry(const std::shared_ptr<VertexArray>& va, uint32_t indexCount,
                                  const std::shared_ptr<Shader>& shader, 
                                  const glm::vec3& position, const glm::vec3& scale,
                                  const glm::vec3& color, const glm::mat4& viewProjection);

    private:
        // 计算球体上的点
        static glm::vec3 GetSpherePoint(float radius, float phi, float theta);
        
        // 计算法线
        static glm::vec3 CalculateNormal(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
        
        // 添加三角形到几何体数据
        static void AddTriangle(GeometryData& data, 
                               const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                               const glm::vec3& n0, const glm::vec3& n1, const glm::vec3& n2,
                               const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec2& uv2);
    };

}