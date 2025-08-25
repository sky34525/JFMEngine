//
// Scene3D.h - 3D场景管理系统
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "Camera.h"
#include "Light.h"
#include "Model.h"
#include "JFMEngine/Physics/Physics.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>

namespace JFM {

    // 3D场景节点
    class JFM_API SceneNode {
    public:
        SceneNode(const std::string& name = "");
        virtual ~SceneNode() = default;

        // 层次结构
        void AddChild(std::shared_ptr<SceneNode> child);
        void RemoveChild(std::shared_ptr<SceneNode> child);
        void SetParent(std::shared_ptr<SceneNode> parent);

        const std::vector<std::shared_ptr<SceneNode>>& GetChildren() const { return m_Children; }
        std::shared_ptr<SceneNode> GetParent() const { return m_Parent.lock(); }

        // 变换
        void SetLocalTransform(const glm::mat4& transform) { m_LocalTransform = transform; }
        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::vec3& rotation);
        void SetScale(const glm::vec3& scale);

        glm::vec3 GetPosition() const { return m_Position; }
        glm::vec3 GetRotation() const { return m_Rotation; }
        glm::vec3 GetScale() const { return m_Scale; }

        glm::mat4 GetLocalTransform() const { return m_LocalTransform; }
        glm::mat4 GetWorldTransform() const;

        // 属性
        void SetName(const std::string& name) { m_Name = name; }
        const std::string& GetName() const { return m_Name; }

        void SetVisible(bool visible) { m_Visible = visible; }
        bool IsVisible() const { return m_Visible; }

        // 组件系统
        template<typename T>
        void AddComponent(std::shared_ptr<T> component);

        template<typename T>
        std::shared_ptr<T> GetComponent();

        template<typename T>
        void RemoveComponent();

        // 更新和渲染
        virtual void Update(float deltaTime);
        virtual void Render();

    protected:
        std::string m_Name;
        glm::mat4 m_LocalTransform = glm::mat4(1.0f);
        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::vec3 m_Rotation = glm::vec3(0.0f);
        glm::vec3 m_Scale = glm::vec3(1.0f);

        bool m_Visible = true;

        std::weak_ptr<SceneNode> m_Parent;
        std::vector<std::shared_ptr<SceneNode>> m_Children;

        std::unordered_map<std::string, std::shared_ptr<void>> m_Components;
    };

    // 3D场景
    class JFM_API Scene3D {
    public:
        Scene3D(const std::string& name = "Scene");
        ~Scene3D() = default;

        // 场景管理
        void SetName(const std::string& name) { m_Name = name; }
        const std::string& GetName() const { return m_Name; }

        // 节点管理
        std::shared_ptr<SceneNode> CreateNode(const std::string& name = "");
        void AddNode(std::shared_ptr<SceneNode> node);
        void RemoveNode(std::shared_ptr<SceneNode> node);
        std::shared_ptr<SceneNode> FindNode(const std::string& name);

        // 相机管理
        void SetActiveCamera(std::shared_ptr<Camera> camera) { m_ActiveCamera = camera; }
        std::shared_ptr<Camera> GetActiveCamera() const { return m_ActiveCamera; }
        void AddCamera(const std::string& name, std::shared_ptr<Camera> camera);

        // 光照管理
        void AddLight(std::shared_ptr<Light> light);
        void RemoveLight(std::shared_ptr<Light> light);
        const std::vector<std::shared_ptr<Light>>& GetLights() const { return m_Lights; }

        // 天空盒
        void SetSkybox(std::shared_ptr<Texture> skybox) { m_Skybox = skybox; }
        std::shared_ptr<Texture> GetSkybox() const { return m_Skybox; }

        // 环境设置
        void SetAmbientLight(const glm::vec3& ambient) { m_AmbientLight = ambient; }
        glm::vec3 GetAmbientLight() const { return m_AmbientLight; }

        void SetFogColor(const glm::vec3& color) { m_FogColor = color; }
        void SetFogDensity(float density) { m_FogDensity = density; }
        void EnableFog(bool enable) { m_FogEnabled = enable; }

        // 场景更新和渲染
        void Update(float deltaTime);
        void Render();

        // 场景序列化
        void SaveToFile(const std::string& filepath);
        bool LoadFromFile(const std::string& filepath);

        // 物理世界集成
        void SetPhysicsWorld(std::shared_ptr<PhysicsWorld> world) { m_PhysicsWorld = world; }
        std::shared_ptr<PhysicsWorld> GetPhysicsWorld() const { return m_PhysicsWorld; }

    private:
        std::string m_Name;
        std::shared_ptr<SceneNode> m_RootNode;
        std::vector<std::shared_ptr<SceneNode>> m_Nodes;

        std::shared_ptr<Camera> m_ActiveCamera;
        std::unordered_map<std::string, std::shared_ptr<Camera>> m_Cameras;

        std::vector<std::shared_ptr<Light>> m_Lights;
        std::shared_ptr<Texture> m_Skybox;

        glm::vec3 m_AmbientLight = glm::vec3(0.1f);
        glm::vec3 m_FogColor = glm::vec3(0.5f);
        float m_FogDensity = 0.01f;
        bool m_FogEnabled = false;

        std::shared_ptr<PhysicsWorld> m_PhysicsWorld;
    };

    // 场景管理器
    class JFM_API SceneManager {
    public:
        static SceneManager& GetInstance() {
            static SceneManager instance;
            return instance;
        }

        void SetActiveScene(std::shared_ptr<Scene3D> scene);
        std::shared_ptr<Scene3D> GetActiveScene() const { return m_ActiveScene; }

        void AddScene(const std::string& name, std::shared_ptr<Scene3D> scene);
        void RemoveScene(const std::string& name);
        std::shared_ptr<Scene3D> GetScene(const std::string& name);

        void Update(float deltaTime);
        void Render();

    private:
        std::shared_ptr<Scene3D> m_ActiveScene;
        std::unordered_map<std::string, std::shared_ptr<Scene3D>> m_Scenes;
    };

}
