//
// ComponentPool.h - 组件池存储系统
// 高效的组件内存管理和访问
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/Utils/Log.h"
#include "Entity.h"
#include "Component.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>

namespace JFM {
    namespace ECS {

        // 抽象组件池基类
        class JFM_API IComponentPool {
        public:
            virtual ~IComponentPool() = default;
            virtual void RemoveComponent(Entity entity) = 0;
            virtual bool HasComponent(Entity entity) const = 0;
            virtual ComponentTypeID GetComponentTypeID() const = 0;
        };

        // 类型化组件池
        template<typename T>
        class ComponentPool : public IComponentPool {
        public:
            ComponentPool() = default;
            ~ComponentPool() = default;

            // 添加组件
            T& AddComponent(Entity entity, T&& component = T{}) {
                auto it = m_EntityToIndex.find(entity);
                if (it != m_EntityToIndex.end()) {
                    // 组件已存在，替换它
                    m_Components[it->second] = std::move(component);
                    return m_Components[it->second];
                }

                // 添加新组件
                size_t newIndex = m_Components.size();
                m_Components.push_back(std::move(component));
                m_Entities.push_back(entity);
                m_EntityToIndex[entity] = newIndex;

                return m_Components[newIndex];
            }

            // 获取组件
            T& GetComponent(Entity entity) {
                auto it = m_EntityToIndex.find(entity);
                if (it == m_EntityToIndex.end()) {
                    static T dummy;
                    return dummy;
                }
                return m_Components[it->second];
            }

            const T& GetComponent(Entity entity) const {
                auto it = m_EntityToIndex.find(entity);
                if (it == m_EntityToIndex.end()) {
                    static T dummy;
                    return dummy;
                }
                return m_Components[it->second];
            }

            // 移除组件
            void RemoveComponent(Entity entity) override {
                auto it = m_EntityToIndex.find(entity);
                if (it == m_EntityToIndex.end()) {
                    return; // 组件不存在
                }

                size_t indexToRemove = it->second;
                size_t lastIndex = m_Components.size() - 1;

                // 如果不是最后一个元素，将最后一个元素移到被删除的位置
                if (indexToRemove != lastIndex) {
                    m_Components[indexToRemove] = std::move(m_Components[lastIndex]);
                    m_Entities[indexToRemove] = m_Entities[lastIndex];
                    m_EntityToIndex[m_Entities[indexToRemove]] = indexToRemove;
                }

                // 移除最后一个元素
                m_Components.pop_back();
                m_Entities.pop_back();
                m_EntityToIndex.erase(entity);
            }

            // 检查组件是否存在
            bool HasComponent(Entity entity) const override {
                return m_EntityToIndex.find(entity) != m_EntityToIndex.end();
            }

            // 获取所有组件
            std::vector<T>& GetComponents() { return m_Components; }
            const std::vector<T>& GetComponents() const { return m_Components; }

            // 获取所有实体
            const std::vector<Entity>& GetEntities() const { return m_Entities; }

            // 获取组件类型ID
            ComponentTypeID GetComponentTypeID() const override {
                return ComponentTypeRegistry::GetTypeID<T>();
            }

            // 迭代器支持
            typename std::vector<T>::iterator begin() { return m_Components.begin(); }
            typename std::vector<T>::iterator end() { return m_Components.end(); }
            typename std::vector<T>::const_iterator begin() const { return m_Components.begin(); }
            typename std::vector<T>::const_iterator end() const { return m_Components.end(); }

        private:
            std::vector<T> m_Components;                    // 紧密排列的组件数据
            std::vector<Entity> m_Entities;                // 对应的实体ID
            std::unordered_map<Entity, size_t> m_EntityToIndex; // 实体到索引的映射
        };

    } // namespace ECS
} // namespace JFM
