//
// Component.cpp - ECS组件系统实现
//

#include "JFMEngine/ECS/Component.h"

namespace JFM {
    namespace ECS {

        // 静态成员初始化
        ComponentTypeID ComponentTypeRegistry::s_NextTypeID = 1;

    } // namespace ECS
} // namespace JFM
