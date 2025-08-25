//
// Created by kw on 25-7-16.
//

#ifndef CORE_H
#define CORE_H

// 前置声明和包含
#ifdef JFM_DEBUG
    // 在Debug模式下需要OpenGL和日志系统
    #include <glad/glad.h>
    #include "JFMEngine/Utils/Log.h"
#endif

// 跨平台 API 导出宏定义
#ifdef _WIN32
    #ifdef JFM_BUILD_DLL
        #define JFM_API __declspec(dllexport)
    #else
        #define JFM_API __declspec(dllimport)
    #endif
#else
    // Linux/macOS 平台
    #ifdef JFM_BUILD_DLL
        #define JFM_API __attribute__((visibility("default")))
    #else
        #define JFM_API
    #endif
#endif

// 断言宏定义
#ifdef JFM_DEBUG
    #define JFM_ENABLE_ASSERTS
#endif

#ifdef JFM_ENABLE_ASSERTS
    #define JFM_ASSERT(x, msg) { if(!(x)) { __debugbreak(); } }
    #define JFM_CORE_ASSERT(x, msg) { if(!(x)) { __debugbreak(); } }
#else
    #define JFM_ASSERT(x, msg)
    #define JFM_CORE_ASSERT(x, msg)
#endif

// 调试断点宏（跨平台）
#ifdef _WIN32
    #define __debugbreak() __debugbreak()
#elif defined(__APPLE__)
    #define __debugbreak() __builtin_trap()
#else
    #define __debugbreak() __builtin_trap()
#endif

// OpenGL错误检查宏
#ifdef JFM_DEBUG
    #define JFM_GL_ERROR_CHECK() \
        do { \
            GLenum error = glGetError(); \
            if (error != GL_NO_ERROR) { \
                JFM_CORE_ERROR("OpenGL错误 0x{:x} 在文件 {} 第 {} 行", error, __FILE__, __LINE__); \
                __debugbreak(); \
            } \
        } while(0)
    
    #define JFM_GL_CALL(func) \
        do { \
            func; \
            JFM_GL_ERROR_CHECK(); \
        } while(0)
#else
    #define JFM_GL_ERROR_CHECK()
    #define JFM_GL_CALL(func) func
#endif

#endif //CORE_H
