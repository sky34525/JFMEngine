//
// Components.h - 标准ECS组件定义
// 定义引擎中常用的基础组件
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "JFMEngine/Core/Core.h"
#include "Component.h"
#include "JFMEngine/Renderer/Light.h"  // 添加 Light.h 头文件
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>
#include <memory>
#include <functional>
#include <algorithm>

// 前向声明
namespace JFM {
    class Mesh;
    class Material;
    enum class LightType;
}

namespace JFM {
    namespace ECS {

        // ========== 核心组件 ==========

        // 变换组件
        struct TransformComponent : public Component<TransformComponent> {
            glm::vec3 Position = glm::vec3(0.0f);
            glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            glm::vec3 Scale = glm::vec3(1.0f);

            // 世界变换（用于层次结构）
            glm::mat4 WorldMatrix = glm::mat4(1.0f);
            Entity Parent = Entity(NULL_ENTITY);
            bool IsDirty = true;

            glm::mat4 GetLocalMatrix() const {
                return glm::translate(glm::mat4(1.0f), Position) *
                       glm::mat4_cast(Rotation) *
                       glm::scale(glm::mat4(1.0f), Scale);
            }

            void SetPosition(const glm::vec3& pos) {
                Position = pos;
                IsDirty = true;
            }

            void SetRotation(const glm::quat& rot) {
                Rotation = rot;
                IsDirty = true;
            }

            void SetScale(const glm::vec3& scale) {
                Scale = scale;
                IsDirty = true;
            }

            // 前向量、右向量、上向量
            glm::vec3 GetForward() const {
                return Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
            }

            glm::vec3 GetRight() const {
                return Rotation * glm::vec3(1.0f, 0.0f, 0.0f);
            }

            glm::vec3 GetUp() const {
                return Rotation * glm::vec3(0.0f, 1.0f, 0.0f);
            }
        };

        // 标签组件
        struct TagComponent : public Component<TagComponent> {
            std::string Tag = "Entity";

            TagComponent() = default;
            TagComponent(const std::string& tag) : Tag(tag) {}
        };

        // 活跃状态组件
        struct ActiveComponent : public Component<ActiveComponent> {
            bool IsActive = true;

            ActiveComponent() = default;
            ActiveComponent(bool active) : IsActive(active) {}
        };

        // ========== 渲染组件 ==========

        // 网格渲染组件
        struct MeshRendererComponent : public Component<MeshRendererComponent> {
            std::shared_ptr<Mesh> MeshData;
            std::shared_ptr<Material> MaterialData;
            bool CastShadows = true;
            bool ReceiveShadows = true;
            int RenderLayer = 0;

            MeshRendererComponent() = default;
            MeshRendererComponent(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
                : MeshData(mesh), MaterialData(material) {}
        };

        // 相机组件
        struct CameraComponent : public Component<CameraComponent> {
            enum class ProjectionType { Perspective, Orthographic };

            ProjectionType Type = ProjectionType::Perspective;
            float FOV = 45.0f;
            float NearPlane = 0.1f;
            float FarPlane = 1000.0f;
            float OrthographicSize = 10.0f;
            bool IsPrimary = false;

            glm::mat4 GetProjectionMatrix(float aspectRatio) const {
                if (Type == ProjectionType::Perspective) {
                    return glm::perspective(glm::radians(FOV), aspectRatio, NearPlane, FarPlane);
                } else {
                    float orthoLeft = -OrthographicSize * aspectRatio * 0.5f;
                    float orthoRight = OrthographicSize * aspectRatio * 0.5f;
                    float orthoBottom = -OrthographicSize * 0.5f;
                    float orthoTop = OrthographicSize * 0.5f;
                    return glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, NearPlane, FarPlane);
                }
            }
        };

        // 光源组件
        struct LightComponent : public Component<LightComponent> {
            LightType Type = LightType::Directional;
            glm::vec3 Color = glm::vec3(1.0f);
            float Intensity = 1.0f;
            float Range = 10.0f;
            float InnerConeAngle = 30.0f;
            float OuterConeAngle = 45.0f;
            bool CastShadows = true;

            LightComponent() = default;
            LightComponent(LightType type, const glm::vec3& color, float intensity)
                : Type(type), Color(color), Intensity(intensity) {}
        };

        // 聚光灯组件
        struct SpotLightComponent : public Component<SpotLightComponent> {
            glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);
            float Intensity = 1.0f;
            float Range = 20.0f;
            float InnerConeAngle = 30.0f;
            float OuterConeAngle = 45.0f;
            glm::vec3 Direction = glm::vec3(0.0f, -1.0f, 0.0f);
            bool CastShadows = true;

