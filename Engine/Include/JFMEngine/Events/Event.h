//
// Created by kw on 25-7-16.
//

#ifndef EVENT_H
#define EVENT_H

#include "JFMEngine/Core/Core.h"
#include <string>
#include <functional>
#include <ostream>

namespace JFM {

    // 事件类型枚举
    enum class EventType {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        AppTick, AppUpdate, AppRender,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    // 事件分类，用于过滤
    enum EventCategory {
        None = 0,
        EventCategoryApplication    = 1 << 0,
        EventCategoryInput          = 1 << 1,
        EventCategoryKeyboard       = 1 << 2,
        EventCategoryMouse          = 1 << 3,
        EventCategoryMouseButton    = 1 << 4
    };

    // 事件基类
    //将成员定义成函数而不是数据成员，避免了内存占用和类型安全问题。
    //这种设计体现了"接口与实现分离"的原则，基类专注于定义契约，具体实现由子类负责，既保证了类型安全，又优化了内存使用。
    class JFM_API Event {
        friend class EventDispatcher;
    public:
        virtual ~Event() = default;

        // 纯虚函数，子类必须实现
        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        virtual int GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetName(); }

        // 检查事件是否属于某个分类
        inline bool IsInCategory(EventCategory category) {
            return GetCategoryFlags() & category;
        }

        bool Handled = false;
    };

    // 事件分发器
    class EventDispatcher {
        template<typename T>
        using EventFn = std::function<bool(T&)>;

    public:
        EventDispatcher(Event& event)
            : m_Event(event) {}

        // 分发事件到对应的处理函数
        template<typename T>
        bool Dispatch(EventFn<T> func) {
            if (m_Event.GetEventType() == T::GetStaticType()) {//T::GetStaticType()是一个静态成员函数，用于获取事件类型
                //使用GetStaticType()来获取事件类型,可以避免T的实例化，从而提高性能和类型安全。
                m_Event.Handled = func(*(T*)&m_Event);//C风格强制转换，将 m_Event 的地址解释为类型 T 的地址，并解引用以获得具体事件对象。
                //func(*(T*)&m_Event) 调用了事件处理函数 func，并将转换后的具体事件对象作为参数传递。func 是一个 std::function<bool(T&)> 类型的函数对象，表示一个接受事件对象并返回布尔值的函数。
                return true;
            }
            return false;
        }

    private:
        Event& m_Event;
    };

    // 输出流操作符重载
    inline std::ostream& operator<<(std::ostream& os, const Event& e) {
        return os << e.ToString();
    }

}

// 宏定义，用于简化事件类的实现
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
                               virtual EventType GetEventType() const override { return GetStaticType(); }\
                               virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

#endif //EVENT_H
