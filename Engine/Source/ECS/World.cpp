//
// World.cpp - ECS世界管理器实现
//

#include "JFMEngine/ECS/World.h"
#include "JFMEngine/Utils/Log.h"

namespace JFM {
    namespace ECS {

        World::World() {
        }

        World::~World() {
            // 清理所有系统
            for (auto& system : m_Systems) {
                system->OnDestroy();
            }
            m_Systems.clear();

            // 清理所有组件池
            m_ComponentPools.clear();

        }

        Entity World::CreateEntity() {
            EntityID id;

            if (!m_FreeEntityIDs.empty()) {
                // 重用已释放的ID
                id = m_FreeEntityIDs.front();
                m_FreeEntityIDs.pop();
            } else {
                // 分配新ID
                id = m_NextEntityID++;
            }

            Entity entity(id);
            m_EntityComponentMasks[entity] = 0;
            
            return entity;
        }

        void World::DestroyEntity(Entity entity) {
            if (!IsEntityValid(entity)) {
                return;
            }

            // 移除所有组件
            ComponentMask mask = m_EntityComponentMasks[entity];
            for (ComponentTypeID typeID = 0; typeID < 64; ++typeID) {
                if (mask & (1ULL << typeID)) {
                    auto poolIt = m_ComponentPools.find(typeID);
                    if (poolIt != m_ComponentPools.end()) {
                        poolIt->second->RemoveComponent(entity);
                    }
                }
            }

            // 从实体掩码映射中移除
            m_EntityComponentMasks.erase(entity);

            // 将ID加入空闲列表
            m_FreeEntityIDs.push(entity.GetID());

        }

        bool World::IsEntityValid(Entity entity) const {
            if (entity == NULL_ENTITY) {
                return false;
            }

            return m_EntityComponentMasks.find(entity) != m_EntityComponentMasks.end();
        }

        std::vector<Entity> World::GetEntitiesWithComponents(ComponentMask mask) const {
            std::vector<Entity> result;

            for (const auto& pair : m_EntityComponentMasks) {
                if ((pair.second & mask) == mask) {
                    result.push_back(pair.first);
                }
            }

            return result;
        }

        void World::Update(float deltaTime) {
            for (auto& system : m_Systems) {
                system->Update(deltaTime);
            }
        }

        void World::Render() {
            for (auto& system : m_Systems) {
                system->Render();
            }
        }

    } // namespace ECS
} // namespace JFM
