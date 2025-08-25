//
// Created by kw on 25-7-23.
// GLFW窗口实现 - 跨平台窗口系统
//

#ifndef GLFWWINDOW_H
#define GLFWWINDOW_H

#include "Window.h"
#include "../Events/ApplicationEvent.h"
#include "../Events/KeyEvent.h"
#include "../Events/MouseEvent.h"

struct GLFWwindow;

namespace JFM {

    class JFM_API GLFWWindow : public Window {
    public:
        GLFWWindow(const WindowProps& props);
        virtual ~GLFWWindow();

        void OnUpdate() override;
        void SwapBuffers() override;

        // 窗口属性
        unsigned int GetWidth() const override { return m_Data.Width; }
        unsigned int GetHeight() const override { return m_Data.Height; }
        const std::string& GetTitle() const override { return m_Data.Title; }

        // 窗口设置
        void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
        void SetVSync(bool enabled) override;
        bool IsVSync() const override { return m_Data.VSync; }

        // 窗口状态
        bool ShouldClose() const override;
        void Close() override;

        // 获取原生窗口句柄
        void* GetNativeWindow() const override { return m_Window; }

    private:
        virtual void Init(const WindowProps& props);
        virtual void Shutdown();

        // 设置GLFW回调函数
        void SetupEventCallbacks();

        GLFWwindow* m_Window;

        // 窗口数据结构
        struct WindowData {
            std::string Title;
            unsigned int Width, Height;
            bool VSync;

            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
    };

}

#endif //GLFWWINDOW_H
