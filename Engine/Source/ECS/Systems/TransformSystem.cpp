//
// TransformSystem.cpp - 变换系统实现
//

#include "JFMEngine/ECS/Systems/TransformSystem.h"
#include "JFMEngine/ECS/World.h"
#include "JFMEngine/Utils/Log.h"
#include <glm/gtc/matrix_transform.hpp>

namespace JFM {
    namespace ECS {

        void TransformSystem::Update(float deltaTime) {
            (void)deltaTime; // 避免未使用参数警告

            // 获取所有根节点（没有父节点的实体）
            auto rootEntities = GetEntities<TransformComponent>();

            for (Entity entity : rootEntities) {
                auto& transform = m_World->GetComponent<TransformComponent>(entity);

                // 只处理根节点或dirty的节点
                if (!m_World->HasComponent<ParentComponent>(entity) || transform.IsDirty) {
                    UpdateTransformHierarchy(entity);
                }
            }
        }

        void TransformSystem::UpdateTransformHierarchy(Entity entity, const glm::mat4& parentMatrix) {
            if (!m_World->IsEntityValid(entity)) {
                return;
            }

            auto& transform = m_World->GetComponent<TransformComponent>(entity);

            // 计算本地变换矩阵
            glm::mat4 localMatrix = transform.GetLocalMatrix();

            // 计算世界变换矩阵
            transform.WorldMatrix = parentMatrix * localMatrix;
            transform.IsDirty = false;

            // 递归更新所有子节点
            if (m_World->HasComponent<ChildrenComponent>(entity)) {
                const auto& children = m_World->GetComponent<ChildrenComponent>(entity);

                for (Entity child : children.Children) {
                    if (m_World->IsEntityValid(child)) {
                        UpdateTransformHierarchy(child, transform.WorldMatrix);
                    }
                }
            }
        }

        void TransformSystem::MarkChildrenDirty(Entity entity) {
            if (!m_World->IsEntityValid(entity)) {
                return;
            }

            if (m_World->HasComponent<ChildrenComponent>(entity)) {
                const auto& children = m_World->GetComponent<ChildrenComponent>(entity);

                for (Entity child : children.Children) {
                    if (m_World->IsEntityValid(child)) {
                        auto& childTransform = m_World->GetComponent<TransformComponent>(child);
                        childTransform.IsDirty = true;
                        MarkChildrenDirty(child);
                    }
                }
            }
        }

    } // namespace ECS
} // namespace JFM
