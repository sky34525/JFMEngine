//
// AudioClip.h - 音频片段类定义
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include <string>

namespace JFM {

    class JFM_API AudioClip {
    public:
        AudioClip(const std::string& path);
        ~AudioClip();

        void Play();
        void Stop();
        void Pause();

        bool IsPlaying() const { return m_IsPlaying; }
        float GetDuration() const { return m_Duration; }
        const std::string& GetPath() const { return m_Path; }

    private:
        std::string m_Path;
        bool m_IsPlaying = false;
        float m_Duration = 0.0f;
        // 在实际实现中，这里会包含音频库相关的数据
    };

} // namespace JFM
