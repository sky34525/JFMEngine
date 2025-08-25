//
// OpenGLDebug.cpp - OpenGL调试和错误检查工具实现
//

#include "JFMEngine/Renderer/OpenGLDebug.h"
#include "JFMEngine/Utils/Log.h"
#include <GLFW/glfw3.h>
#include <iostream>

// OpenGL调试相关常量定义（如果系统没有提供）
#ifndef GL_DEBUG_OUTPUT
#define GL_DEBUG_OUTPUT 0x92E0
#endif
#ifndef GL_DEBUG_OUTPUT_SYNCHRONOUS
#define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242
#endif
#ifndef GL_DEBUG_SEVERITY_NOTIFICATION
#define GL_DEBUG_SEVERITY_NOTIFICATION 0x826B
#endif
#ifndef GL_DEBUG_SOURCE_API
#define GL_DEBUG_SOURCE_API 0x8246
#endif
#ifndef GL_DEBUG_SOURCE_WINDOW_SYSTEM
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM 0x8247
#endif
#ifndef GL_DEBUG_SOURCE_SHADER_COMPILER
#define GL_DEBUG_SOURCE_SHADER_COMPILER 0x8248
#endif
#ifndef GL_DEBUG_SOURCE_THIRD_PARTY
#define GL_DEBUG_SOURCE_THIRD_PARTY 0x8249
#endif
#ifndef GL_DEBUG_SOURCE_APPLICATION
#define GL_DEBUG_SOURCE_APPLICATION 0x824A
#endif
#ifndef GL_DEBUG_SOURCE_OTHER
#define GL_DEBUG_SOURCE_OTHER 0x824B
#endif
#ifndef GL_DEBUG_TYPE_ERROR
#define GL_DEBUG_TYPE_ERROR 0x824C
#endif
#ifndef GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#endif
#ifndef GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 0x824E
#endif
#ifndef GL_DEBUG_TYPE_PORTABILITY
#define GL_DEBUG_TYPE_PORTABILITY 0x824F
#endif
#ifndef GL_DEBUG_TYPE_PERFORMANCE
#define GL_DEBUG_TYPE_PERFORMANCE 0x8250
#endif
#ifndef GL_DEBUG_TYPE_MARKER
#define GL_DEBUG_TYPE_MARKER 0x8268
#endif
#ifndef GL_DEBUG_TYPE_OTHER
#define GL_DEBUG_TYPE_OTHER 0x8251
#endif
#ifndef GL_DEBUG_SEVERITY_HIGH
#define GL_DEBUG_SEVERITY_HIGH 0x9146
#endif
#ifndef GL_DEBUG_SEVERITY_MEDIUM
#define GL_DEBUG_SEVERITY_MEDIUM 0x9147
#endif
#ifndef GL_DEBUG_SEVERITY_LOW
#define GL_DEBUG_SEVERITY_LOW 0x9148
#endif

