//
// Physics.cpp - 物理系统实现
//

#include "JFMEngine/Physics/Physics.h"
#include <algorithm>
#include <cmath>

namespace JFM {

    // AABB 实现
    bool AABB::Intersects(const AABB& other) const {
        return (Min.x <= other.Max.x && Max.x >= other.Min.x) &&
               (Min.y <= other.Max.y && Max.y >= other.Min.y) &&
               (Min.z <= other.Max.z && Max.z >= other.Min.z);
    }

    // Rigidbody 实现
    Rigidbody::Rigidbody() {
        m_Position = glm::vec3(0.0f);
        m_Velocity = glm::vec3(0.0f);
        m_Force = glm::vec3(0.0f);
        m_Mass = 1.0f;
        m_InvMass = 1.0f;
        m_UseGravity = true;
        m_Drag = 0.98f;
    }

    void Rigidbody::UpdatePhysics(float deltaTime) {
        if (m_InvMass == 0.0f) return; // 静态物体

        // 检查输入参数的有效性
        if (deltaTime <= 0.0f || !std::isfinite(deltaTime)) return;

        // 检查当前状态的有效性
        if (!std::isfinite(m_Position.x) || !std::isfinite(m_Position.y) || !std::isfinite(m_Position.z)) {
            // 重置到安全位置
            m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
            m_Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
            m_Force = glm::vec3(0.0f, 0.0f, 0.0f);
            return;
        }

        // 应用重力
        if (m_UseGravity) {
            glm::vec3 gravity = PhysicsWorld::GetInstance().GetGravity();
            if (std::isfinite(gravity.x) && std::isfinite(gravity.y) && std::isfinite(gravity.z)) {
                m_Force += gravity * m_Mass;
            }
        }

        // 计算加速度 F = ma -> a = F/m
        glm::vec3 acceleration = m_Force * m_InvMass;

        // 检查加速度的有效性
        if (!std::isfinite(acceleration.x) || !std::isfinite(acceleration.y) || !std::isfinite(acceleration.z)) {
            acceleration = glm::vec3(0.0f);
        }

        // 更新速度
        m_Velocity += acceleration * deltaTime;

        // 检查速度的有效性
        if (!std::isfinite(m_Velocity.x) || !std::isfinite(m_Velocity.y) || !std::isfinite(m_Velocity.z)) {
            m_Velocity = glm::vec3(0.0f);
        }

        // 应用阻力
        if (m_Drag > 0.0f && m_Drag <= 1.0f) {
            m_Velocity *= m_Drag;
        }

        // 更新位置
        m_Position += m_Velocity * deltaTime;

        // 检查位置的有效性
        if (!std::isfinite(m_Position.x) || !std::isfinite(m_Position.y) || !std::isfinite(m_Position.z)) {
            m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
            m_Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        }

        // 清除力
        m_Force = glm::vec3(0.0f);
    }

    // BoxCollider 实现
    AABB BoxCollider::GetAABB(const glm::vec3& position) const {
        glm::vec3 halfSize = m_Size * 0.5f;
        return AABB(position - halfSize, position + halfSize);
    }

    bool BoxCollider::CheckCollision(const Collider& other, const glm::vec3& posA, const glm::vec3& posB) const {
        // 简单的AABB碰撞检测
        AABB aabbA = GetAABB(posA);
        AABB aabbB = other.GetAABB(posB);
        return aabbA.Intersects(aabbB);
    }

    // PhysicsWorld 实现
    void PhysicsWorld::Update(float deltaTime) {
        if (m_Paused) return;

        // 累积时间，使用固定时间步长
        m_AccumulatedTime += deltaTime;

        int subSteps = 0;
        while (m_AccumulatedTime >= m_FixedTimeStep && subSteps < m_MaxSubSteps) {
            // 更新所有刚体
            for (auto& rigidbody : m_Rigidbodies) {
                if (rigidbody) {
                    rigidbody->UpdatePhysics(m_FixedTimeStep);
                }
            }

            // 简单的碰撞检测和响应
            for (size_t i = 0; i < m_Rigidbodies.size(); ++i) {
                for (size_t j = i + 1; j < m_Rigidbodies.size(); ++j) {
                    auto& rbA = m_Rigidbodies[i];
                    auto& rbB = m_Rigidbodies[j];

                    if (rbA && rbB) {
                        // 简单的碰撞响应：弹性碰撞
                        float distance = glm::length(rbA->m_Position - rbB->m_Position);
                        float minDistance = 1.0f; // 最小距离阈值

                        if (distance < minDistance && distance > 0.0f) {
                            // 计算碰撞法向量
                            glm::vec3 normal = glm::normalize(rbA->m_Position - rbB->m_Position);

                            // 检查法向量是否有效
                            if (glm::length(normal) < 0.001f) {
                                // 如果法向量无效，使用默认分离方向
                                normal = glm::vec3(1.0f, 0.0f, 0.0f);
                            }

                            // 分离物体
                            float overlap = minDistance - distance;
                            float totalMass = rbA->GetMass() + rbB->GetMass();

                            if (totalMass > 0.0f) {
                                float ratioA = rbB->GetMass() / totalMass;
                                float ratioB = rbA->GetMass() / totalMass;

                                rbA->m_Position += normal * overlap * ratioA;
                                rbB->m_Position -= normal * overlap * ratioB;

                                // 弹性碰撞速度计算
                                float restitution = 0.8f; // 恢复系数
                                glm::vec3 relativeVelocity = rbA->m_Velocity - rbB->m_Velocity;
                                float velocityAlongNormal = glm::dot(relativeVelocity, normal);

                                if (velocityAlongNormal > 0) continue; // 物体正在分离

                                float impulse = -(1 + restitution) * velocityAlongNormal;
                                float invMassSum = 1.0f/rbA->GetMass() + 1.0f/rbB->GetMass();

                                // 防止除零
                                if (invMassSum > 0.0f) {
                                    impulse /= invMassSum;

                                    glm::vec3 impulseVector = impulse * normal;
                                    rbA->m_Velocity += impulseVector / rbA->GetMass();
                                    rbB->m_Velocity -= impulseVector / rbB->GetMass();
                                }
                            }
                        }
                    }
                }
            }

            m_AccumulatedTime -= m_FixedTimeStep;
            subSteps++;
        }

        // 清理空指针
        m_Rigidbodies.erase(
            std::remove_if(m_Rigidbodies.begin(), m_Rigidbodies.end(),
                [](const std::shared_ptr<Rigidbody>& ptr) { return !ptr; }),
            m_Rigidbodies.end()
        );
    }

    void PhysicsWorld::AddRigidbody(std::shared_ptr<Rigidbody> rigidbody) {
        if (rigidbody) {
            m_Rigidbodies.push_back(rigidbody);
        }
    }

    void PhysicsWorld::RemoveRigidbody(std::shared_ptr<Rigidbody> rigidbody) {
        m_Rigidbodies.erase(
            std::remove(m_Rigidbodies.begin(), m_Rigidbodies.end(), rigidbody),
            m_Rigidbodies.end()
        );
    }

}
