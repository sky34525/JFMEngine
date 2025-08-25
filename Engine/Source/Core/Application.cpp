//
// Created by kw on 25-7-16.
//

#include "JFMEngine/Core/Application.h"
#include "JFMEngine/Utils/Log.h"
#include "JFMEngine/Events/KeyEvent.h"
#include "JFMEngine/Events/MouseEvent.h"
#include "JFMEngine/Renderer/Renderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace JFM {

    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        JFM_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        // 初始化GLFW
        if (!glfwInit())
        {
            return;
        }

        // 创建窗口
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_Window = glfwCreateWindow(800, 600, "JFMengine", nullptr, nullptr);
        if (!m_Window)
        {
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(m_Window);

        // 初始化GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            return;
        }

        // 初始化渲染系统
        Renderer::Init();

        // 设置窗口用户指针
        glfwSetWindowUserPointer(m_Window, this);

        // 初始化Core事件系统
        InitializeEventSystem();

        // 注册事件处理器
        RegisterEventHandlers();

        // 设置GLFW事件回调
        SetupEventCallbacks();

    }

    Application::~Application()
    {

        // 清理渲染系统
        Renderer::Shutdown();

        // 关闭Core事件系统
        JFM::EventSystem::GetInstance().Shutdown();

        if (m_Window)
        {
            glfwDestroyWindow(m_Window);
        }
        glfwTerminate();
    }

    void Application::Run()
    {

        // 初始化时间变量
        float lastFrameTime = 0.0f;

        while (!glfwWindowShouldClose(m_Window) && m_Running)
        {
            // 计算deltaTime
            float time = (float)glfwGetTime();
            float deltaTime = time - lastFrameTime;
            lastFrameTime = time;

            // 收集输入事件
            glfwPollEvents();

            // 处理Core事件系统中的事件
            JFM::EventSystem::GetInstance().ProcessEvents();

            // 更新所有图层 - 传递deltaTime参数
            for (auto& layer : m_LayerStack)
            {
                if (layer && layer->IsEnabled())
                {
                    layer->OnUpdate(deltaTime);
                }
            }

            // 应用程序更新
            OnUpdate();

            // 渲染所有图层 - 这是缺失的重要步骤！
            for (auto& layer : m_LayerStack)
            {
                if (layer && layer->IsEnabled())
                {
                    layer->OnRender();
                }
            }

            // 让图层设置渲染状态（包括清屏颜色）
            for (auto& layer : m_LayerStack)
            {
                if (layer && layer->IsEnabled())//防御性编程
                {
                    layer->OnImGuiRender();//准备渲染指令
                }
            }

            // 执行渲染
            glfwSwapBuffers(m_Window);
        }
    }

    void Application::PushLayer(std::shared_ptr<Layer> layer)
    {
        m_LayerStack.PushLayer(layer);
    }

    void Application::PushOverlay(std::shared_ptr<Layer> overlay)
    {
        m_LayerStack.PushOverlay(overlay);
    }

    void Application::OnEvent(JFM::Event& e)
    {
        // 将事件传递给图层栈（从顶层到底层）
        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
        {
            if (*it && (*it)->IsEnabled())
            {
                (*it)->OnEvent(e);
                if (e.Handled)
                    break;
            }
        }

        // 应用程序层的事件处理（用于调试和监控）
        // 事件已被处理
    }

    void Application::InitializeEventSystem()
    {
        // 初始化Core事件系统，使用多线程处理
        JFM::EventSystem::GetInstance().Initialize(2);
    }

    void Application::RegisterEventHandlers()
    {
        auto& eventSystem = JFM::EventSystem::GetInstance();

        // 注册窗口事件处理器
        eventSystem.RegisterHandler<JFM::WindowCloseEvent>(
            [this](JFM::WindowCloseEvent& e) -> bool {
                // 先让Application处理
                bool handled = OnWindowClose(e);
                // 如果没有被处理，传递给图层
                if (!handled) {
                    OnEvent(e);
                }
                return handled;
            }, 100); // 高优先级

        eventSystem.RegisterHandler<JFM::WindowResizeEvent>(
            [this](JFM::WindowResizeEvent& e) -> bool {
                bool handled = OnWindowResize(e);
                if (!handled) {
                    OnEvent(e);
                }
                return handled;
            }, 90);

        // 注册键盘事件处理器
        eventSystem.RegisterHandler<JFM::KeyPressedEvent>(
            [this](JFM::KeyPressedEvent& e) -> bool {
                // 先让Application处理
                bool handled = OnKeyPressed(e);
                // 然后传递给图层系统
                OnEvent(e);
                return handled;
            }, 80);

        eventSystem.RegisterHandler<JFM::KeyReleasedEvent>(
            [this](JFM::KeyReleasedEvent& e) -> bool {
                bool handled = OnKeyReleased(e);
                OnEvent(e);
                return handled;
            }, 80);

        // 注册鼠标事件处理器
        eventSystem.RegisterHandler<JFM::MouseButtonPressedEvent>(
            [this](JFM::MouseButtonPressedEvent& e) -> bool {
                bool handled = OnMouseButtonPressed(e);
                OnEvent(e);
                return handled;
            }, 70);

        eventSystem.RegisterHandler<JFM::MouseButtonReleasedEvent>(
            [this](JFM::MouseButtonReleasedEvent& e) -> bool {
                bool handled = OnMouseButtonReleased(e);
                OnEvent(e);
                return handled;
            }, 70);

        eventSystem.RegisterHandler<JFM::MouseMovedEvent>(
            [this](JFM::MouseMovedEvent& e) -> bool {
                bool handled = OnMouseMoved(e);
                OnEvent(e);
                return handled;
            }, 50); // 较低优先级，因为这类事件频繁

        eventSystem.RegisterHandler<JFM::MouseScrolledEvent>(
            [this](JFM::MouseScrolledEvent& e) -> bool {
                bool handled = OnMouseScrolled(e);
                OnEvent(e);
                return handled;
            }, 60);

    }

    // 默认事件处理实现 - 子类可以重写
    bool Application::OnWindowClose(JFM::WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }

    bool Application::OnWindowResize(JFM::WindowResizeEvent& e)
    {
        // 通知渲染系统窗口大小变化
        Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

        return false; // 允许其他处理器处理此事件
    }

    bool Application::OnKeyPressed(JFM::KeyPressedEvent& e)
    {
        // 基类默认不处理键盘事件，交给子类
        return false;
    }

    bool Application::OnKeyReleased(JFM::KeyReleasedEvent& e)
    {
        return false;
    }

    bool Application::OnMouseButtonPressed(JFM::MouseButtonPressedEvent& e)
    {
        return false;
    }

    bool Application::OnMouseButtonReleased(JFM::MouseButtonReleasedEvent& e)
    {
        return false;
    }

    bool Application::OnMouseMoved(JFM::MouseMovedEvent& e)
    {
        return false;
    }

    bool Application::OnMouseScrolled(JFM::MouseScrolledEvent& e)
    {
        return false;
    }

    void Application::SetupEventCallbacks()
    {
        // GLFW回调函数 - 将事件投递到Core事件系统
        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            switch (action)
            {
                case GLFW_PRESS:
                {
                    auto event = std::make_unique<JFM::KeyPressedEvent>(key, 0);
                    JFM::EventSystem::GetInstance().PostEvent(std::move(event));
                    break;
                }
                case GLFW_RELEASE:
                {
                    auto event = std::make_unique<JFM::KeyReleasedEvent>(key);
                    JFM::EventSystem::GetInstance().PostEvent(std::move(event));
                    break;
                }
                case GLFW_REPEAT:
                {
                    auto event = std::make_unique<JFM::KeyPressedEvent>(key, 1);
                    JFM::EventSystem::GetInstance().PostEvent(std::move(event));
                    break;
                }
            }
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
        {
            switch (action)
            {
                case GLFW_PRESS:
                {
                    auto event = std::make_unique<JFM::MouseButtonPressedEvent>(button);
                    JFM::EventSystem::GetInstance().PostEvent(std::move(event));
                    break;
                }
                case GLFW_RELEASE:
                {
                    auto event = std::make_unique<JFM::MouseButtonReleasedEvent>(button);
                    JFM::EventSystem::GetInstance().PostEvent(std::move(event));
                    break;
                }
            }
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
        {
            auto event = std::make_unique<JFM::MouseMovedEvent>((float)xpos, (float)ypos);
            JFM::EventSystem::GetInstance().PostEvent(std::move(event));
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
        {
            auto event = std::make_unique<JFM::MouseScrolledEvent>((float)xoffset, (float)yoffset);
            JFM::EventSystem::GetInstance().PostEvent(std::move(event));
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
        {
            auto event = std::make_unique<JFM::WindowCloseEvent>();
            JFM::EventSystem::GetInstance().PostEvent(std::move(event));
        });

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
        {
            auto event = std::make_unique<JFM::WindowResizeEvent>(width, height);
            JFM::EventSystem::GetInstance().PostEvent(std::move(event));
        });

    }

}
