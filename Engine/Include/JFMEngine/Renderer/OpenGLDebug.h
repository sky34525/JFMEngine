//
// OpenGLDebug.h - OpenGL调试和错误检查工具
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include <glad/glad.h>
#include <string>

namespace JFM {

    class OpenGLDebug {
    public:
        // 获取OpenGL错误的字符串描述
        static std::string GetOpenGLErrorString(GLenum error);
        
        // 检查并输出当前所有OpenGL错误
        static bool CheckOpenGLErrors(const char* file, int line, const char* function = nullptr);
        
        // 清除所有OpenGL错误
        static void ClearOpenGLErrors();
        
        // 启用OpenGL调试输出（需要OpenGL 4.3+）
        static void EnableDebugOutput();
        
        // 禁用OpenGL调试输出
        static void DisableDebugOutput();
    };

    // 便利宏，用于更详细的错误检查
    #define JFM_GL_CHECK_ERROR() \
        JFM::OpenGLDebug::CheckOpenGLErrors(__FILE__, __LINE__, __FUNCTION__)

} // namespace JFM