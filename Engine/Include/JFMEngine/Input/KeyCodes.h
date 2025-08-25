//
// KeyCodes.h - 键盘按键代码定义
// 定义所有键盘按键的常量值
//

#pragma once

#include "JFMEngine/Core/Core.h"

namespace JFM {

    // 键盘按键代码
    // 基于GLFW键码，但封装为引擎自己的定义
    namespace Key {
        enum : int {
            // 字母键
            A = 65, B = 66, C = 67, D = 68, E = 69, F = 70, G = 71,
            H = 72, I = 73, J = 74, K = 75, L = 76, M = 77, N = 78,
            O = 79, P = 80, Q = 81, R = 82, S = 83, T = 84, U = 85,
            V = 86, W = 87, X = 88, Y = 89, Z = 90,

            // 数字键
            D0 = 48, D1 = 49, D2 = 50, D3 = 51, D4 = 52,
            D5 = 53, D6 = 54, D7 = 55, D8 = 56, D9 = 57,

            // 功能键
            F1 = 290, F2 = 291, F3 = 292, F4 = 293, F5 = 294, F6 = 295,
            F7 = 296, F8 = 297, F9 = 298, F10 = 299, F11 = 300, F12 = 301,

            // 方向键
            UP = 265, DOWN = 264, LEFT = 263, RIGHT = 262,

            // 特殊键
            SPACE = 32,
            ENTER = 257,
            TAB = 258,
            BACKSPACE = 259,
            DELETE_KEY = 261,
            INSERT = 260,

            // 修饰键
            LEFT_SHIFT = 340,
            RIGHT_SHIFT = 344,
            LEFT_CONTROL = 341,
            RIGHT_CONTROL = 345,
            LEFT_ALT = 342,
            RIGHT_ALT = 346,
            LEFT_SUPER = 343,
            RIGHT_SUPER = 347,

            ESCAPE = 256,
            PAGE_UP = 266,
            PAGE_DOWN = 267,
            HOME = 268,
            END = 269,
            CAPS_LOCK = 280,
            NUM_LOCK = 282,
            SCROLL_LOCK = 281,

            // 小键盘
            KP_0 = 320, KP_1 = 321, KP_2 = 322, KP_3 = 323, KP_4 = 324,
            KP_5 = 325, KP_6 = 326, KP_7 = 327, KP_8 = 328, KP_9 = 329,
            KP_DECIMAL = 330,
            KP_DIVIDE = 331,
            KP_MULTIPLY = 332,
            KP_SUBTRACT = 333,
            KP_ADD = 334,
            KP_ENTER = 335,
            KP_EQUAL = 336,

            // 标点符号
            SEMICOLON = 59,
            EQUAL = 61,
            COMMA = 44,
            MINUS = 45,
            PERIOD = 46,
            SLASH = 47,
            GRAVE_ACCENT = 96,
            LEFT_BRACKET = 91,
            BACKSLASH = 92,
            RIGHT_BRACKET = 93,
            APOSTROPHE = 39
        };
    }

    // 鼠标按键代码
    namespace Mouse {
        enum : int {
            BUTTON_1 = 0,
            BUTTON_2 = 1,
            BUTTON_3 = 2,
            BUTTON_4 = 3,
            BUTTON_5 = 4,
            BUTTON_6 = 5,
            BUTTON_7 = 6,
            BUTTON_8 = 7,

            // 常用别名
            LEFT = BUTTON_1,
            RIGHT = BUTTON_2,
            MIDDLE = BUTTON_3
        };
    }

}
