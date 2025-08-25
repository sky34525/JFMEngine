//
// Component.h - ECS组件系统基础
// 定义组件基类和组件ID管理
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include <typeinfo>
#include <typeindex>
#include <cstdint>

namespace JFM {
    namespace ECS {

        // 组件ID类型
        using ComponentTypeID = uint32_t;

        // 组件ID生成器
        class ComponentTypeRegistry {
        public:
            template<typename T>
            static ComponentTypeID GetTypeID() {
                static ComponentTypeID typeID = s_NextTypeID++;
                return typeID;
            }

            static ComponentTypeID GetNextTypeID() { return s_NextTypeID; }

        private:
            static ComponentTypeID s_NextTypeID;
        };

        // 组件基类（可选，用于多态操作）
        class JFM_API IComponent {
        public:
            virtual ~IComponent() = default;
            virtual ComponentTypeID GetTypeID() const = 0;
        };

        // 组件基类模板
        template<typename T>
        class Component : public IComponent {
        public:
            static ComponentTypeID GetStaticTypeID() {
                return ComponentTypeRegistry::GetTypeID<T>();
            }

            ComponentTypeID GetTypeID() const override {
                return GetStaticTypeID();
            }
        };

        // 组件掩码类型（用于快速系统查询）
        using ComponentMask = uint64_t;

        template<typename T>
        ComponentMask GetComponentMask() {
            ComponentTypeID id = ComponentTypeRegistry::GetTypeID<T>();
            return id < 64 ? (1ULL << id) : 0;
        }

    } // namespace ECS
} // namespace JFM
