//
// System.h - ECS系统基类
// 定义系统接口和基础功能
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "Entity.h"
#include "Component.h"
#include <vector>

namespace JFM {
    namespace ECS {

        // 前置声明
        class World;

        // 系统基类
        class JFM_API System {
        public:
            System() = default;
            virtual ~System() = default;

            // 系统生命周期
            virtual void OnCreate() {}
            virtual void OnDestroy() {}
            virtual void Update(float deltaTime) {}
            virtual void Render() {}

            // 组件变化通知
            template<typename T>
            void OnComponentAdded(Entity entity) {
                // 子类可以重写此方法来响应组件添加
                (void)entity; // 避免未使用参数警告
            }

            template<typename T>
            void OnComponentRemoved(Entity entity) {
                // 子类可以重写此方法来响应组件移除
                (void)entity; // 避免未使用参数警告
            }

            // 设置世界引用
            void SetWorld(World* world) { m_World = world; }

        protected:
            World* m_World = nullptr;

            // 获取满足条件的实体列表 - 声明在这里，实现在cpp文件中
            template<typename... Components>
            std::vector<Entity> GetEntities() const;
        };

    } // namespace ECS
} // namespace JFM
