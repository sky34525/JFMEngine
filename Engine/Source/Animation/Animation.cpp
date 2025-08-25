//
// Animation.cpp - 动画系统实现
//

#include "JFMEngine/Animation/Animation.h"
#include "JFMEngine/Utils/Log.h"
#include <assimp/scene.h>
#include <algorithm>

namespace JFM {

    // AnimationChannel 实现
    AnimationChannel::AnimationChannel(const std::string& name) : m_Name(name) {}

    void AnimationChannel::AddPositionKey(float time, const glm::vec3& position) {
        m_PositionKeys.push_back({time, position});
    }

    void AnimationChannel::AddRotationKey(float time, const glm::quat& rotation) {
        m_RotationKeys.push_back({time, rotation});
    }

    void AnimationChannel::AddScaleKey(float time, const glm::vec3& scale) {
        m_ScaleKeys.push_back({time, scale});
    }

    glm::vec3 AnimationChannel::GetPosition(float time) const {
        if (m_PositionKeys.empty()) return glm::vec3(0.0f);
        if (m_PositionKeys.size() == 1) return m_PositionKeys[0].Value;
        return InterpolatePosition(time);
    }

    glm::quat AnimationChannel::GetRotation(float time) const {
        if (m_RotationKeys.empty()) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        if (m_RotationKeys.size() == 1) return m_RotationKeys[0].Value;
        return InterpolateRotation(time);
    }

    glm::vec3 AnimationChannel::GetScale(float time) const {
        if (m_ScaleKeys.empty()) return glm::vec3(1.0f);
        if (m_ScaleKeys.size() == 1) return m_ScaleKeys[0].Value;
        return InterpolateScale(time);
    }

    glm::mat4 AnimationChannel::GetTransformation(float time) const {
        glm::vec3 position = GetPosition(time);
        glm::quat rotation = GetRotation(time);
        glm::vec3 scale = GetScale(time);

        glm::mat4 trans = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 rot = glm::mat4_cast(rotation);
        glm::mat4 scl = glm::scale(glm::mat4(1.0f), scale);

        return trans * rot * scl;
    }

    glm::vec3 AnimationChannel::InterpolatePosition(float time) const {
        int index = FindPositionIndex(time);
        if (index == m_PositionKeys.size() - 1) {
            return m_PositionKeys[index].Value;
        }

        float deltaTime = m_PositionKeys[index + 1].Time - m_PositionKeys[index].Time;
        float factor = (time - m_PositionKeys[index].Time) / deltaTime;

        const glm::vec3& start = m_PositionKeys[index].Value;
        const glm::vec3& end = m_PositionKeys[index + 1].Value;

        return glm::mix(start, end, factor);
    }

    glm::quat AnimationChannel::InterpolateRotation(float time) const {
        int index = FindRotationIndex(time);
        if (index == m_RotationKeys.size() - 1) {
            return m_RotationKeys[index].Value;
        }

        float deltaTime = m_RotationKeys[index + 1].Time - m_RotationKeys[index].Time;
        float factor = (time - m_RotationKeys[index].Time) / deltaTime;

        const glm::quat& start = m_RotationKeys[index].Value;
        const glm::quat& end = m_RotationKeys[index + 1].Value;

        return glm::slerp(start, end, factor);
    }

    glm::vec3 AnimationChannel::InterpolateScale(float time) const {
        int index = FindScaleIndex(time);
        if (index == m_ScaleKeys.size() - 1) {
            return m_ScaleKeys[index].Value;
        }

        float deltaTime = m_ScaleKeys[index + 1].Time - m_ScaleKeys[index].Time;
        float factor = (time - m_ScaleKeys[index].Time) / deltaTime;

        const glm::vec3& start = m_ScaleKeys[index].Value;
        const glm::vec3& end = m_ScaleKeys[index + 1].Value;

        return glm::mix(start, end, factor);
    }

    int AnimationChannel::FindPositionIndex(float time) const {
        for (size_t i = 0; i < m_PositionKeys.size() - 1; ++i) {
            if (time < m_PositionKeys[i + 1].Time) {
                return i;
            }
        }
        return m_PositionKeys.size() - 1;
    }

    int AnimationChannel::FindRotationIndex(float time) const {
        for (size_t i = 0; i < m_RotationKeys.size() - 1; ++i) {
            if (time < m_RotationKeys[i + 1].Time) {
                return i;
            }
        }
        return m_RotationKeys.size() - 1;
    }

