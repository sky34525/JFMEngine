//
// Scene.cpp - 现代化场景管理系统实现
//

#include "JFMEngine/Scene/Scene.h"
#include "JFMEngine/ECS/Systems/TransformSystem.h"
#include "JFMEngine/ECS/Systems/RenderSystem.h"
#include "JFMEngine/Utils/Log.h"
#include <algorithm>

namespace JFM {

    Scene::Scene(const std::string& name)
        : m_Name(name), m_State(SceneState::Unloaded) {
        m_World = std::make_unique<ECS::World>();
    }

    Scene::~Scene() {
        Destroy();
    }

    void Scene::Initialize() {
        if (m_State != SceneState::Unloaded) {
            return;
        }

        SetupDefaultSystems();
        m_State = SceneState::Active;
    }

    void Scene::Update(float deltaTime) {
        if (m_State != SceneState::Active) {
            return;
        }

        m_AccumulatedTime += deltaTime;
        m_World->Update(deltaTime);
    }

    void Scene::Render() {
        if (m_State != SceneState::Active) {
            return;
        }

        m_World->Render();
    }

    void Scene::Destroy() {
        if (m_State == SceneState::Unloaded) {
            return;
        }

        m_State = SceneState::Unloading;

        // 清理所有实体
        m_EntityNameMap.clear();
        m_EntityNames.clear();

        // World的析构函数会处理剩余的清理工作
        m_World.reset();

        m_State = SceneState::Unloaded;
    }

    ECS::Entity Scene::CreateEntity(const std::string& name) {
        ECS::Entity entity = m_World->CreateEntity();

        // 添加基础组件
        m_World->AddComponent<ECS::TagComponent>(entity, name);
        m_World->AddComponent<ECS::TransformComponent>(entity);
        m_World->AddComponent<ECS::ActiveComponent>(entity, true);

        // 建立名称映射
        m_EntityNames[entity] = name;
        m_EntityNameMap[name] = entity;

        if (OnEntityCreated) {
            OnEntityCreated(entity);
        }

        return entity;
    }

    ECS::Entity Scene::CreateEntityWithTag(const std::string& tag) {
        ECS::Entity entity = CreateEntity(tag);
        return entity;
    }

    void Scene::DestroyEntity(ECS::Entity entity) {
        if (!m_World->IsEntityValid(entity)) {
            return;
        }

        // 移除子节点关系
        if (m_World->HasComponent<ECS::ChildrenComponent>(entity)) {
            const auto& children = m_World->GetComponent<ECS::ChildrenComponent>(entity);
            for (ECS::Entity child : children.Children) {
                RemoveParent(child);
            }
        }

        // 移除父节点关系
        if (m_World->HasComponent<ECS::ParentComponent>(entity)) {
            RemoveParent(entity);
        }

        // 从名称映射中移除
        auto nameIt = m_EntityNames.find(entity);
        if (nameIt != m_EntityNames.end()) {
            m_EntityNameMap.erase(nameIt->second);
            m_EntityNames.erase(nameIt);
        }

        if (OnEntityDestroyed) {
            OnEntityDestroyed(entity);
        }

        m_World->DestroyEntity(entity);
    }

    ECS::Entity Scene::FindEntityByTag(const std::string& tag) {
        auto entities = m_World->GetEntitiesWithComponents<ECS::TagComponent>();

        for (ECS::Entity entity : entities) {
            const auto& tagComp = m_World->GetComponent<ECS::TagComponent>(entity);
            if (tagComp.Tag == tag) {
                return entity;
            }
        }

        return ECS::NULL_ENTITY;
    }

    std::vector<ECS::Entity> Scene::FindEntitiesWithTag(const std::string& tag) {
        std::vector<ECS::Entity> result;
        auto entities = m_World->GetEntitiesWithComponents<ECS::TagComponent>();

        for (ECS::Entity entity : entities) {
            const auto& tagComp = m_World->GetComponent<ECS::TagComponent>(entity);
            if (tagComp.Tag == tag) {
                result.push_back(entity);
            }
        }

        return result;
    }

