//
// Created by kw on 25-7-16.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/Events/Event.h"
#include "JFMEngine/Events/ApplicationEvent.h"
#include "JFMEngine/Events/KeyEvent.h"
#include "JFMEngine/Events/MouseEvent.h"
#include "JFMEngine/Core/EventSystem.h"
#include "JFMEngine/Core/LayerStack.h"

// 前向声明GLFWwindow以避免头文件冲突
struct GLFWwindow;

namespace JFM {

    class JFM_API Application
    {
    public:
        Application();
        virtual ~Application();

        void Run();

        // 图层管理
        void PushLayer(std::shared_ptr<Layer> layer);
        void PushOverlay(std::shared_ptr<Layer> overlay);

        virtual void OnUpdate() {}
        virtual void OnEvent(JFM::Event& e);

        // 获取应用程序实例（单例模式）
        static Application& Get() { return *s_Instance; }

    protected:
        // 事件处理方法 - 子类可以重写这些方法来处理特定事件
        virtual bool OnWindowClose(JFM::WindowCloseEvent& e);
        virtual bool OnWindowResize(JFM::WindowResizeEvent& e);
        virtual bool OnKeyPressed(JFM::KeyPressedEvent& e);
        virtual bool OnKeyReleased(JFM::KeyReleasedEvent& e);
        virtual bool OnMouseButtonPressed(JFM::MouseButtonPressedEvent& e);
        virtual bool OnMouseButtonReleased(JFM::MouseButtonReleasedEvent& e);
        virtual bool OnMouseMoved(JFM::MouseMovedEvent& e);
        virtual bool OnMouseScrolled(JFM::MouseScrolledEvent& e);

    private:
        void SetupEventCallbacks();
        void InitializeEventSystem();
        void RegisterEventHandlers();

        bool m_Running = true;
        GLFWwindow* m_Window = nullptr;
        LayerStack m_LayerStack;

        static Application* s_Instance;
    };

    // 在客户端中定义
    Application* CreateApplication();

}

#endif //APPLICATION_H