// 声明 glDebugMessageCallback 函数类型和函数指针（如果系统没有提供）
typedef void (APIENTRY *GLDEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
typedef void (APIENTRY *PFNGLDEBUGMESSAGECALLBACKPROC)(GLDEBUGPROC callback, const void* userParam);

// 动态获取函数指针
static PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback = nullptr;

namespace JFM {

    std::string OpenGLDebug::GetOpenGLErrorString(GLenum error) {
        switch (error) {
            case GL_NO_ERROR:          return "GL_NO_ERROR";
            case GL_INVALID_ENUM:      return "GL_INVALID_ENUM";
            case GL_INVALID_VALUE:     return "GL_INVALID_VALUE";
            case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
            case GL_OUT_OF_MEMORY:     return "GL_OUT_OF_MEMORY";
            case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
            default: return "未知OpenGL错误";
        }
    }

    bool OpenGLDebug::CheckOpenGLErrors(const char* file, int line, const char* function) {
        GLenum error;
        bool hasError = false;
        
        while ((error = glGetError()) != GL_NO_ERROR) {
            hasError = true;
            std::string errorString = GetOpenGLErrorString(error);
            
            if (function) {
                JFM_CORE_ERROR("OpenGL错误: {} (0x{:x}) 在 {}:{} 函数: {}", 
                              errorString, error, file, line, function);
            } else {
                JFM_CORE_ERROR("OpenGL错误: {} (0x{:x}) 在 {}:{}", 
                              errorString, error, file, line);
            }
        }
        
        return hasError;
    }

    void OpenGLDebug::ClearOpenGLErrors() {
        while (glGetError() != GL_NO_ERROR);
    }

    void OpenGLDebug::EnableDebugOutput() {
        // 动态获取 glDebugMessageCallback 函数指针
        if (!glDebugMessageCallback) {
            glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)glfwGetProcAddress("glDebugMessageCallback");
        }
        
        // 检查是否支持调试输出
        if (glDebugMessageCallback) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            
            // 设置调试回调
            glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity,
                                    GLsizei length, const GLchar* message, const void* userParam) {
                // 忽略通知级别的消息
                if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
                
                std::string sourceStr, typeStr, severityStr;
                
                // 解析消息来源
                switch (source) {
                    case GL_DEBUG_SOURCE_API:             sourceStr = "API"; break;
                    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   sourceStr = "Window System"; break;
                    case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "Shader Compiler"; break;
                    case GL_DEBUG_SOURCE_THIRD_PARTY:     sourceStr = "Third Party"; break;
                    case GL_DEBUG_SOURCE_APPLICATION:     sourceStr = "Application"; break;
                    case GL_DEBUG_SOURCE_OTHER:           sourceStr = "Other"; break;
                    default:                              sourceStr = "Unknown"; break;
                }
                
                // 解析消息类型
                switch (type) {
                    case GL_DEBUG_TYPE_ERROR:               typeStr = "Error"; break;
                    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "Deprecated Behavior"; break;
                    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  typeStr = "Undefined Behavior"; break;
                    case GL_DEBUG_TYPE_PORTABILITY:         typeStr = "Portability"; break;
                    case GL_DEBUG_TYPE_PERFORMANCE:         typeStr = "Performance"; break;
                    case GL_DEBUG_TYPE_MARKER:              typeStr = "Marker"; break;
                    case GL_DEBUG_TYPE_OTHER:               typeStr = "Other"; break;
                    default:                                typeStr = "Unknown"; break;
                }
                
                // 解析严重程度
                switch (severity) {
                    case GL_DEBUG_SEVERITY_HIGH:   severityStr = "HIGH"; break;
                    case GL_DEBUG_SEVERITY_MEDIUM: severityStr = "MEDIUM"; break;
                    case GL_DEBUG_SEVERITY_LOW:    severityStr = "LOW"; break;
                    default:                       severityStr = "UNKNOWN"; break;
                }
                
                // 输出调试消息
                if (severity == GL_DEBUG_SEVERITY_HIGH) {
                    JFM_CORE_ERROR("OpenGL调试 [{}] {} ({}): {}", sourceStr, typeStr, severityStr, message);
                } else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
                    JFM_CORE_WARN("OpenGL调试 [{}] {} ({}): {}", sourceStr, typeStr, severityStr, message);
                } else {
                    JFM_CORE_INFO("OpenGL调试 [{}] {} ({}): {}", sourceStr, typeStr, severityStr, message);
                }
            }, nullptr);
            
            JFM_CORE_INFO("OpenGL调试输出已启用");
        } else {
            JFM_CORE_WARN("当前OpenGL上下文不支持调试输出");
        }
    }

    void OpenGLDebug::DisableDebugOutput() {
        if (glDebugMessageCallback) {
            glDisable(GL_DEBUG_OUTPUT);
            glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            JFM_CORE_INFO("OpenGL调试输出已禁用");
        }
    }

} // namespace JFM