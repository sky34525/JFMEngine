//
// Physics3D.h - 增强的3D物理系统
//

#pragma once

#include "JFMEngine/Physics/Physics.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace JFM {

    // 3D刚体组件
    class JFM_API Rigidbody3D : public EnhancedRigidbody {
    public:
        Rigidbody3D();

        // 3D旋转
        void SetRotation(const glm::quat& rotation) { m_Rotation = rotation; }
        glm::quat GetRotation() const { return m_Rotation; }

        void SetAngularVelocity(const glm::vec3& angularVel) { m_AngularVelocity = angularVel; }
        glm::vec3 GetAngularVelocity() const { return m_AngularVelocity; }

        // 惯性张量
        void SetInertiaTensor(const glm::mat3& inertia) { m_InertiaTensor = inertia; }
        glm::mat3 GetInertiaTensor() const { return m_InertiaTensor; }

        // 3D力和扭矩
        void AddTorque(const glm::vec3& torque) { m_Torque += torque; }
        void ApplyForceAtPosition(const glm::vec3& force, const glm::vec3& worldPos);

        // 变换矩阵
        glm::mat4 GetTransformMatrix() const;

        // 物理更新
        void UpdatePhysics3D(float deltaTime);

        // 约束
        void SetLinearDamping(float damping) { m_LinearDamping = damping; }
        void SetAngularDamping(float damping) { m_AngularDamping = damping; }

        void LockPosition(bool x, bool y, bool z) {
            m_LockPosition = glm::bvec3(x, y, z);
        }
        void LockRotation(bool x, bool y, bool z) {
            m_LockRotation = glm::bvec3(x, y, z);
        }

    private:
        glm::quat m_Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 m_AngularVelocity = glm::vec3(0.0f);
        glm::vec3 m_Torque = glm::vec3(0.0f);
        glm::mat3 m_InertiaTensor = glm::mat3(1.0f);

        float m_LinearDamping = 0.01f;
        float m_AngularDamping = 0.05f;

        glm::bvec3 m_LockPosition = glm::bvec3(false);
        glm::bvec3 m_LockRotation = glm::bvec3(false);
    };

    // 复杂碰撞器
    class JFM_API MeshCollider : public Collider {
    public:
        MeshCollider(const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices);

        virtual AABB GetAABB(const glm::vec3& position) const override;
        virtual bool CheckCollision(const Collider& other, const glm::vec3& posA, const glm::vec3& posB) const override;

        bool IsConvex() const { return m_IsConvex; }
        void SetConvex(bool convex) { m_IsConvex = convex; }

    private:
        std::vector<glm::vec3> m_Vertices;
        std::vector<uint32_t> m_Indices;
        bool m_IsConvex = false;

        bool RayTriangleIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                                const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                                float& t) const;
    };

    // 胶囊碰撞器
    class JFM_API CapsuleCollider : public Collider {
    public:
        CapsuleCollider(float radius, float height) : m_Radius(radius), m_Height(height) {}

        virtual AABB GetAABB(const glm::vec3& position) const override;
        virtual bool CheckCollision(const Collider& other, const glm::vec3& posA, const glm::vec3& posB) const override;

        float GetRadius() const { return m_Radius; }
        float GetHeight() const { return m_Height; }

    private:
        float m_Radius;
        float m_Height;
    };

    // 物理关节系统
    class JFM_API Joint {
    public:
        enum Type {
            FIXED,
            HINGE,
            BALL_SOCKET,
            SLIDER,
            SPRING
        };

        Joint(Type type, std::shared_ptr<Rigidbody3D> bodyA, std::shared_ptr<Rigidbody3D> bodyB);
        virtual ~Joint() = default;

        virtual void UpdateConstraint(float deltaTime) = 0;

        void SetBreakForce(float force) { m_BreakForce = force; }
        bool IsBroken() const { return m_Broken; }

    protected:
        Type m_Type;
        std::shared_ptr<Rigidbody3D> m_BodyA;
        std::shared_ptr<Rigidbody3D> m_BodyB;
        float m_BreakForce = FLT_MAX;
        bool m_Broken = false;
    };

    // 铰链关节
    class JFM_API HingeJoint : public Joint {
    public:
        HingeJoint(std::shared_ptr<Rigidbody3D> bodyA, std::shared_ptr<Rigidbody3D> bodyB,
                   const glm::vec3& anchor, const glm::vec3& axis);

        void SetLimits(float minAngle, float maxAngle);
        void SetMotor(float targetVelocity, float maxForce);

        virtual void UpdateConstraint(float deltaTime) override;

    private:
        glm::vec3 m_Anchor;
        glm::vec3 m_Axis;
        float m_MinAngle = -FLT_MAX;
        float m_MaxAngle = FLT_MAX;
        float m_TargetVelocity = 0.0f;
        float m_MaxMotorForce = 0.0f;
    };

    // 3D物理世界
    class JFM_API PhysicsWorld3D : public PhysicsWorld {
    public:
        static PhysicsWorld3D& GetInstance() {
            static PhysicsWorld3D instance;
            return instance;
        }

        // 关节管理
        void AddJoint(std::shared_ptr<Joint> joint);
        void RemoveJoint(std::shared_ptr<Joint> joint);

        // 3D物理查询
        struct RaycastHit3D {
            bool Hit = false;
            glm::vec3 Point = glm::vec3(0.0f);
            glm::vec3 Normal = glm::vec3(0.0f);
            float Distance = 0.0f;
            std::shared_ptr<Rigidbody3D> Rigidbody = nullptr;
        };

        RaycastHit3D Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance = 1000.0f);
        std::vector<std::shared_ptr<Rigidbody3D>> SphereCast(const glm::vec3& center, float radius);

        // 3D物理更新
        virtual void Update(float deltaTime) override;

    private:
        std::vector<std::shared_ptr<Joint>> m_Joints;

        void UpdateJoints(float deltaTime);
        void SolveConstraints(float deltaTime);
    };

}
