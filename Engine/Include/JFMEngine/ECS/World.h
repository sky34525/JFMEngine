//
// World.h - ECS世界管理器
// 管理所有实体、组件和系统的核心类
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/Utils/Log.h"
#include "Entity.h"
#include "Component.h"
#include "ComponentPool.h"
#include "System.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <queue>

namespace JFM {
    namespace ECS {

        class JFM_API World {
        public:
            World();
            ~World();

            // 实体管理
            Entity CreateEntity();
            void DestroyEntity(Entity entity);
            bool IsEntityValid(Entity entity) const;

            // 组件管理
            template<typename T, typename... Args>
            T& AddComponent(Entity entity, Args&&... args) {
                if (!IsEntityValid(entity)) {
                    static T dummy;
                    return dummy;
                }

                auto pool = GetOrCreateComponentPool<T>();
                T& component = pool->AddComponent(entity, T(std::forward<Args>(args)...));

                // 更新实体的组件掩码
                m_EntityComponentMasks[entity] |= GetComponentMask<T>();

                // 通知系统有新组件添加
                NotifyComponentAdded<T>(entity);

                return component;
            }

            template<typename T>
            void RemoveComponent(Entity entity) {
                if (!HasComponent<T>(entity)) {
                    return;
                }

                auto pool = GetComponentPool<T>();
                if (pool) {
                    pool->RemoveComponent(entity);
                    m_EntityComponentMasks[entity] &= ~GetComponentMask<T>();
                    NotifyComponentRemoved<T>(entity);
                }
            }

            template<typename T>
            T& GetComponent(Entity entity) {
                auto pool = GetComponentPool<T>();
                if (!pool) {
                    static T dummy;
                    return dummy;
                }
                return pool->GetComponent(entity);
            }

            template<typename T>
            const T& GetComponent(Entity entity) const {
                auto pool = GetComponentPool<T>();
                if (!pool) {
                    static T dummy;
                    return dummy;
                }
                return pool->GetComponent(entity);
            }

            template<typename T>
            bool HasComponent(Entity entity) const {
                auto pool = GetComponentPool<T>();
                return pool && pool->HasComponent(entity);
            }

            // 获取实体的组件掩码
            ComponentMask GetEntityComponentMask(Entity entity) const {
                auto it = m_EntityComponentMasks.find(entity);
                return it != m_EntityComponentMasks.end() ? it->second : 0;
            }

            // 系统管理
            template<typename T, typename... Args>
            T* AddSystem(Args&&... args) {
                static_assert(std::is_base_of_v<System, T>, "T must inherit from System");

                auto system = std::make_unique<T>(std::forward<Args>(args)...);
                T* systemPtr = system.get();
                system->SetWorld(this);
                system->OnCreate();

                m_Systems.push_back(std::move(system));
                return systemPtr;
            }

            template<typename T>
            T* GetSystem() {
                for (auto& system : m_Systems) {
                    T* castedSystem = dynamic_cast<T*>(system.get());
                    if (castedSystem) {
                        return castedSystem;
                    }
                }
                return nullptr;
            }

            // 查询实体
            std::vector<Entity> GetEntitiesWithComponents(ComponentMask mask) const;

            template<typename... Components>
            std::vector<Entity> GetEntitiesWithComponents() const {
                ComponentMask mask = (GetComponentMask<Components>() | ...);
                return GetEntitiesWithComponents(mask);
            }

            // 更新系统
            void Update(float deltaTime);
            void Render();

            // 获取统计信息
            size_t GetEntityCount() const { return m_NextEntityID - m_FreeEntityIDs.size() - 1; }
            size_t GetSystemCount() const { return m_Systems.size(); }

        private:
            // 组件池管理
            template<typename T>
            ComponentPool<T>* GetComponentPool() {
                ComponentTypeID typeID = ComponentTypeRegistry::GetTypeID<T>();
                auto it = m_ComponentPools.find(typeID);
                if (it != m_ComponentPools.end()) {
                    return static_cast<ComponentPool<T>*>(it->second.get());
                }
                return nullptr;
            }

            template<typename T>
            const ComponentPool<T>* GetComponentPool() const {
                ComponentTypeID typeID = ComponentTypeRegistry::GetTypeID<T>();
                auto it = m_ComponentPools.find(typeID);
                if (it != m_ComponentPools.end()) {
                    return static_cast<const ComponentPool<T>*>(it->second.get());
                }
                return nullptr;
            }

            template<typename T>
            ComponentPool<T>* GetOrCreateComponentPool() {
                ComponentTypeID typeID = ComponentTypeRegistry::GetTypeID<T>();
                auto it = m_ComponentPools.find(typeID);
                if (it != m_ComponentPools.end()) {
                    return static_cast<ComponentPool<T>*>(it->second.get());
                }

                auto pool = std::make_unique<ComponentPool<T>>();
                ComponentPool<T>* poolPtr = pool.get();
                m_ComponentPools[typeID] = std::move(pool);
                return poolPtr;
            }

            // 系统通知
            template<typename T>
            void NotifyComponentAdded(Entity entity) {
                for (auto& system : m_Systems) {
                    system->OnComponentAdded<T>(entity);
                }
            }

            template<typename T>
            void NotifyComponentRemoved(Entity entity) {
                for (auto& system : m_Systems) {
                    system->OnComponentRemoved<T>(entity);
                }
            }

        private:
            EntityID m_NextEntityID = 1; // 0 保留为 NULL_ENTITY
            std::queue<EntityID> m_FreeEntityIDs;
            std::unordered_map<Entity, ComponentMask> m_EntityComponentMasks;

            std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentPool>> m_ComponentPools;
            std::vector<std::unique_ptr<System>> m_Systems;
        };

    } // namespace ECS
} // namespace JFM
