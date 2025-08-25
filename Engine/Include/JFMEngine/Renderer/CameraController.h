//
// CameraController.h - 摄像机控制器
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/Events/Event.h"
#include "JFMEngine/Events/MouseEvent.h"
#include "JFMEngine/Events/KeyEvent.h"
#include "JFMEngine/Renderer/Camera.h"
#include <glm/glm.hpp>

namespace JFM {

    class JFM_API CameraController {
    public:
        CameraController(float aspectRatio, bool rotation = false);

        void OnUpdate(float deltaTime);
        void OnEvent(JFM::Event& e);

        void OnResize(float width, float height);

        Camera& GetCamera() { return m_Camera; }
        const Camera& GetCamera() const { return m_Camera; }

        float GetZoomLevel() const { return m_ZoomLevel; }
        void SetZoomLevel(float level) { m_ZoomLevel = level; }
        
        // 鼠标控制设置
        float GetMouseSensitivity() const { return m_MouseSensitivity; }
        void SetMouseSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; }
        
        // 相机位置同步设置
        void SetCameraPosition(const glm::vec3& position) { 
            m_CameraPosition = position; 
            m_Camera.SetPosition(position);
        }

    private:
        bool OnMouseMoved(JFM::MouseMovedEvent& e);
        bool OnMouseScrolled(JFM::MouseScrolledEvent& e);
        bool OnKeyPressed(JFM::KeyPressedEvent& e);
        bool OnKeyReleased(JFM::KeyReleasedEvent& e);

    private:
        float m_AspectRatio;
        float m_ZoomLevel = 1.0f;
        Camera m_Camera;

        bool m_Rotation;

        glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
        float m_CameraRotation = 0.0f; //In degrees, in the anti-clockwise direction
        float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
        
        // 移动状态标志
        bool m_MovingForward = false;
        bool m_MovingBackward = false;
        bool m_MovingLeft = false;
        bool m_MovingRight = false;
        bool m_MovingUp = false;
        bool m_MovingDown = false;
        
        // 鼠标控制相关
        float m_MouseSensitivity = 0.1f;
        bool m_FirstMouse = true;
        float m_LastX = 0.0f;
        float m_LastY = 0.0f;
    };

} // namespace JFM
