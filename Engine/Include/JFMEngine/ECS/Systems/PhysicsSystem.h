//
// PhysicsSystem.h - 物理系统示例
// 展示如何创建自定义ECS系统
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/ECS/System.h"
#include "JFMEngine/ECS/Components.h"
#include <vector>

namespace JFM {
    namespace ECS {

        class JFM_API PhysicsSystem : public System {
        public:
            PhysicsSystem();
            ~PhysicsSystem();

            void OnCreate() override;
            void OnDestroy() override;
            void Update(float deltaTime) override;

            // 组件变化响应
            template<>
            void OnComponentAdded<RigidbodyComponent>(Entity entity);

            template<>
            void OnComponentRemoved<RigidbodyComponent>(Entity entity);

        private:
            void InitializePhysicsWorld();
            void CleanupPhysicsWorld();
            void UpdateRigidbodies(float deltaTime);
            void SyncTransforms();

            // 物理世界指针（具体实现取决于使用的物理引擎）
            void* m_PhysicsWorld = nullptr;

            // 需要物理更新的实体缓存
            std::vector<Entity> m_PhysicsEntities;
        };

    } // namespace ECS
} // namespace JFM
