//
// TransformSystem.h - 变换系统
// 处理实体的层次结构和世界变换计算
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/ECS/System.h"
#include "JFMEngine/ECS/Components.h"
#include <glm/glm.hpp>

namespace JFM {
    namespace ECS {

        class JFM_API TransformSystem : public System {
        public:
            void Update(float deltaTime) override;

        private:
            void UpdateTransformHierarchy(Entity entity, const glm::mat4& parentMatrix = glm::mat4(1.0f));
            void MarkChildrenDirty(Entity entity);
        };

    } // namespace ECS
} // namespace JFM
