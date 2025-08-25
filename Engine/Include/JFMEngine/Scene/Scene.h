//
// Scene.h - 现代化场景管理系统
// 基于ECS架构的场景管理，支持序列化和层次结构
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/ECS/World.h"
#include "JFMEngine/ECS/Entity.h"
#include "JFMEngine/ECS/Components.h"
#include "JFMEngine/Resources/ResourceManager.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

namespace JFM {

    // 场景状态枚举
    enum class SceneState {
        Unloaded,
        Loading,
        Active,
        Paused,
        Unloading
    };

    // 场景类 - 基于ECS的现代场景管理
    class JFM_API Scene {
    public:
        Scene(const std::string& name = "Untitled Scene");
        ~Scene();

        // 场景生命周期
        void Initialize();
        void Update(float deltaTime);
        void Render();
        void Destroy();

        // 场景状态管理
        void SetState(SceneState state) { m_State = state; }
        SceneState GetState() const { return m_State; }
        bool IsActive() const { return m_State == SceneState::Active; }

        // 实体管理
        ECS::Entity CreateEntity(const std::string& name = "Entity");
        ECS::Entity CreateEntityWithTag(const std::string& tag);
        void DestroyEntity(ECS::Entity entity);

        // 根据标签查找实体
        ECS::Entity FindEntityByTag(const std::string& tag);
        std::vector<ECS::Entity> FindEntitiesWithTag(const std::string& tag);

        // 根据名称查找实体
        ECS::Entity FindEntityByName(const std::string& name);

        // 层次结构管理
        void SetParent(ECS::Entity child, ECS::Entity parent);
        void RemoveParent(ECS::Entity child);
        std::vector<ECS::Entity> GetChildren(ECS::Entity parent);
        ECS::Entity GetParent(ECS::Entity child);

        // 预制体系统
        ECS::Entity CreateFromPrefab(const std::string& prefabPath);
        void SaveAsPrefab(ECS::Entity entity, const std::string& prefabPath);

        // 场景序列化
        bool SaveToFile(const std::string& filepath);
        bool LoadFromFile(const std::string& filepath);
        std::string SerializeToString() const;
        bool DeserializeFromString(const std::string& data);

        // 相机管理
        void SetActiveCamera(ECS::Entity camera);
        ECS::Entity GetActiveCamera() const { return m_ActiveCamera; }
        ECS::Entity CreateCamera(const std::string& name = "Camera");

        // 光源管理
        ECS::Entity CreateDirectionalLight(const glm::vec3& direction = glm::vec3(0, -1, 0));
        ECS::Entity CreatePointLight(const glm::vec3& position = glm::vec3(0, 0, 0));
        ECS::Entity CreateSpotLight(const glm::vec3& position = glm::vec3(0, 0, 0));

        // 3D对象创建
        ECS::Entity CreateCube(const std::string& name = "Cube");
        ECS::Entity CreateSphere(const std::string& name = "Sphere");
        ECS::Entity CreatePlane(const std::string& name = "Plane");
        ECS::Entity CreateMeshFromFile(const std::string& meshPath, const std::string& name = "Mesh");

        // 获取ECS世界
        ECS::World* GetWorld() { return m_World.get(); }
        const ECS::World* GetWorld() const { return m_World.get(); }

        // 场景信息
        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }
        const std::string& GetFilePath() const { return m_FilePath; }

        // 场景统计
        size_t GetEntityCount() const;
        void GetSceneStatistics(size_t& entityCount, size_t& componentCount, size_t& systemCount) const;

        // 事件回调
        std::function<void(ECS::Entity)> OnEntityCreated;
        std::function<void(ECS::Entity)> OnEntityDestroyed;

    private:
        void SetupDefaultSystems();
        void SerializeEntity(ECS::Entity entity, class JSONValue& entityData) const;
        ECS::Entity DeserializeEntity(const class JSONValue& entityData);

        // 组件序列化辅助函数
        template<typename T>
        void SerializeComponent(ECS::Entity entity, class JSONValue& componentData) const;

        template<typename T>
        void DeserializeComponent(ECS::Entity entity, const class JSONValue& componentData);

    private:
        std::string m_Name;
        std::string m_FilePath;
        SceneState m_State = SceneState::Unloaded;

        std::unique_ptr<ECS::World> m_World;
        ECS::Entity m_ActiveCamera = ECS::NULL_ENTITY;

        // 实体名称映射（用于快速查找）
        std::unordered_map<std::string, ECS::Entity> m_EntityNameMap;
        std::unordered_map<ECS::Entity, std::string> m_EntityNames;

        // 运行时统计
        size_t m_LastEntityCount = 0;
        float m_AccumulatedTime = 0.0f;
    };

    // 场景管理器 - 管理多个场景的加载和切换
    class JFM_API SceneManager {
    public:
        static SceneManager& GetInstance();

        // 场景管理
        std::shared_ptr<Scene> CreateScene(const std::string& name);
        bool LoadScene(const std::string& filepath);
        bool LoadSceneAsync(const std::string& filepath);
        void UnloadScene(const std::string& name);

        // 场景切换
        void SetActiveScene(std::shared_ptr<Scene> scene);
        void SetActiveScene(const std::string& name);
        std::shared_ptr<Scene> GetActiveScene() { return m_ActiveScene; }

        // 场景查询
        std::shared_ptr<Scene> GetScene(const std::string& name);
        bool HasScene(const std::string& name) const;
        std::vector<std::string> GetLoadedSceneNames() const;

        // 更新和渲染
        void Update(float deltaTime);
        void Render();

        // 场景堆栈（用于叠加场景，如UI场景）
        void PushScene(std::shared_ptr<Scene> scene);
        void PopScene();
        std::shared_ptr<Scene> GetTopScene();

    private:
        SceneManager() = default;

        std::unordered_map<std::string, std::shared_ptr<Scene>> m_Scenes;
        std::shared_ptr<Scene> m_ActiveScene;
        std::vector<std::shared_ptr<Scene>> m_SceneStack; // 用于叠加场景

        // 异步加载支持
        std::vector<std::string> m_PendingScenes;
        bool m_IsLoadingScene = false;
    };

} // namespace JFM
