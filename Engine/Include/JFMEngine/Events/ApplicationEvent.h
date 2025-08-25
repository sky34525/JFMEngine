//
// Created by kw on 25-7-16.
//

#ifndef APPLICATIONEVENT_H
#define APPLICATIONEVENT_H

#include "Event.h"
#include <sstream>

namespace JFM {

    // 窗口调整大小事件
    class JFM_API WindowResizeEvent : public Event {
    public:
        WindowResizeEvent(unsigned int width, unsigned int height)
            : m_Width(width), m_Height(height) {}

        inline unsigned int GetWidth() const { return m_Width; }
        inline unsigned int GetHeight() const { return m_Height; }

        std::string ToString() const override {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
        unsigned int m_Width, m_Height;
    };

    // 窗口关闭事件
    class JFM_API WindowCloseEvent : public Event {
    public:
        WindowCloseEvent() {}

        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    // 应用程序更新事件
    class JFM_API AppTickEvent : public Event {
    public:
        AppTickEvent() {}

        EVENT_CLASS_TYPE(AppTick)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    // 应用程序渲染事件
    class JFM_API AppUpdateEvent : public Event {
    public:
        AppUpdateEvent() {}

        EVENT_CLASS_TYPE(AppUpdate)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    // 应用程序渲染事件
    class JFM_API AppRenderEvent : public Event {
    public:
        AppRenderEvent() {}

        EVENT_CLASS_TYPE(AppRender)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

} // namespace JFM

#endif //APPLICATIONEVENT_H
