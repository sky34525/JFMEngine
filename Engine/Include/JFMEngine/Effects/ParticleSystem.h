//
// ParticleSystem.h - 3D粒子系统
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/Renderer/Texture.h"
#include "JFMEngine/Renderer/Shader.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <random>

namespace JFM {

    // 粒子数据
    struct Particle {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec3 Velocity = glm::vec3(0.0f);
        glm::vec3 Acceleration = glm::vec3(0.0f);
        glm::vec4 Color = glm::vec4(1.0f);

        float Life = 1.0f;
        float MaxLife = 1.0f;
        float Size = 1.0f;
        float Rotation = 0.0f;
        float AngularVelocity = 0.0f;

        bool Active = false;
    };

    // 发射器形状
    enum class EmitterShape {
        POINT,
        SPHERE,
        BOX,
        CONE,
        CIRCLE
    };

    // 粒子系统配置
    struct ParticleSystemConfig {
        // 发射参数
        uint32_t MaxParticles = 1000;
        float EmissionRate = 50.0f;  // 每秒发射数量

        // 生命周期
        float LifeTime = 5.0f;
        float LifeTimeVariation = 1.0f;

        // 初始属性
        glm::vec3 StartVelocity = glm::vec3(0.0f, 5.0f, 0.0f);
        glm::vec3 VelocityVariation = glm::vec3(2.0f, 1.0f, 2.0f);

        glm::vec4 StartColor = glm::vec4(1.0f);
        glm::vec4 EndColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);

        float StartSize = 1.0f;
        float EndSize = 2.0f;
        float SizeVariation = 0.2f;

        // 物理属性
        glm::vec3 Gravity = glm::vec3(0.0f, -9.81f, 0.0f);
        float Damping = 0.98f;

        // 发射器形状
        EmitterShape Shape = EmitterShape::POINT;
        glm::vec3 ShapeSize = glm::vec3(1.0f);

        // 渲染属性
        std::shared_ptr<Texture> Texture = nullptr;
        bool Billboard = true;
        bool AdditiveBlending = true;
    };

    // 粒子系统
    class JFM_API ParticleSystem {
    public:
        ParticleSystem(const ParticleSystemConfig& config);
        ~ParticleSystem();

        // 控制
        void Start() { m_IsPlaying = true; }
        void Stop() { m_IsPlaying = false; }
        void Pause() { m_IsPaused = true; }
        void Resume() { m_IsPaused = false; }
        void Reset();

        // 位置和变换
        void SetPosition(const glm::vec3& position) { m_Position = position; }
        glm::vec3 GetPosition() const { return m_Position; }

        void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }
        void SetScale(float scale) { m_Scale = scale; }

        // 配置修改
        void SetConfig(const ParticleSystemConfig& config) { m_Config = config; }
        const ParticleSystemConfig& GetConfig() const { return m_Config; }

        void SetEmissionRate(float rate) { m_Config.EmissionRate = rate; }
        void SetMaxParticles(uint32_t max);

        // 更新和渲染
        void Update(float deltaTime);
        void Render(const glm::mat4& viewMatrix, const glm::mat4& projMatrix);

        // 统计信息
        uint32_t GetActiveParticleCount() const { return m_ActiveParticles; }
        bool IsPlaying() const { return m_IsPlaying; }

        // 爆发发射
        void Burst(uint32_t count);

    private:
        ParticleSystemConfig m_Config;
        std::vector<Particle> m_Particles;

        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::vec3 m_Rotation = glm::vec3(0.0f);
        float m_Scale = 1.0f;

        bool m_IsPlaying = false;
        bool m_IsPaused = false;
        uint32_t m_ActiveParticles = 0;

        float m_EmissionTimer = 0.0f;

        // 渲染资源
        uint32_t m_VAO = 0;
        uint32_t m_VBO = 0;
        std::shared_ptr<Shader> m_ParticleShader;

        // 随机数生成
        std::mt19937 m_RandomEngine;
        std::uniform_real_distribution<float> m_Distribution;

        void InitializeRenderResources();
        void EmitParticle();
        void UpdateParticle(Particle& particle, float deltaTime);
        glm::vec3 GetEmissionPosition();
        glm::vec3 GetEmissionVelocity();
        float Random(float min, float max);
    };

    // 粒子系统管理器
    class JFM_API ParticleManager {
    public:
        static ParticleManager& GetInstance() {
            static ParticleManager instance;
            return instance;
        }

        void Initialize();
        void Shutdown();

        std::shared_ptr<ParticleSystem> CreateSystem(const ParticleSystemConfig& config);
        void DestroySystem(std::shared_ptr<ParticleSystem> system);

        void UpdateAll(float deltaTime);
        void RenderAll(const glm::mat4& viewMatrix, const glm::mat4& projMatrix);

        // 预设效果
        std::shared_ptr<ParticleSystem> CreateFire(const glm::vec3& position);
        std::shared_ptr<ParticleSystem> CreateSmoke(const glm::vec3& position);
        std::shared_ptr<ParticleSystem> CreateExplosion(const glm::vec3& position);
        std::shared_ptr<ParticleSystem> CreateRain(const glm::vec3& position, const glm::vec3& area);
        std::shared_ptr<ParticleSystem> CreateSnow(const glm::vec3& position, const glm::vec3& area);

    private:
        std::vector<std::shared_ptr<ParticleSystem>> m_Systems;
        bool m_Initialized = false;
    };

}
