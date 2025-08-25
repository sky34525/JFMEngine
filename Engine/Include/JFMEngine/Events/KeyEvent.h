//
// Created by kw on 25-7-16.
//

#ifndef KEYEVENT_H
#define KEYEVENT_H

#include "Event.h"
#include <sstream>

namespace JFM {

    // 键盘事件基类
    class JFM_API KeyEvent : public Event {
    public:
        inline int GetKeyCode() const { return m_KeyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

    protected:
        KeyEvent(int keycode)
            : m_KeyCode(keycode) {}

        int m_KeyCode;
    };

    // 按键按下事件
    class JFM_API KeyPressedEvent : public KeyEvent {
    public:
        KeyPressedEvent(int keycode, int repeatCount)
            : KeyEvent(keycode), m_RepeatCount(repeatCount) {}

        inline int GetRepeatCount() const { return m_RepeatCount; }

        std::string ToString() const override {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPressed)

    private:
        int m_RepeatCount;
    };

    // 按键释放事件
    class JFM_API KeyReleasedEvent : public KeyEvent {
    public:
        KeyReleasedEvent(int keycode)
            : KeyEvent(keycode) {}

        std::string ToString() const override {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << m_KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    // 字符输入事件
    class JFM_API KeyTypedEvent : public KeyEvent {
    public:
        KeyTypedEvent(int keycode)
            : KeyEvent(keycode) {}

        std::string ToString() const override {
            std::stringstream ss;
            ss << "KeyTypedEvent: " << m_KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyTyped)
    };

}

#endif //KEYEVENT_H
