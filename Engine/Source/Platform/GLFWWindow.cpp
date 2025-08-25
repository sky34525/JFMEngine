//
// Created by kw on 25-7-23.
// GLFW窗口实现
//

#include "JFMEngine/Platform/GLFWWindow.h"
#include "JFMEngine/Events/ApplicationEvent.h"
#include "JFMEngine/Events/KeyEvent.h"
#include "JFMEngine/Events/MouseEvent.h"
#include "JFMEngine/Utils/Log.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace JFM {

    static bool s_GLFWInitialized = false;

    // GLFW错误回调
    static void GLFWErrorCallback(int error, const char* description) {
        // GLFW错误处理
    }

    // 窗口工厂方法
    std::unique_ptr<Window> Window::Create(const WindowProps& props) {
        return std::make_unique<GLFWWindow>(props);
    }

    GLFWWindow::GLFWWindow(const WindowProps& props) {
        Init(props);
    }

    GLFWWindow::~GLFWWindow() {
        Shutdown();
    }

    void GLFWWindow::Init(const WindowProps& props) {
        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;


        // 初始化GLFW
        if (!s_GLFWInitialized) {
            int success = glfwInit();
            JFM_CORE_ASSERT(success, "Could not initialize GLFW!");
            glfwSetErrorCallback(GLFWErrorCallback);
            s_GLFWInitialized = true;
        }

        // 设置OpenGL版本和配置
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // macOS需要

        // 创建窗口
        m_Window = glfwCreateWindow((int)props.Width, (int)props.Height,
                                   m_Data.Title.c_str(), nullptr, nullptr);

        if (!m_Window) {
            glfwTerminate();
            return;
        }

        // 设置OpenGL上下文
        glfwMakeContextCurrent(m_Window);

        // 将窗口数据与GLFW窗口关联
        glfwSetWindowUserPointer(m_Window, &m_Data);

        // 设置垂直同步
        SetVSync(props.VSync);

        // 设置事件回调
        SetupEventCallbacks();

    }

    void GLFWWindow::Shutdown() {
        if (m_Window) {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }

        // 注意：这里不调用glfwTerminate()，因为可能有其他窗口
    }

    void GLFWWindow::OnUpdate() {
        glfwPollEvents();
    }

    void GLFWWindow::SwapBuffers() {
        glfwSwapBuffers(m_Window);
    }

    void GLFWWindow::SetVSync(bool enabled) {
        if (enabled)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);

        m_Data.VSync = enabled;
    }

    bool GLFWWindow::ShouldClose() const {
        return glfwWindowShouldClose(m_Window);
    }

    void GLFWWindow::Close() {
        glfwSetWindowShouldClose(m_Window, GLFW_TRUE);
    }

    void GLFWWindow::SetupEventCallbacks() {
        // 窗口大小变化回调
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.Width = width;
            data.Height = height;

            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

        // 窗口关闭回调
        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            WindowCloseEvent event;
            data.EventCallback(event);
        });

        // 键盘输入回调
        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action) {
                case GLFW_PRESS: {
                    KeyPressedEvent event(key, 0);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    KeyReleasedEvent event(key);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_REPEAT: {
                    KeyPressedEvent event(key, 1);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        // 字符输入回调
        glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            KeyTypedEvent event(keycode);
            data.EventCallback(event);
        });

        // 鼠标按钮回调
        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action) {
                case GLFW_PRESS: {
                    MouseButtonPressedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    MouseButtonReleasedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        // 鼠标滚轮回调
        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data.EventCallback(event);
        });

        // 鼠标移动回调
        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            MouseMovedEvent event((float)xPos, (float)yPos);
            data.EventCallback(event);
        });
    }

}
