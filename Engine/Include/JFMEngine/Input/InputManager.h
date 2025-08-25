//
// InputManager.h - 增强的输入管理系统
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <functional>
#include <vector>

namespace JFM {

    // 输入设备类型
    enum class InputDevice {
        KEYBOARD,
        MOUSE,
        GAMEPAD
    };

    // 输入动作类型
    enum class InputAction {
        PRESSED,
        RELEASED,
        HELD
    };

    // 游戏手柄按钮
    enum class GamepadButton {
        A, B, X, Y,
        LEFT_BUMPER, RIGHT_BUMPER,
        LEFT_TRIGGER, RIGHT_TRIGGER,
        BACK, START,
        LEFT_STICK, RIGHT_STICK,
        DPAD_UP, DPAD_DOWN, DPAD_LEFT, DPAD_RIGHT
    };

    // 输入绑定
    struct InputBinding {
        std::string ActionName;
        InputDevice Device;
        int KeyCode;
        InputAction Action;
        std::function<void()> Callback;
    };

    // 轴绑定（用于模拟输入）
    struct AxisBinding {
        std::string AxisName;
        int PositiveKey;
        int NegativeKey;
        float Sensitivity = 1.0f;
        float DeadZone = 0.1f;
    };

    class JFM_API InputManager {
    public:
        static InputManager& GetInstance() {
            static InputManager instance;
            return instance;
        }

        void Initialize();
        void Update();

        // 键盘输入
        bool IsKeyPressed(int keycode) const;
        bool IsKeyReleased(int keycode) const;
        bool IsKeyHeld(int keycode) const;

        // 鼠标输入
        bool IsMouseButtonPressed(int button) const;
        bool IsMouseButtonReleased(int button) const;
        bool IsMouseButtonHeld(int button) const;

        glm::vec2 GetMousePosition() const { return m_MousePosition; }
        glm::vec2 GetMouseDelta() const { return m_MouseDelta; }
        float GetMouseWheelDelta() const { return m_MouseWheelDelta; }

        void SetMousePosition(const glm::vec2& position);
        void SetCursorMode(bool visible, bool confined = false);

        // 游戏手柄输入
        bool IsGamepadConnected(int gamepadId = 0) const;
        bool IsGamepadButtonPressed(GamepadButton button, int gamepadId = 0) const;
        bool IsGamepadButtonReleased(GamepadButton button, int gamepadId = 0) const;
        bool IsGamepadButtonHeld(GamepadButton button, int gamepadId = 0) const;

        float GetGamepadAxis(int axis, int gamepadId = 0) const;
        glm::vec2 GetGamepadStick(bool leftStick = true, int gamepadId = 0) const;

        // 动作绑定系统
        void BindAction(const std::string& actionName, InputDevice device, int keyCode,
                       InputAction action, std::function<void()> callback);
        void UnbindAction(const std::string& actionName);
        void TriggerAction(const std::string& actionName);

        // 轴绑定系统
        void BindAxis(const std::string& axisName, int positiveKey, int negativeKey,
                     float sensitivity = 1.0f);
        float GetAxis(const std::string& axisName) const;

        // 输入配置
        void LoadInputConfig(const std::string& configFile);
        void SaveInputConfig(const std::string& configFile);

        // 事件回调
        void SetKeyCallback(std::function<void(int, int, int, int)> callback) { m_KeyCallback = callback; }
        void SetMouseCallback(std::function<void(int, int, int)> callback) { m_MouseCallback = callback; }
        void SetScrollCallback(std::function<void(double, double)> callback) { m_ScrollCallback = callback; }

    private:
        // 键盘状态
        std::unordered_map<int, bool> m_KeyStates;
        std::unordered_map<int, bool> m_PrevKeyStates;

        // 鼠标状态
        std::unordered_map<int, bool> m_MouseButtonStates;
        std::unordered_map<int, bool> m_PrevMouseButtonStates;
        glm::vec2 m_MousePosition = glm::vec2(0.0f);
        glm::vec2 m_PrevMousePosition = glm::vec2(0.0f);
        glm::vec2 m_MouseDelta = glm::vec2(0.0f);
        float m_MouseWheelDelta = 0.0f;

        // 游戏手柄状态
        struct GamepadState {
            bool Connected = false;
            std::unordered_map<GamepadButton, bool> ButtonStates;
            std::unordered_map<GamepadButton, bool> PrevButtonStates;
            std::vector<float> AxisValues;
        };
        std::vector<GamepadState> m_GamepadStates;

        // 绑定系统
        std::unordered_map<std::string, InputBinding> m_ActionBindings;
        std::unordered_map<std::string, AxisBinding> m_AxisBindings;

        // 事件回调
        std::function<void(int, int, int, int)> m_KeyCallback;
        std::function<void(int, int, int)> m_MouseCallback;
        std::function<void(double, double)> m_ScrollCallback;

        void UpdateKeyboardState();
        void UpdateMouseState();
        void UpdateGamepadState();
        void ProcessInputBindings();
    };

    // 便利的输入宏
    #define INPUT_MANAGER InputManager::GetInstance()
    #define IS_KEY_PRESSED(key) INPUT_MANAGER.IsKeyPressed(key)
    #define IS_KEY_HELD(key) INPUT_MANAGER.IsKeyHeld(key)
    #define IS_MOUSE_PRESSED(button) INPUT_MANAGER.IsMouseButtonPressed(button)
    #define GET_MOUSE_POS() INPUT_MANAGER.GetMousePosition()
    #define GET_AXIS(name) INPUT_MANAGER.GetAxis(name)

}
