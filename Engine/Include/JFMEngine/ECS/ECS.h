//
// ECS.h - ECS系统主头文件
// 包含所有ECS相关的头文件，提供统一的接口
//

#pragma once

// 核心ECS组件
#include "Entity.h"
#include "Component.h"
#include "ComponentPool.h"
#include "System.h"
#include "World.h"

// 标准组件
#include "Components.h"

// 标准系统
#include "Systems/TransformSystem.h"
#include "Systems/RenderSystem.h"

namespace JFM {
    namespace ECS {

        // ECS工厂类 - 提供便捷的创建方法
        class JFM_API ECSFactory {
        public:
            // 创建标准3D对象
            static Entity CreateCube(World* world, const std::string& name = "Cube");
            static Entity CreateSphere(World* world, const std::string& name = "Sphere");
            static Entity CreatePlane(World* world, const std::string& name = "Plane");

            // 创建光源
            static Entity CreateDirectionalLight(World* world, const glm::vec3& direction = glm::vec3(0, -1, 0));
            static Entity CreatePointLight(World* world, const glm::vec3& position = glm::vec3(0, 0, 0));
            static Entity CreateSpotLight(World* world, const glm::vec3& position = glm::vec3(0, 0, 0));

            // 创建相机
            static Entity CreatePerspectiveCamera(World* world, float fov = 45.0f);
            static Entity CreateOrthographicCamera(World* world, float size = 10.0f);

            // 创建空对象
            static Entity CreateEmpty(World* world, const std::string& name = "GameObject");
        };

        // ECS查询辅助类
        class JFM_API ECSQuery {
        public:
            explicit ECSQuery(World* world) : m_World(world) {}

            // 链式查询接口
            template<typename T>
            ECSQuery& With() {
                m_RequiredMask |= GetComponentMask<T>();
                return *this;
            }

            template<typename T>
            ECSQuery& Without() {
                m_ExcludedMask |= GetComponentMask<T>();
                return *this;
            }

            // 执行查询
            std::vector<Entity> Execute() const;

            // 遍历查询结果
            template<typename Func>
            void ForEach(Func&& func) const {
                auto entities = Execute();
                for (Entity entity : entities) {
                    func(entity);
                }
            }

        private:
            World* m_World;
            ComponentMask m_RequiredMask = 0;
            ComponentMask m_ExcludedMask = 0;
        };

    } // namespace ECS
} // namespace JFM
