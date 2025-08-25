//
// Scene.h - 场景管理系统
// 管理场景对象、层次结构和渲染队列
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/Renderer/Camera.h"
#include "JFMEngine/Renderer/Light.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace JFM {

    // 变换组件
    struct Transform {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec3 Rotation = glm::vec3(0.0f);
        glm::vec3 Scale = glm::vec3(1.0f);

        glm::mat4 GetMatrix() const;
        void SetMatrix(const glm::mat4& matrix);
    };

    // 场景对象基类
    class JFM_API SceneObject {
    public:
        SceneObject(const std::string& name = "GameObject");
        virtual ~SceneObject() = default;

        Transform& GetTransform() { return m_Transform; }
        const Transform& GetTransform() const { return m_Transform; }

        void SetName(const std::string& name) { m_Name = name; }
        const std::string& GetName() const { return m_Name; }

        void SetActive(bool active) { m_Active = active; }
        bool IsActive() const { return m_Active; }

        // 层次结构
        void AddChild(std::shared_ptr<SceneObject> child);
        void RemoveChild(std::shared_ptr<SceneObject> child);
        const std::vector<std::shared_ptr<SceneObject>>& GetChildren() const { return m_Children; }

        virtual void Update(float deltaTime) {}
        virtual void Render() {}

    protected:
        std::string m_Name;
        Transform m_Transform;
        bool m_Active = true;

        std::weak_ptr<SceneObject> m_Parent;
        std::vector<std::shared_ptr<SceneObject>> m_Children;
    };

    // 场景类
    class JFM_API Scene {
    public:
        Scene(const std::string& name = "Scene");
        ~Scene() = default;

        void Update(float deltaTime);
        void Render(const Camera& camera);

        // 对象管理
        std::shared_ptr<SceneObject> CreateObject(const std::string& name = "GameObject");
        void AddObject(std::shared_ptr<SceneObject> object);
        void RemoveObject(std::shared_ptr<SceneObject> object);
        std::shared_ptr<SceneObject> FindObject(const std::string& name);

        // 光照管理
        void AddLight(std::shared_ptr<Light> light);
        void RemoveLight(std::shared_ptr<Light> light);
        const std::vector<std::shared_ptr<Light>>& GetLights() const { return m_Lights; }

        // 摄像机管理
        void SetMainCamera(std::shared_ptr<Camera> camera) { m_MainCamera = camera; }
        std::shared_ptr<Camera> GetMainCamera() const { return m_MainCamera; }

        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }

    private:
        std::string m_Name;
        std::vector<std::shared_ptr<SceneObject>> m_Objects;
        std::vector<std::shared_ptr<Light>> m_Lights;
        std::shared_ptr<Camera> m_MainCamera;

        void UpdateObject(std::shared_ptr<SceneObject> object, float deltaTime);
        void RenderObject(std::shared_ptr<SceneObject> object);
    };

    // 场景管理器
    class JFM_API SceneManager {
    public:
        static SceneManager& GetInstance() {
            static SceneManager instance;
            return instance;
        }

        void LoadScene(std::shared_ptr<Scene> scene);
        void UnloadCurrentScene();

        std::shared_ptr<Scene> GetCurrentScene() const { return m_CurrentScene; }
        void SetCurrentScene(std::shared_ptr<Scene> scene) { m_CurrentScene = scene; }

        void Update(float deltaTime);
        void Render();

    private:
        std::shared_ptr<Scene> m_CurrentScene;
        SceneManager() = default;
    };

}
