//
// Physics.h - 简单物理系统
// 支持基础碰撞检测和物理模拟
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace JFM {

    // 包围盒
    struct AABB {
        glm::vec3 Min;
        glm::vec3 Max;

        AABB() : Min(0.0f), Max(0.0f) {}
        AABB(const glm::vec3& min, const glm::vec3& max) : Min(min), Max(max) {}

        bool Intersects(const AABB& other) const;
        glm::vec3 GetCenter() const { return (Min + Max) * 0.5f; }
        glm::vec3 GetSize() const { return Max - Min; }
    };

    // 刚体组件
    class JFM_API Rigidbody {
    public:
        Rigidbody();

        void SetMass(float mass) { m_Mass = mass; m_InvMass = (mass == 0.0f) ? 0.0f : 1.0f / mass; }
        float GetMass() const { return m_Mass; }

        void SetVelocity(const glm::vec3& velocity) { m_Velocity = velocity; }
        glm::vec3 GetVelocity() const { return m_Velocity; }

        void AddForce(const glm::vec3& force) { m_Force += force; }
        void SetGravity(bool gravity) { m_UseGravity = gravity; }

        void UpdatePhysics(float deltaTime);

        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::vec3 m_Velocity = glm::vec3(0.0f);
        glm::vec3 m_Force = glm::vec3(0.0f);

    private:
        float m_Mass = 1.0f;
        float m_InvMass = 1.0f;
        bool m_UseGravity = true;
        float m_Drag = 0.98f;
    };

    // 碰撞器基类
    class JFM_API Collider {
    public:
        virtual ~Collider() = default;
        virtual AABB GetAABB(const glm::vec3& position) const = 0;
        virtual bool CheckCollision(const Collider& other, const glm::vec3& posA, const glm::vec3& posB) const = 0;
    };

    // 盒子碰撞器
    class JFM_API BoxCollider : public Collider {
    public:
        BoxCollider(const glm::vec3& size) : m_Size(size) {}

        virtual AABB GetAABB(const glm::vec3& position) const override;
        virtual bool CheckCollision(const Collider& other, const glm::vec3& posA, const glm::vec3& posB) const override;

        glm::vec3 GetSize() const { return m_Size; }

    private:
        glm::vec3 m_Size;
    };

    // 物理世界
    class JFM_API PhysicsWorld {
    public:
        static PhysicsWorld& GetInstance() {
            static PhysicsWorld instance;
            return instance;
        }

        void Update(float deltaTime);
        void AddRigidbody(std::shared_ptr<Rigidbody> rigidbody);
        void RemoveRigidbody(std::shared_ptr<Rigidbody> rigidbody);

        void SetGravity(const glm::vec3& gravity) { m_Gravity = gravity; }
        glm::vec3 GetGravity() const { return m_Gravity; }

        // 获取所有刚体（用于工具函数）
        const std::vector<std::shared_ptr<Rigidbody>>& GetRigidbodies() const { return m_Rigidbodies; }

        // 设置物理参数
        void SetTimeStep(float timeStep) { m_FixedTimeStep = timeStep; }
        float GetTimeStep() const { return m_FixedTimeStep; }

        void SetMaxSubSteps(int maxSubSteps) { m_MaxSubSteps = maxSubSteps; }
        int GetMaxSubSteps() const { return m_MaxSubSteps; }

        // 暂停/恢复物理模拟
        void SetPaused(bool paused) { m_Paused = paused; }
        bool IsPaused() const { return m_Paused; }

    private:
        std::vector<std::shared_ptr<Rigidbody>> m_Rigidbodies;
        glm::vec3 m_Gravity = glm::vec3(0.0f, -9.81f, 0.0f);
        float m_FixedTimeStep = 1.0f / 60.0f; // 60 FPS
        int m_MaxSubSteps = 3;
        bool m_Paused = false;
        float m_AccumulatedTime = 0.0f;

        PhysicsWorld() = default;
    };

}