    ECS::Entity Scene::FindEntityByName(const std::string& name) {
        auto it = m_EntityNameMap.find(name);
        return (it != m_EntityNameMap.end()) ? ECS::Entity(it->second) : ECS::Entity(ECS::NULL_ENTITY);
    }

    void Scene::SetParent(ECS::Entity child, ECS::Entity parent) {
        if (!m_World->IsEntityValid(child) || !m_World->IsEntityValid(parent)) {
            return;
        }

        // 移除旧的父子关系
        RemoveParent(child);

        // 设置新的父子关系
        if (!m_World->HasComponent<ECS::ParentComponent>(child)) {
            m_World->AddComponent<ECS::ParentComponent>(child);
        }
        m_World->GetComponent<ECS::ParentComponent>(child).Parent = parent;

        // 更新父节点的子节点列表
        if (!m_World->HasComponent<ECS::ChildrenComponent>(parent)) {
            m_World->AddComponent<ECS::ChildrenComponent>(parent);
        }
        m_World->GetComponent<ECS::ChildrenComponent>(parent).AddChild(child);

        // 标记变换为dirty
        if (m_World->HasComponent<ECS::TransformComponent>(child)) {
            m_World->GetComponent<ECS::TransformComponent>(child).IsDirty = true;
        }
    }

    void Scene::RemoveParent(ECS::Entity child) {
        if (!m_World->HasComponent<ECS::ParentComponent>(child)) {
            return;
        }

        auto& parentComp = m_World->GetComponent<ECS::ParentComponent>(child);
        ECS::Entity parent = parentComp.Parent;

        if (m_World->IsEntityValid(parent) && m_World->HasComponent<ECS::ChildrenComponent>(parent)) {
            m_World->GetComponent<ECS::ChildrenComponent>(parent).RemoveChild(child);
        }

        m_World->RemoveComponent<ECS::ParentComponent>(child);

        // 标记变换为dirty
        if (m_World->HasComponent<ECS::TransformComponent>(child)) {
            m_World->GetComponent<ECS::TransformComponent>(child).IsDirty = true;
        }
    }

    std::vector<ECS::Entity> Scene::GetChildren(ECS::Entity parent) {
        if (!m_World->HasComponent<ECS::ChildrenComponent>(parent)) {
            return {};
        }

        return m_World->GetComponent<ECS::ChildrenComponent>(parent).Children;
    }

    ECS::Entity Scene::GetParent(ECS::Entity child) {
        if (!m_World->HasComponent<ECS::ParentComponent>(child)) {
            return ECS::NULL_ENTITY;
        }

        return m_World->GetComponent<ECS::ParentComponent>(child).Parent;
    }

    void Scene::SetActiveCamera(ECS::Entity camera) {
        // 取消之前的主相机
        if (m_ActiveCamera != ECS::NULL_ENTITY && m_World->IsEntityValid(m_ActiveCamera)) {
            if (m_World->HasComponent<ECS::CameraComponent>(m_ActiveCamera)) {
                m_World->GetComponent<ECS::CameraComponent>(m_ActiveCamera).IsPrimary = false;
            }
        }

        // 设置新的主相机
        m_ActiveCamera = camera;
        if (camera != ECS::NULL_ENTITY && m_World->IsEntityValid(camera)) {
            if (m_World->HasComponent<ECS::CameraComponent>(camera)) {
                m_World->GetComponent<ECS::CameraComponent>(camera).IsPrimary = true;
            }
        }
    }

    ECS::Entity Scene::CreateCamera(const std::string& name) {
        ECS::Entity camera = CreateEntity(name);
        m_World->AddComponent<ECS::CameraComponent>(camera);

        // 如果这是第一个相机，设为主相机
        if (m_ActiveCamera == ECS::NULL_ENTITY) {
            SetActiveCamera(camera);
        }

        return camera;
    }

