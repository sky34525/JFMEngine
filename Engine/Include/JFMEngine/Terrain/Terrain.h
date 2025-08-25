//
// Terrain.h - 3D地形系统
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/Renderer/Texture.h"
#include "JFMEngine/Renderer/Shader.h"
#include "JFMEngine/Renderer/Mesh.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace JFM {

    // 地形配置
    struct TerrainConfig {
        uint32_t Width = 256;
        uint32_t Height = 256;
        float Scale = 1.0f;
        float HeightScale = 50.0f;

        // 纹理混合
        std::shared_ptr<Texture> HeightMap = nullptr;
        std::shared_ptr<Texture> Texture1 = nullptr;  // 草地
        std::shared_ptr<Texture> Texture2 = nullptr;  // 石头
        std::shared_ptr<Texture> Texture3 = nullptr;  // 雪
        std::shared_ptr<Texture> Texture4 = nullptr;  // 沙土
        std::shared_ptr<Texture> BlendMap = nullptr;  // 混合贴图

        // LOD设置
        bool EnableLOD = true;
        uint32_t LODLevels = 4;
        float LODDistance = 100.0f;
    };

    // 地形块
    class JFM_API TerrainChunk {
    public:
        TerrainChunk(int x, int z, const TerrainConfig& config);
        ~TerrainChunk();

        void Generate();
        void Render(const std::shared_ptr<Shader>& shader);

        float GetHeightAt(float x, float z) const;
        glm::vec3 GetNormalAt(float x, float z) const;

        bool IsInFrustum(const glm::mat4& viewProj) const;
        void UpdateLOD(const glm::vec3& cameraPos);

    private:
        int m_ChunkX, m_ChunkZ;
        TerrainConfig m_Config;

        std::vector<float> m_Heights;
        std::vector<glm::vec3> m_Normals;

        std::shared_ptr<Mesh> m_Mesh;
        uint32_t m_CurrentLOD = 0;
        std::vector<std::shared_ptr<Mesh>> m_LODMeshes;

        void GenerateHeights();
        void GenerateNormals();
        void GenerateMesh(uint32_t lodLevel = 0);
        float SampleHeight(int x, int z) const;
    };

    // 地形系统
    class JFM_API TerrainSystem {
    public:
        TerrainSystem(const TerrainConfig& config);
        ~TerrainSystem();

        void SetConfig(const TerrainConfig& config) { m_Config = config; }
        const TerrainConfig& GetConfig() const { return m_Config; }

        // 地形操作
        void Generate();
        void LoadFromHeightmap(const std::string& heightmapPath);
        void SaveHeightmap(const std::string& outputPath);

        // 高度查询
        float GetHeightAt(float worldX, float worldZ) const;
        glm::vec3 GetNormalAt(float worldX, float worldZ) const;

        // 动态地形修改
        void ModifyHeight(float worldX, float worldZ, float radius, float strength, bool raise = true);
        void SmoothTerrain(float worldX, float worldZ, float radius, float strength);
        void FlattenTerrain(float worldX, float worldZ, float radius, float targetHeight);

        // 渲染
        void Render(const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const glm::vec3& cameraPos);

        // 碰撞检测
        bool RaycastTerrain(const glm::vec3& origin, const glm::vec3& direction, glm::vec3& hitPoint) const;

    private:
        TerrainConfig m_Config;
        std::vector<std::unique_ptr<TerrainChunk>> m_Chunks;
        std::shared_ptr<Shader> m_TerrainShader;

        uint32_t m_ChunksX, m_ChunksZ;

        void InitializeChunks();
        void LoadShader();
        TerrainChunk* GetChunkAt(float worldX, float worldZ) const;
    };

    // 植被系统
    struct VegetationInstance {
        glm::vec3 Position;
        glm::vec3 Scale;
        float Rotation;
        uint32_t Type;
    };

    class JFM_API VegetationSystem {
    public:
        struct VegetationType {
            std::shared_ptr<Model> Model;
            float Density = 0.1f;        // 密度 (0-1)
            float MinHeight = 0.0f;       // 最小高度
            float MaxHeight = 100.0f;     // 最大高度
            float MinSlope = 0.0f;        // 最小坡度
            float MaxSlope = 45.0f;       // 最大坡度
            glm::vec2 ScaleRange = glm::vec2(0.8f, 1.2f);
        };

        void AddVegetationType(const VegetationType& type) { m_Types.push_back(type); }
        void GenerateVegetation(const TerrainSystem& terrain, const glm::vec2& area);
        void Render(const glm::mat4& viewMatrix, const glm::mat4& projMatrix);

        void ClearVegetation() { m_Instances.clear(); }

    private:
        std::vector<VegetationType> m_Types;
        std::vector<VegetationInstance> m_Instances;

        bool ShouldPlaceVegetation(const VegetationType& type, float height, float slope) const;
        void RenderInstancedType(uint32_t typeIndex, const glm::mat4& viewMatrix, const glm::mat4& projMatrix);
    };

}