    int AnimationChannel::FindScaleIndex(float time) const {
        for (size_t i = 0; i < m_ScaleKeys.size() - 1; ++i) {
            if (time < m_ScaleKeys[i + 1].Time) {
                return i;
            }
        }
        return m_ScaleKeys.size() - 1;
    }

    // AnimationClip 实现
    AnimationClip::AnimationClip(const std::string& name, float duration, float ticksPerSecond)
        : m_Name(name), m_Duration(duration), m_TicksPerSecond(ticksPerSecond) {}

    void AnimationClip::AddChannel(std::shared_ptr<AnimationChannel> channel) {
        m_Channels[channel->GetName()] = channel;
    }

    std::shared_ptr<AnimationChannel> AnimationClip::GetChannel(const std::string& boneName) const {
        auto it = m_Channels.find(boneName);
        return (it != m_Channels.end()) ? it->second : nullptr;
    }

    std::shared_ptr<AnimationClip> AnimationClip::CreateFromAssimp(const aiAnimation* assimpAnim) {
        auto clip = std::make_shared<AnimationClip>(
            assimpAnim->mName.C_Str(),
            static_cast<float>(assimpAnim->mDuration),
            static_cast<float>(assimpAnim->mTicksPerSecond != 0 ? assimpAnim->mTicksPerSecond : 25.0f)
        );

        // 处理每个动画通道
        for (unsigned int i = 0; i < assimpAnim->mNumChannels; ++i) {
            const aiNodeAnim* nodeAnim = assimpAnim->mChannels[i];
            auto channel = std::make_shared<AnimationChannel>(nodeAnim->mNodeName.C_Str());

            // 添加位置关键帧
            for (unsigned int j = 0; j < nodeAnim->mNumPositionKeys; ++j) {
                const aiVectorKey& key = nodeAnim->mPositionKeys[j];
                channel->AddPositionKey(
                    static_cast<float>(key.mTime),
                    glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z)
                );
            }

            // 添加旋转关键帧
            for (unsigned int j = 0; j < nodeAnim->mNumRotationKeys; ++j) {
                const aiQuatKey& key = nodeAnim->mRotationKeys[j];
                channel->AddRotationKey(
                    static_cast<float>(key.mTime),
                    glm::quat(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z)
                );
            }

            // 添加缩放关键帧
            for (unsigned int j = 0; j < nodeAnim->mNumScalingKeys; ++j) {
                const aiVectorKey& key = nodeAnim->mScalingKeys[j];
                channel->AddScaleKey(
                    static_cast<float>(key.mTime),
                    glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z)
                );
            }

            clip->AddChannel(channel);
        }


        return clip;
    }

    // Animator 实现
    Animator::Animator()
        : m_CurrentTime(0.0f), m_Speed(1.0f), m_IsPlaying(false), m_Loop(true) {}

    void Animator::SetAnimationClip(std::shared_ptr<AnimationClip> clip) {
        m_CurrentClip = clip;
        m_CurrentTime = 0.0f;
        if (clip) {
            m_BoneTransforms.resize(100, glm::mat4(1.0f)); // 最多支持100个骨骼
        }
    }

    void Animator::Update(float deltaTime) {
        if (!m_IsPlaying || !m_CurrentClip) return;

        m_CurrentTime += deltaTime * m_Speed * m_CurrentClip->GetTicksPerSecond();

        if (m_CurrentTime >= m_CurrentClip->GetDuration()) {
            if (m_Loop) {
                m_CurrentTime = fmod(m_CurrentTime, m_CurrentClip->GetDuration());
            } else {
                m_CurrentTime = m_CurrentClip->GetDuration();
                m_IsPlaying = false;
            }
        }

        CalculateBoneTransforms();
    }

    void Animator::Play() {
        m_IsPlaying = true;
    }

    void Animator::Pause() {
        m_IsPlaying = false;
    }

    void Animator::Stop() {
        m_IsPlaying = false;
        m_CurrentTime = 0.0f;
    }

    void Animator::SetTime(float time) {
        if (m_CurrentClip) {
            m_CurrentTime = std::clamp(time, 0.0f, m_CurrentClip->GetDuration());
        }
    }

    float Animator::GetNormalizedTime() const {
        if (!m_CurrentClip || m_CurrentClip->GetDuration() == 0.0f) return 0.0f;
        return m_CurrentTime / m_CurrentClip->GetDuration();
    }

    void Animator::CalculateBoneTransforms() {
        // 这里是简化版本，实际需要根据骨骼层次结构计算
        // 在完整实现中，需要遍历骨骼层次结构并应用动画变换
    }

} // namespace JFM
