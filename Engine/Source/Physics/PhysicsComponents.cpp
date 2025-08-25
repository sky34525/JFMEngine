//
// PhysicsComponents.cpp - 额外物理组件的实现
//

#include "JFMEngine/Physics/PhysicsComponents.h"
#include <algorithm>
#include <limits>

namespace JFM {

    // SphereCollider 实现
    AABB SphereCollider::GetAABB(const glm::vec3& position) const {
        glm::vec3 radiusVec(m_Radius);
        return AABB(position - radiusVec, position + radiusVec);
    }

    bool SphereCollider::CheckCollision(const Collider& other, const glm::vec3& posA, const glm::vec3& posB) const {
        // 尝试转换为球体碰撞器
        const SphereCollider* otherSphere = dynamic_cast<const SphereCollider*>(&other);
        if (otherSphere) {
            return CheckSphereCollision(*otherSphere, posA, posB);
        }

        // 尝试转换为盒子碰撞器
        const BoxCollider* otherBox = dynamic_cast<const BoxCollider*>(&other);
        if (otherBox) {
            return CheckBoxCollision(*otherBox, posA, posB);
        }

        // 回退到AABB检测
        AABB aabbA = GetAABB(posA);
        AABB aabbB = other.GetAABB(posB);
        return aabbA.Intersects(aabbB);
    }

    bool SphereCollider::CheckSphereCollision(const SphereCollider& other, const glm::vec3& posA, const glm::vec3& posB) const {
        float distance = glm::length(posA - posB);
        float radiusSum = m_Radius + other.m_Radius;
        return distance <= radiusSum;
    }

    bool SphereCollider::CheckBoxCollision(const BoxCollider& box, const glm::vec3& spherePos, const glm::vec3& boxPos) const {
        // 球体与AABB的碰撞检测
        AABB boxAABB = box.GetAABB(boxPos);
        glm::vec3 closestPoint = PhysicsUtils::ClosestPointOnAABB(spherePos, boxAABB);
        float distance = glm::length(spherePos - closestPoint);
        return distance <= m_Radius;
    }

    // EnhancedRigidbody 实现
    EnhancedRigidbody::EnhancedRigidbody() : Rigidbody() {
        m_Material = PhysicsMaterial();
        m_AngularVelocity = glm::vec3(0.0f);
        m_Rotation = glm::vec3(0.0f);
        m_AngularDrag = 0.95f;
    }

    void EnhancedRigidbody::ApplyImpulse(const glm::vec3& impulse) {
        if (GetMass() > 0.0f) {
            m_Velocity += impulse / GetMass();
        }
    }

    void EnhancedRigidbody::ApplyForceAtPoint(const glm::vec3& force, const glm::vec3& point) {
        // 应用线性力
        AddForce(force);

        // 计算扭矩（简化版）
        glm::vec3 torque = glm::cross(point - m_Position, force);
        m_AngularVelocity += torque * 0.1f; // 简化的惯性计算
    }

    void EnhancedRigidbody::ConstrainToBounds(const AABB& bounds) {
        // 约束位置
        m_Position.x = std::max(bounds.Min.x, std::min(bounds.Max.x, m_Position.x));
        m_Position.y = std::max(bounds.Min.y, std::min(bounds.Max.y, m_Position.y));
        m_Position.z = std::max(bounds.Min.z, std::min(bounds.Max.z, m_Position.z));

        // 如果碰到边界，反弹速度
        if (m_Position.x <= bounds.Min.x || m_Position.x >= bounds.Max.x) {
            m_Velocity.x *= -m_Material.Restitution;
        }
        if (m_Position.y <= bounds.Min.y || m_Position.y >= bounds.Max.y) {
            m_Velocity.y *= -m_Material.Restitution;
        }
        if (m_Position.z <= bounds.Min.z || m_Position.z >= bounds.Max.z) {
            m_Velocity.z *= -m_Material.Restitution;
        }
    }

    // PhysicsUtils 实现
    namespace PhysicsUtils {

        RaycastHit Raycast(const Ray& ray) {
            RaycastHit hit;
            float closestDistance = ray.MaxDistance;

            // 遍历所有刚体进行射线检测
            auto& world = PhysicsWorld::GetInstance();
            const auto& rigidbodies = world.GetRigidbodies();

            for (const auto& rb : rigidbodies) {
                if (!rb) continue;

                // 简单的射线-球体相交检测（假设每个刚体都有1单位半径的球形边界）
                glm::vec3 toRb = rb->m_Position - ray.Origin;
                float projection = glm::dot(toRb, ray.Direction);

                if (projection < 0.0f || projection > ray.MaxDistance) continue;

                glm::vec3 closestPoint = ray.Origin + ray.Direction * projection;
                float distance = glm::length(rb->m_Position - closestPoint);

                if (distance <= 0.5f && projection < closestDistance) { // 假设0.5单位半径
                    hit.Hit = true;
                    hit.Point = closestPoint;
                    hit.Normal = glm::normalize(closestPoint - rb->m_Position);
                    hit.Distance = projection;
                    hit.Rigidbody = rb;
                    closestDistance = projection;
                }
            }

            return hit;
        }

        std::vector<std::shared_ptr<Rigidbody>> OverlapSphere(const glm::vec3& center, float radius) {
            std::vector<std::shared_ptr<Rigidbody>> results;

            auto& world = PhysicsWorld::GetInstance();
            const auto& rigidbodies = world.GetRigidbodies();

            for (const auto& rb : rigidbodies) {
                if (!rb) continue;

                float distance = glm::length(rb->m_Position - center);
                if (distance <= radius + 0.5f) { // 假设刚体有0.5单位半径
                    results.push_back(rb);
                }
            }

            return results;
        }

        std::vector<std::shared_ptr<Rigidbody>> OverlapBox(const glm::vec3& center, const glm::vec3& size) {
            std::vector<std::shared_ptr<Rigidbody>> results;

            auto& world = PhysicsWorld::GetInstance();
            const auto& rigidbodies = world.GetRigidbodies();

            AABB checkBox(center - size * 0.5f, center + size * 0.5f);

            for (const auto& rb : rigidbodies) {
                if (!rb) continue;

                // 简单的点-AABB检测（假设刚体是点）
                if (rb->m_Position.x >= checkBox.Min.x && rb->m_Position.x <= checkBox.Max.x &&
                    rb->m_Position.y >= checkBox.Min.y && rb->m_Position.y <= checkBox.Max.y &&
                    rb->m_Position.z >= checkBox.Min.z && rb->m_Position.z <= checkBox.Max.z) {
                    results.push_back(rb);
                }
            }

            return results;
        }

        float DistancePointToAABB(const glm::vec3& point, const AABB& aabb) {
            glm::vec3 closestPoint = ClosestPointOnAABB(point, aabb);
            return glm::length(point - closestPoint);
        }

        glm::vec3 ClosestPointOnAABB(const glm::vec3& point, const AABB& aabb) {
            glm::vec3 result;
            result.x = std::max(aabb.Min.x, std::min(point.x, aabb.Max.x));
            result.y = std::max(aabb.Min.y, std::min(point.y, aabb.Max.y));
            result.z = std::max(aabb.Min.z, std::min(point.z, aabb.Max.z));
            return result;
        }
    }

}
