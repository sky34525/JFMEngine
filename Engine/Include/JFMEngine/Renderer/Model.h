//
// Model.h - 3D模型加载和管理系统
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"  // 添加 Texture.h 头文件
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>

// Assimp前向声明
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

namespace JFM {

    // 3D模型类
    class JFM_API Model {
    public:
        Model(const std::string& path);
        ~Model() = default;

        void Draw(const std::shared_ptr<Shader>& shader) const;
        void Update(float deltaTime); // 新增：更新动画
        void SetTransform(const glm::mat4& transform) { m_Transform = transform; }
        const glm::mat4& GetTransform() const { return m_Transform; }

        // 获取模型信息
        size_t GetMeshCount() const { return m_Meshes.size(); }
        const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const { return m_Meshes; }

        // 动画相关方法
        void SetAnimator(std::shared_ptr<class Animator> animator) { m_Animator = animator; }
        std::shared_ptr<class Animator> GetAnimator() const { return m_Animator; }
        bool HasAnimations() const { return !m_AnimationClips.empty(); }
        const std::vector<std::shared_ptr<class AnimationClip>>& GetAnimationClips() const { return m_AnimationClips; }

        // 播放指定动画
        void PlayAnimation(const std::string& animationName);
        void PlayAnimation(int animationIndex);
        void StopAnimation();
        void PauseAnimation();
        void SetAnimationLoop(bool loop);
        void SetAnimationSpeed(float speed);

    private:
        std::vector<std::shared_ptr<Mesh>> m_Meshes;
        std::vector<std::shared_ptr<Material>> m_Materials;
        std::vector<std::shared_ptr<class AnimationClip>> m_AnimationClips; // 动画剪辑
        std::shared_ptr<class Animator> m_Animator; // 动画器
        std::string m_Directory;
        glm::mat4 m_Transform = glm::mat4(1.0f);

        void LoadModel(const std::string& path);
        void ProcessNode(aiNode* node, const aiScene* scene);
        std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<std::shared_ptr<Texture>> LoadMaterialTextures(aiMaterial* mat, int type, const std::string& typeName);

        // 动画处理方法
        void LoadAnimations(const aiScene* scene); // 加载动画数据
    };

    // 模型管理器
    class JFM_API ModelManager {
    public:
        static ModelManager& GetInstance() {
            static ModelManager instance;
            return instance;
        }

        std::shared_ptr<Model> LoadModel(const std::string& path);
        void UnloadModel(const std::string& path);
        void Clear();

    private:
        std::unordered_map<std::string, std::shared_ptr<Model>> m_Models;
    };

    // 基础几何体生成器
    namespace Primitives {
        JFM_API std::shared_ptr<Mesh> CreateCube(float size = 1.0f);
        JFM_API std::shared_ptr<Mesh> CreateSphere(float radius = 1.0f, int segments = 32);
        JFM_API std::shared_ptr<Mesh> CreatePlane(float width = 1.0f, float height = 1.0f);
        JFM_API std::shared_ptr<Mesh> CreateCylinder(float radius = 1.0f, float height = 2.0f, int segments = 32);
    }

}
