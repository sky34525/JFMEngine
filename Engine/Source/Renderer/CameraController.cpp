#include "JFMEngine/Renderer/CameraController.h"
#include "JFMEngine/Input/KeyCodes.h"
#include <GLFW/glfw3.h>
#include <algorithm>

namespace JFM {

    CameraController::CameraController(float aspectRatio, bool rotation)
        : m_AspectRatio(aspectRatio), m_Camera(45.0f, aspectRatio, 0.1f, 1000.0f), m_Rotation(rotation) {
    }

    void CameraController::OnUpdate(float deltaTime) {
        // 计算相机前方向和右方向向量
        float yawRad = glm::radians(m_Camera.GetYaw());
        float pitchRad = glm::radians(m_Camera.GetPitch());

        //通过相机的俯仰角和偏航角计算
        glm::vec3 front;//相机观察方向向量
        front.x = cos(yawRad) * cos(pitchRad);
        front.y = sin(pitchRad);
        front.z = sin(yawRad) * cos(pitchRad);
        front = glm::normalize(front);
        
        glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));//通过front向量和世界y轴的叉积得到right
        glm::vec3 up = glm::normalize(glm::cross(right, front));////通过right和front的叉积得到up
        
        // 根据按键状态更新相机位置
        float velocity = m_CameraTranslationSpeed * deltaTime;
        
        if (m_MovingForward)
            m_CameraPosition += front * velocity;
        if (m_MovingBackward)
            m_CameraPosition -= front * velocity;
        if (m_MovingRight)
            m_CameraPosition += right * velocity;
        if (m_MovingLeft)
            m_CameraPosition -= right * velocity;
        if (m_MovingUp)
            m_CameraPosition += up * velocity;
        if (m_MovingDown)
            m_CameraPosition -= up * velocity;
            
        // 将位置更新应用到相机
        m_Camera.SetPosition(m_CameraPosition);
    }

    void CameraController::OnEvent(JFM::Event& e) {
        JFM::EventDispatcher dispatcher(e);
        dispatcher.Dispatch<JFM::MouseMovedEvent>([this](JFM::MouseMovedEvent& event) {
            return OnMouseMoved(event);
        });
        dispatcher.Dispatch<JFM::MouseScrolledEvent>([this](JFM::MouseScrolledEvent& event) {
            return OnMouseScrolled(event);
        });
        dispatcher.Dispatch<JFM::KeyPressedEvent>([this](JFM::KeyPressedEvent& event) {
            return OnKeyPressed(event);
        });
        dispatcher.Dispatch<JFM::KeyReleasedEvent>([this](JFM::KeyReleasedEvent& event) {
            return OnKeyReleased(event);
        });
    }

    void CameraController::OnResize(float width, float height) {
        m_AspectRatio = width / height;
    }

    bool CameraController::OnMouseMoved(JFM::MouseMovedEvent& e) {
        float xpos = e.GetX();
        float ypos = e.GetY();

        if (m_FirstMouse) {
            m_LastX = xpos;
            m_LastY = ypos;
            m_FirstMouse = false;
        }

        float xoffset = xpos - m_LastX;
        float yoffset = m_LastY - ypos; // Y坐标翻转
        m_LastX = xpos;
        m_LastY = ypos;

        xoffset *= m_MouseSensitivity;
        yoffset *= m_MouseSensitivity;

        // 获取当前相机角度
        float yaw = m_Camera.GetYaw();
        float pitch = m_Camera.GetPitch();

        // 更新角度
        yaw += xoffset;
        pitch += yoffset;

        // 限制pitch角度，防止翻转
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        // 应用新角度到相机
        m_Camera.SetRotation(pitch, yaw);
        
        return true;
    }

    bool CameraController::OnMouseScrolled(JFM::MouseScrolledEvent& e) {
        // 控制FOV来实现缩放效果
        float fov = m_Camera.GetFov();
        fov -= e.GetYOffset() * 2.0f;
        
        // 限制FOV范围
        if (fov < 1.0f)
            fov = 1.0f;
        if (fov > 120.0f)
            fov = 120.0f;
            
        m_Camera.SetFov(fov);
        
        // 同时更新缩放级别用于移动速度调整
        m_ZoomLevel = fov / 45.0f; // 标准化到45度FOV
        
        return true;
    }

    bool CameraController::OnKeyPressed(JFM::KeyPressedEvent& e) {
        switch (e.GetKeyCode()) {
            case Key::W:
                m_MovingForward = true;
                return true;
            case Key::S:
                m_MovingBackward = true;
                return true;
            case Key::A:
                m_MovingLeft = true;
                return true;
            case Key::D:
                m_MovingRight = true;
                return true;
            case Key::SPACE:
                m_MovingUp = true;
                return true;
            case Key::LEFT_SHIFT:
                m_MovingDown = true;
                return true;
        }
        return false;
    }

    bool CameraController::OnKeyReleased(JFM::KeyReleasedEvent& e) {
        switch (e.GetKeyCode()) {
            case Key::W:
                m_MovingForward = false;
                return true;
            case Key::S:
                m_MovingBackward = false;
                return true;
            case Key::A:
                m_MovingLeft = false;
                return true;
            case Key::D:
                m_MovingRight = false;
                return true;
            case Key::SPACE:
                m_MovingUp = false;
                return true;
            case Key::LEFT_SHIFT:
                m_MovingDown = false;
                return true;
        }
        return false;
    }

} // namespace JFM
