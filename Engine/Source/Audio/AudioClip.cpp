//
// AudioClip.cpp - 音频片段类实现
//

#include "JFMEngine/Audio/AudioClip.h"
#include "JFMEngine/Utils/Log.h"

namespace JFM {

    AudioClip::AudioClip(const std::string& path) : m_Path(path) {
        // 在实际实现中，这里会使用音频库（如OpenAL、FMOD等）加载音频文件
        // 目前提供占位符实现
        m_Duration = 1.0f; // 占位符时长
    }

    AudioClip::~AudioClip() {
        // 在实际实现中，这里会清理音频资源
    }

    void AudioClip::Play() {
        m_IsPlaying = true;
        // 占位符实现 - 实际项目中会调用音频库的播放函数
    }

    void AudioClip::Stop() {
        m_IsPlaying = false;
        // 占位符实现
    }

    void AudioClip::Pause() {
        m_IsPlaying = false;
        // 占位符实现
    }

} // namespace JFM