    ECS::Entity Scene::CreateDirectionalLight(const glm::vec3& direction) {
        ECS::Entity light = CreateEntity("Directional Light");
        auto& lightComp = m_World->AddComponent<ECS::LightComponent>(light);
        lightComp.Type = LightType::Directional;

        // 设置方向
        auto& transform = m_World->GetComponent<ECS::TransformComponent>(light);
        glm::vec3 forward = glm::normalize(direction);
        transform.Rotation = glm::quatLookAt(forward, glm::vec3(0, 1, 0));

        return light;
    }

    ECS::Entity Scene::CreatePointLight(const glm::vec3& position) {
        ECS::Entity light = CreateEntity("Point Light");
        auto& lightComp = m_World->AddComponent<ECS::LightComponent>(light);
        lightComp.Type = LightType::Point;

        auto& transform = m_World->GetComponent<ECS::TransformComponent>(light);
        transform.Position = position;

        return light;
    }

    ECS::Entity Scene::CreateSpotLight(const glm::vec3& position) {
        ECS::Entity light = CreateEntity("Spot Light");
        auto& lightComp = m_World->AddComponent<ECS::LightComponent>(light);
        lightComp.Type = LightType::Spot;

        auto& transform = m_World->GetComponent<ECS::TransformComponent>(light);
        transform.Position = position;

        return light;
    }

    size_t Scene::GetEntityCount() const {
        return m_World->GetEntityCount();
    }

    void Scene::GetSceneStatistics(size_t& entityCount, size_t& componentCount, size_t& systemCount) const {
        entityCount = m_World->GetEntityCount();
        componentCount = 0; // TODO: 实现组件计数
        systemCount = m_World->GetSystemCount();
    }

    void Scene::SetupDefaultSystems() {
        // 添加核心系统
        m_World->AddSystem<ECS::TransformSystem>();
        m_World->AddSystem<ECS::RenderSystem>();

    }

    // ========== SceneManager Implementation ==========

    SceneManager& SceneManager::GetInstance() {
        static SceneManager instance;
        return instance;
    }

    std::shared_ptr<Scene> SceneManager::CreateScene(const std::string& name) {
        if (HasScene(name)) {
            return GetScene(name);
        }

        auto scene = std::make_shared<Scene>(name);
        m_Scenes[name] = scene;
        return scene;
    }

    void SceneManager::SetActiveScene(std::shared_ptr<Scene> scene) {
        if (m_ActiveScene) {
            m_ActiveScene->SetState(SceneState::Paused);
        }

        m_ActiveScene = scene;
        if (m_ActiveScene) {
            if (m_ActiveScene->GetState() == SceneState::Unloaded) {
                m_ActiveScene->Initialize();
            } else {
                m_ActiveScene->SetState(SceneState::Active);
            }
        }
    }

    void SceneManager::SetActiveScene(const std::string& name) {
        auto scene = GetScene(name);
        if (scene) {
            SetActiveScene(scene);
        }
    }

    std::shared_ptr<Scene> SceneManager::GetScene(const std::string& name) {
        auto it = m_Scenes.find(name);
        return (it != m_Scenes.end()) ? it->second : nullptr;
    }

    bool SceneManager::HasScene(const std::string& name) const {
        return m_Scenes.find(name) != m_Scenes.end();
    }

    std::vector<std::string> SceneManager::GetLoadedSceneNames() const {
        std::vector<std::string> names;
        for (const auto& pair : m_Scenes) {
            names.push_back(pair.first);
        }
        return names;
    }

    void SceneManager::Update(float deltaTime) {
        if (m_ActiveScene) {
            m_ActiveScene->Update(deltaTime);
        }

        // 更新场景堆栈中的场景
        for (auto& scene : m_SceneStack) {
            scene->Update(deltaTime);
        }
    }

    void SceneManager::Render() {
        if (m_ActiveScene) {
            m_ActiveScene->Render();
        }

        // 渲染场景堆栈中的场景（叠加渲染）
        for (auto& scene : m_SceneStack) {
            scene->Render();
        }
    }

} // namespace JFM
