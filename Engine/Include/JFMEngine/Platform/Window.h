//
// Created by kw on 25-7-23.
// 窗口抽象接口 - 跨平台窗口系统
//

#ifndef WINDOW_H
#define WINDOW_H

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/Events/Event.h"
#include <string>
#include <functional>

namespace JFM {

    // 窗口属性结构
    struct WindowProps {
        std::string Title;
        unsigned int Width;
        unsigned int Height;
        bool VSync;//控制垂直同步

        WindowProps(const std::string& title = "JFM Engine",
                   unsigned int width = 1280,
                   unsigned int height = 720,
                   bool vsync = true)
            : Title(title), Width(width), Height(height), VSync(vsync) {}
    };

    // 窗口抽象基类
    class JFM_API Window {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        virtual ~Window() = default;

        // 窗口操作
        virtual void OnUpdate() = 0;
        virtual void SwapBuffers() = 0;

        // 窗口属性
        virtual unsigned int GetWidth() const = 0;
        virtual unsigned int GetHeight() const = 0;
        virtual const std::string& GetTitle() const = 0;

        // 窗口设置
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        // 窗口状态
        virtual bool ShouldClose() const = 0;
        virtual void Close() = 0;

        // 获取原生窗口句柄
        virtual void* GetNativeWindow() const = 0;

        // 静态创建方法
        static std::unique_ptr<Window> Create(const WindowProps& props = WindowProps());

    protected:
        Window() = default;
    };

}

#endif //WINDOW_H
