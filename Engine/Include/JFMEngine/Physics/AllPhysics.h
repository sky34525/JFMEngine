//
// AllPhysics.h - 物理系统主要头文件
// 包含所有物理相关的组件和工具
//

#pragma once

#include "Physics.h"
#include "PhysicsComponents.h"

// 便利的类型别名
namespace JFM {
    using RigidBody = Rigidbody;
    using PhysWorld = PhysicsWorld;
    using BoxCol = BoxCollider;
    using SphereCol = SphereCollider;
    using EnhancedRB = EnhancedRigidbody;
}

// 便利的静态函数
namespace JFM::Physics {

    // 快速创建物理对象的便利函数
    inline std::shared_ptr<Rigidbody> CreateRigidbody(
        const glm::vec3& position = glm::vec3(0.0f),
        float mass = 1.0f,
        bool useGravity = true
    ) {
        auto rb = std::make_shared<EnhancedRigidbody>();
        rb->m_Position = position;
        rb->SetMass(mass);
        rb->SetGravity(useGravity);
        return rb;
    }

    // 快速创建球体碰撞器
    inline std::shared_ptr<SphereCollider> CreateSphereCollider(float radius) {
        return std::make_shared<SphereCollider>(radius);
    }

    // 快速创建盒子碰撞器
    inline std::shared_ptr<BoxCollider> CreateBoxCollider(const glm::vec3& size) {
        return std::make_shared<BoxCollider>(size);
    }

    // 快速添加到物理世界
    inline void AddToWorld(std::shared_ptr<Rigidbody> rigidbody) {
        PhysicsWorld::GetInstance().AddRigidbody(rigidbody);
    }

    // 快速从物理世界移除
    inline void RemoveFromWorld(std::shared_ptr<Rigidbody> rigidbody) {
        PhysicsWorld::GetInstance().RemoveRigidbody(rigidbody);
    }

    // 快速设置世界参数
    inline void SetWorldGravity(const glm::vec3& gravity) {
        PhysicsWorld::GetInstance().SetGravity(gravity);
    }

    inline void SetWorldTimeStep(float timeStep) {
        PhysicsWorld::GetInstance().SetTimeStep(timeStep);
    }

    inline void PauseWorld(bool paused) {
        PhysicsWorld::GetInstance().SetPaused(paused);
    }

    // 更新物理世界
    inline void UpdateWorld(float deltaTime) {
        PhysicsWorld::GetInstance().Update(deltaTime);
    }
}
