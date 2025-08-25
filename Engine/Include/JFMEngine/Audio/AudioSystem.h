//
// AudioSystem.h - 3D音频系统
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <unordered_map>

namespace JFM {

    // 音频剪辑
    class JFM_API AudioClip {
    public:
        AudioClip(const std::string& filepath);
        ~AudioClip();

        bool IsLoaded() const { return m_Loaded; }
        float GetDuration() const { return m_Duration; }
        uint32_t GetSampleRate() const { return m_SampleRate; }
        uint32_t GetChannels() const { return m_Channels; }

    private:
        uint32_t m_Buffer = 0;
        float m_Duration = 0.0f;
        uint32_t m_SampleRate = 0;
        uint32_t m_Channels = 0;
        bool m_Loaded = false;

        friend class AudioSource;
    };

    // 3D音频源
    class JFM_API AudioSource {
    public:
        AudioSource();
        ~AudioSource();

        // 音频控制
        void Play();
        void Pause();
        void Stop();
        void SetClip(std::shared_ptr<AudioClip> clip);

        // 音量和音调
        void SetVolume(float volume) { m_Volume = volume; UpdateSource(); }
        void SetPitch(float pitch) { m_Pitch = pitch; UpdateSource(); }
        float GetVolume() const { return m_Volume; }
        float GetPitch() const { return m_Pitch; }

        // 3D空间音频
        void SetPosition(const glm::vec3& position) { m_Position = position; UpdateSource(); }
        void SetVelocity(const glm::vec3& velocity) { m_Velocity = velocity; UpdateSource(); }
        glm::vec3 GetPosition() const { return m_Position; }

        // 衰减设置
        void SetMinDistance(float distance) { m_MinDistance = distance; UpdateSource(); }
        void SetMaxDistance(float distance) { m_MaxDistance = distance; UpdateSource(); }
        void SetRolloffFactor(float factor) { m_RolloffFactor = factor; UpdateSource(); }

        // 循环播放
        void SetLooping(bool loop) { m_Looping = loop; UpdateSource(); }
        bool IsLooping() const { return m_Looping; }

        // 状态查询
        bool IsPlaying() const;
        bool IsPaused() const;
        float GetPlaybackPosition() const;

    private:
        uint32_t m_Source = 0;
        std::shared_ptr<AudioClip> m_Clip;

        float m_Volume = 1.0f;
        float m_Pitch = 1.0f;
        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::vec3 m_Velocity = glm::vec3(0.0f);

        float m_MinDistance = 1.0f;
        float m_MaxDistance = 100.0f;
        float m_RolloffFactor = 1.0f;
        bool m_Looping = false;

        void UpdateSource();
    };

    // 音频监听器（相机音频）
    class JFM_API AudioListener {
    public:
        static void SetPosition(const glm::vec3& position);
        static void SetVelocity(const glm::vec3& velocity);
        static void SetOrientation(const glm::vec3& forward, const glm::vec3& up);

        static void SetMasterVolume(float volume);
        static float GetMasterVolume();

    private:
        static glm::vec3 s_Position;
        static glm::vec3 s_Velocity;
        static glm::vec3 s_Forward;
        static glm::vec3 s_Up;
        static float s_MasterVolume;
    };

    // 音频管理器
    class JFM_API AudioManager {
    public:
        static AudioManager& GetInstance() {
            static AudioManager instance;
            return instance;
        }

        bool Initialize();
        void Shutdown();
        void Update();

        // 资源管理
        std::shared_ptr<AudioClip> LoadClip(const std::string& filepath);
        void UnloadClip(const std::string& filepath);

        // 快速播放
        void PlayOneShot(const std::string& filepath, float volume = 1.0f);
        void PlayOneShot3D(const std::string& filepath, const glm::vec3& position, float volume = 1.0f);

        // 音频源池
        std::shared_ptr<AudioSource> CreateSource();
        void ReturnSource(std::shared_ptr<AudioSource> source);

    private:
        bool m_Initialized = false;
        std::unordered_map<std::string, std::shared_ptr<AudioClip>> m_LoadedClips;
        std::vector<std::shared_ptr<AudioSource>> m_SourcePool;
        std::vector<std::shared_ptr<AudioSource>> m_ActiveSources;
    };

}