            SpotLightComponent() = default;
            SpotLightComponent(const glm::vec3& color, float intensity, float range)
                : Color(color), Intensity(intensity), Range(range) {}
        };

        // 点光源组件
        struct PointLightComponent : public Component<PointLightComponent> {
            glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);
            float Intensity = 1.0f;
            float Range = 10.0f;
            bool CastShadows = true;

            PointLightComponent() = default;
            PointLightComponent(const glm::vec3& color, float intensity, float range)
                : Color(color), Intensity(intensity), Range(range) {}
        };

        // 方向光组件
        struct DirectionalLightComponent : public Component<DirectionalLightComponent> {
            glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);
            float Intensity = 1.0f;
            glm::vec3 Direction = glm::vec3(0.0f, -1.0f, 0.0f);
            bool CastShadows = true;

            DirectionalLightComponent() = default;
            DirectionalLightComponent(const glm::vec3& color, float intensity, const glm::vec3& direction)
                : Color(color), Intensity(intensity), Direction(direction) {}
        };

        // ========== 物理组件 ==========

        // 刚体组件
        struct RigidbodyComponent : public Component<RigidbodyComponent> {
            enum class BodyType { Static, Kinematic, Dynamic };

            BodyType Type = BodyType::Dynamic;
            float Mass = 1.0f;
            float Drag = 0.0f;
            float AngularDrag = 0.05f;
            glm::vec3 Velocity = glm::vec3(0.0f);
            glm::vec3 AngularVelocity = glm::vec3(0.0f);
            bool UseGravity = true;
            bool IsKinematic = false;

            // 物理引擎特定数据（例如Bullet Physics的btRigidBody*）
            void* PhysicsBody = nullptr;
        };

        // 碰撞器组件
        struct ColliderComponent : public Component<ColliderComponent> {
            enum class ColliderType { Box, Sphere, Capsule, Mesh };

            ColliderType Type = ColliderType::Box;
            glm::vec3 Size = glm::vec3(1.0f);       // Box: 尺寸, Sphere: x=半径
            glm::vec3 Center = glm::vec3(0.0f);     // 相对于Transform的偏移
            bool IsTrigger = false;
            float Friction = 0.6f;
            float Restitution = 0.0f;

            // 物理形状数据
            void* CollisionShape = nullptr;
        };

        // ========== 音频组件 ==========

        // 音频源组件
        struct AudioSourceComponent : public Component<AudioSourceComponent> {
            std::string AudioClipPath;
            float Volume = 1.0f;
            float Pitch = 1.0f;
            bool Loop = false;
            bool PlayOnAwake = false;
            bool Is3D = true;
            float MinDistance = 1.0f;
            float MaxDistance = 500.0f;

            // 运行时状态
            bool IsPlaying = false;
            void* AudioSourceHandle = nullptr;
        };

        // ========== 脚本组件 ==========

        // 原生脚本组件（C++脚本）
        struct NativeScriptComponent : public Component<NativeScriptComponent> {
            void* ScriptInstance = nullptr;

            // 脚本生命周期函数指针
            std::function<void()> OnCreateFunction;
            std::function<void()> OnDestroyFunction;
            std::function<void(float)> OnUpdateFunction;

            template<typename T>
            void Bind() {
                OnCreateFunction = [&]() {
                    ScriptInstance = new T();
                    static_cast<T*>(ScriptInstance)->OnCreate();
                };

                OnDestroyFunction = [&]() {
                    static_cast<T*>(ScriptInstance)->OnDestroy();
                    delete static_cast<T*>(ScriptInstance);
                    ScriptInstance = nullptr;
                };

                OnUpdateFunction = [&](float deltaTime) {
                    static_cast<T*>(ScriptInstance)->OnUpdate(deltaTime);
                };
            }
        };

        // ========== 层次结构组件 ==========

        // 子节点组件
        struct ChildrenComponent : public Component<ChildrenComponent> {
            std::vector<Entity> Children;

            void AddChild(Entity child) {
                auto it = std::find(Children.begin(), Children.end(), child);
                if (it == Children.end()) {
                    Children.push_back(child);
                }
            }

            void RemoveChild(Entity child) {
                auto it = std::find(Children.begin(), Children.end(), child);
                if (it != Children.end()) {
                    Children.erase(it);
                }
            }
        };

        // 父节点组件
        struct ParentComponent : public Component<ParentComponent> {
            Entity Parent = Entity(NULL_ENTITY);

            ParentComponent() = default;
            ParentComponent(Entity parent) : Parent(parent) {}
        };

    } // namespace ECS
} // namespace JFM
