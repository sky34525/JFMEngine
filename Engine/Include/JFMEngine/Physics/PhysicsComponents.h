//
// PhysicsComponents.h - 额外的物理组件
//

#pragma once

#include "Physics.h"
#include <glm/glm.hpp>

namespace JFM {

    // 球体碰撞器
    class JFM_API SphereCollider : public Collider {
    public:
        SphereCollider(float radius) : m_Radius(radius) {}

        virtual AABB GetAABB(const glm::vec3& position) const override;
        virtual bool CheckCollision(const Collider& other, const glm::vec3& posA, const glm::vec3& posB) const override;

        float GetRadius() const { return m_Radius; }
        void SetRadius(float radius) { m_Radius = radius; }

        // 球体与球体的碰撞检测
        bool CheckSphereCollision(const SphereCollider& other, const glm::vec3& posA, const glm::vec3& posB) const;

        // 球体与盒子的碰撞检测
        bool CheckBoxCollision(const BoxCollider& box, const glm::vec3& spherePos, const glm::vec3& boxPos) const;

    private:
        float m_Radius;
    };

    // 物理材质
    struct JFM_API PhysicsMaterial {
        float Friction = 0.6f;        // 摩擦系数
        float Restitution = 0.5f;     // 恢复系数（弹性）
        float Density = 1.0f;         // 密度

        PhysicsMaterial() = default;
        PhysicsMaterial(float friction, float restitution, float density)
            : Friction(friction), Restitution(restitution), Density(density) {}
    };

    // 增强的刚体组件
    class JFM_API EnhancedRigidbody : public Rigidbody {
    public:
        EnhancedRigidbody();

        void SetMaterial(const PhysicsMaterial& material) { m_Material = material; }
        const PhysicsMaterial& GetMaterial() const { return m_Material; }

        void SetCollider(std::shared_ptr<Collider> collider) { m_Collider = collider; }
        std::shared_ptr<Collider> GetCollider() const { return m_Collider; }

        // 应用冲量
        void ApplyImpulse(const glm::vec3& impulse);

        // 应用在特定点的力
        void ApplyForceAtPoint(const glm::vec3& force, const glm::vec3& point);

        // 约束到边界
        void ConstrainToBounds(const AABB& bounds);

        // 角速度和旋转（简化版）
        void SetAngularVelocity(const glm::vec3& angularVel) { m_AngularVelocity = angularVel; }
        glm::vec3 GetAngularVelocity() const { return m_AngularVelocity; }

        glm::vec3 m_Rotation = glm::vec3(0.0f);

    private:
        PhysicsMaterial m_Material;
        std::shared_ptr<Collider> m_Collider;
        glm::vec3 m_AngularVelocity = glm::vec3(0.0f);
        float m_AngularDrag = 0.95f;
    };

    // 物理工具函数
    namespace PhysicsUtils {

        // 射线投射结构
        struct RaycastHit {
            bool Hit = false;
            glm::vec3 Point = glm::vec3(0.0f);
            glm::vec3 Normal = glm::vec3(0.0f);
            float Distance = 0.0f;
            std::shared_ptr<Rigidbody> Rigidbody = nullptr;
        };

        struct Ray {
            glm::vec3 Origin = glm::vec3(0.0f);
            glm::vec3 Direction = glm::vec3(0.0f, 0.0f, 1.0f);
            float MaxDistance = 1000.0f;

            Ray() = default;
            Ray(const glm::vec3& origin, const glm::vec3& direction, float maxDist = 1000.0f)
                : Origin(origin), Direction(glm::normalize(direction)), MaxDistance(maxDist) {}
        };

        // 射线投射
        JFM_API RaycastHit Raycast(const Ray& ray);

        // 重叠检测
        JFM_API std::vector<std::shared_ptr<Rigidbody>> OverlapSphere(const glm::vec3& center, float radius);
        JFM_API std::vector<std::shared_ptr<Rigidbody>> OverlapBox(const glm::vec3& center, const glm::vec3& size);

        // 距离计算
        JFM_API float DistancePointToAABB(const glm::vec3& point, const AABB& aabb);
        JFM_API glm::vec3 ClosestPointOnAABB(const glm::vec3& point, const AABB& aabb);
    }

}
