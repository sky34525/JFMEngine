//
// Animation.h - 动画系统核心组件
// 支持骨骼动画、关键帧动画等
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

// Assimp前向声明
struct aiAnimation;
struct aiNodeAnim;
struct aiScene;

namespace JFM {

    // 关键帧数据结构
    struct VectorKey {
        float Time;
        glm::vec3 Value;
    };

    struct QuatKey {
        float Time;
        glm::quat Value;
    };

    // 骨骼类
    struct Bone {
        std::string Name;
        int ID;
        glm::mat4 OffsetMatrix;
        glm::mat4 FinalTransformation;
    };

    // 动画通道（控制单个骨骼的动画）
    class JFM_API AnimationChannel {
    public:
        AnimationChannel(const std::string& name);

        void AddPositionKey(float time, const glm::vec3& position);
        void AddRotationKey(float time, const glm::quat& rotation);
        void AddScaleKey(float time, const glm::vec3& scale);

        glm::vec3 GetPosition(float time) const;
        glm::quat GetRotation(float time) const;
        glm::vec3 GetScale(float time) const;

        glm::mat4 GetTransformation(float time) const;

        const std::string& GetName() const { return m_Name; }

    private:
        std::string m_Name;
        std::vector<VectorKey> m_PositionKeys;
        std::vector<QuatKey> m_RotationKeys;
        std::vector<VectorKey> m_ScaleKeys;

        // 插值辅助函数
        glm::vec3 InterpolatePosition(float time) const;
        glm::quat InterpolateRotation(float time) const;
        glm::vec3 InterpolateScale(float time) const;

        int FindPositionIndex(float time) const;
        int FindRotationIndex(float time) const;
        int FindScaleIndex(float time) const;
    };

    // 动画剪辑
    class JFM_API AnimationClip {
    public:
        AnimationClip(const std::string& name, float duration, float ticksPerSecond = 25.0f);

        void AddChannel(std::shared_ptr<AnimationChannel> channel);
        std::shared_ptr<AnimationChannel> GetChannel(const std::string& boneName) const;

        float GetDuration() const { return m_Duration; }
        float GetTicksPerSecond() const { return m_TicksPerSecond; }
        const std::string& GetName() const { return m_Name; }

        // 从Assimp动画创建
        static std::shared_ptr<AnimationClip> CreateFromAssimp(const aiAnimation* assimpAnim);

    private:
        std::string m_Name;
        float m_Duration;
        float m_TicksPerSecond;
        std::unordered_map<std::string, std::shared_ptr<AnimationChannel>> m_Channels;
    };

    // 骨骼层次结构节点
    struct AnimationNode {
        std::string Name;
        glm::mat4 Transformation;
        std::vector<std::shared_ptr<AnimationNode>> Children;

        AnimationNode(const std::string& name) : Name(name), Transformation(1.0f) {}
    };

    // 动画器 - 控制动画播放
    class JFM_API Animator {
    public:
        Animator();

        void SetAnimationClip(std::shared_ptr<AnimationClip> clip);
        void Update(float deltaTime);
        void Play();
        void Pause();
        void Stop();
        void SetTime(float time);
        void SetSpeed(float speed) { m_Speed = speed; }
        void SetLoop(bool loop) { m_Loop = loop; }

        bool IsPlaying() const { return m_IsPlaying; }
        float GetCurrentTime() const { return m_CurrentTime; }
        float GetNormalizedTime() const;

        // 获取当前骨骼变换矩阵
        const std::vector<glm::mat4>& GetBoneTransforms() const { return m_BoneTransforms; }

    private:
        std::shared_ptr<AnimationClip> m_CurrentClip;
        float m_CurrentTime;
        float m_Speed;
        bool m_IsPlaying;
        bool m_Loop;

        std::vector<glm::mat4> m_BoneTransforms;
        void CalculateBoneTransforms();
    };

    // 骨骼网格（支持骨骼动画的网格）
    class JFM_API SkeletalMesh {
    public:
        SkeletalMesh();

        void LoadFromAssimp(const aiScene* scene);
        void SetAnimator(std::shared_ptr<Animator> animator) { m_Animator = animator; }

        void Update(float deltaTime);
        void Draw(const std::shared_ptr<class Shader>& shader) const;

        // 骨骼管理
        void AddBone(const Bone& bone);
        int GetBoneID(const std::string& name) const;
        const std::vector<Bone>& GetBones() const { return m_Bones; }

    private:
        std::vector<Bone> m_Bones;
        std::unordered_map<std::string, int> m_BoneMap;
        std::shared_ptr<AnimationNode> m_RootNode;
        std::shared_ptr<Animator> m_Animator;

        // 递归计算骨骼变换
        void CalculateNodeTransforms(std::shared_ptr<AnimationNode> node,
                                   const glm::mat4& parentTransform);
    };

} // namespace JFM
