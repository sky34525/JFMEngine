//
// System.cpp - ECS系统基类实现
//

#include "JFMEngine/ECS/System.h"
#include "JFMEngine/ECS/World.h"
#include "JFMEngine/ECS/Components.h"

namespace JFM {
    namespace ECS {

        // 模板方法的显式实现
        template<typename... Components>
        std::vector<Entity> System::GetEntities() const {
            if (!m_World) return {};
            return m_World->GetEntitiesWithComponents<Components...>();
        }

        // 显式实例化常用的模板组合
        template std::vector<Entity> System::GetEntities<TransformComponent>() const;
        template std::vector<Entity> System::GetEntities<TransformComponent, MeshRendererComponent>() const;
        template std::vector<Entity> System::GetEntities<TransformComponent, CameraComponent>() const;
        template std::vector<Entity> System::GetEntities<TransformComponent, LightComponent>() const;
        template std::vector<Entity> System::GetEntities<TransformComponent, ParentComponent>() const;
        template std::vector<Entity> System::GetEntities<TransformComponent, ChildrenComponent>() const;
        template std::vector<Entity> System::GetEntities<ActiveComponent>() const;

    } // namespace ECS
} // namespace JFM
