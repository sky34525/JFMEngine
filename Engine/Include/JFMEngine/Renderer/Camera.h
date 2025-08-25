#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace JFM {

class Camera {
public:
    //透视投影相机
    Camera(float fov, float aspect, float nearClip, float farClip)
        : m_Position(0.0f, 0.0f, 3.0f), m_Pitch(0.0f), m_Yaw(-90.0f),
          m_Fov(fov), m_Aspect(aspect), m_Near(nearClip), m_Far(farClip) {}

    Camera(const glm::vec3& position, float pitch, float yaw, float fov, float aspect, float nearClip, float farClip)
        : m_Position(position), m_Pitch(pitch), m_Yaw(yaw),
          m_Fov(fov), m_Aspect(aspect), m_Near(nearClip), m_Far(farClip) {}

    void SetPosition(const glm::vec3& pos) { m_Position = pos; }
    void SetRotation(float pitch, float yaw) { m_Pitch = pitch; m_Yaw = yaw; }
    void SetFov(float fov) { m_Fov = fov; }
    glm::vec3 GetPosition() const { return m_Position; }
    float GetPitch() const { return m_Pitch; }
    float GetYaw() const { return m_Yaw; }
    float GetFov() const { return m_Fov; }
    //根据相机属性生成渲染所需的视图矩阵和投影矩阵
    glm::mat4 GetViewMatrix() const {
        glm::vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        return glm::lookAt(m_Position, m_Position + glm::normalize(front), glm::vec3(0, 1, 0));
    }
    glm::mat4 GetProjectionMatrix() const {
        return glm::perspective(glm::radians(m_Fov), m_Aspect, m_Near, m_Far);
    }

    glm::mat4 GetViewProjectionMatrix() const {
        return GetProjectionMatrix() * GetViewMatrix();
    }

    void LookAt(const glm::vec3& target) {
        glm::vec3 direction = glm::normalize(target - m_Position);
        m_Pitch = glm::degrees(asin(direction.y));
        // 修复yaw角度计算 - 使用正确的atan2参数顺序
        m_Yaw = glm::degrees(atan2(direction.x, direction.z));
    }
private:
    glm::vec3 m_Position;// 相机位置
    float m_Pitch, m_Yaw;// 相机俯仰角和偏航角
    float m_Fov, m_Aspect, m_Near, m_Far;// 相机视野、宽高比、近裁剪面和远裁剪面
};

} // namespace JFM
