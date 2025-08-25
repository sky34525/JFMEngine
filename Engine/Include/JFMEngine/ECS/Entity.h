//
// Entity.h - ECS实体系统
// 轻量级实体标识符
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include <cstdint>
#include <functional>

namespace JFM {
    namespace ECS {

        // 实体ID类型
        using EntityID = uint32_t;
        static constexpr EntityID NULL_ENTITY = 0;

        // 实体类 - 轻量级句柄
        class JFM_API Entity {
        public:
            Entity() : m_ID(NULL_ENTITY) {}

            // 允许从 NULL_ENTITY 进行隐式转换，但其他值需要显式转换
            Entity(EntityID id) : m_ID(id) {}

            EntityID GetID() const { return m_ID; }
            bool IsValid() const { return m_ID != NULL_ENTITY; }

            // 与其他 Entity 对象的比较运算符
            bool operator==(const Entity& other) const { return m_ID == other.m_ID; }
            bool operator!=(const Entity& other) const { return m_ID != other.m_ID; }
            bool operator<(const Entity& other) const { return m_ID < other.m_ID; }

            // 与 EntityID 的比较运算符
            bool operator==(EntityID id) const { return m_ID == id; }
            bool operator!=(EntityID id) const { return m_ID != id; }

            // 隐式转换为EntityID
            operator EntityID() const { return m_ID; }

        private:
            EntityID m_ID;
        };

    } // namespace ECS
} // namespace JFM

// 为Entity提供哈希支持
namespace std {
    template<>
    struct hash<JFM::ECS::Entity> {
        std::size_t operator()(const JFM::ECS::Entity& entity) const {
            return std::hash<JFM::ECS::EntityID>()(entity.GetID());
        }
    };
}
