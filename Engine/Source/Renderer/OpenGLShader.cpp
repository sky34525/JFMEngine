//
// OpenGLShader.cpp - OpenGL着色器具体实现
//

#include "JFMEngine/Renderer/OpenGLShader.h"
#include "JFMEngine/Utils/Log.h"
#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace JFM {

    static GLenum ShaderTypeFromString(const std::string& type) {
        if (type == "vertex")
            return GL_VERTEX_SHADER;
        if (type == "fragment" || type == "pixel")
            return GL_FRAGMENT_SHADER;

        JFM_CORE_ASSERT(false, "Unknown shader type!");
        return 0;
    }

    OpenGLShader::OpenGLShader(const std::string& filepath) {
        std::string source = ReadFile(filepath);// 读取着色器源码文件
        auto shaderSources = PreProcess(source);// 预处理着色器源码，提取不同类型的着色器代码
        Compile(shaderSources);

        // 从文件路径中提取名称
        auto lastSlash = filepath.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        auto lastDot = filepath.rfind('.');
        auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
        m_Name = filepath.substr(lastSlash, count);
    }

    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
        : m_Name(name) {
        std::unordered_map<GLenum, std::string> sources;
        sources[GL_VERTEX_SHADER] = vertexSrc;
        sources[GL_FRAGMENT_SHADER] = fragmentSrc;
        Compile(sources);
    }

    OpenGLShader::~OpenGLShader() {
        glDeleteProgram(m_RendererID);
    }

    std::string OpenGLShader::ReadFile(const std::string& filepath) {
        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary);// 打开文件以二进制模式读取
        if (in) {
            in.seekg(0, std::ios::end);//将文件读取指针移动到文件末尾
            size_t size = in.tellg();//返回文件指针当前位置，即文件大小
            if (size != -1) {
                result.resize(size);//提前分配足够的空间以避免多次内存分配
                in.seekg(0, std::ios::beg);//将文件指针重新定位到文件开头
                in.read(&result[0], size);// 从文件中读取数据到字符串
            }
        }
        return result;
    }

    std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source) {
        std::unordered_map<GLenum, std::string> shaderSources;

        const char* typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0);//从第0个字符开始查找typeToken，返回首字符在原字符串中的下标
        while (pos != std::string::npos) {
            size_t eol = source.find_first_of("\r\n", pos);//从pos开始查找第一个出现的换行符或回车符
            JFM_CORE_ASSERT(eol != std::string::npos, "Syntax error");
            size_t begin = pos + typeTokenLength + 1;
            std::string type = source.substr(begin, eol - begin);
            JFM_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

            size_t nextLinePos = source.find_first_not_of("\r\n", eol);
            JFM_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
            pos = source.find(typeToken, nextLinePos);

            shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);//将着色器源码存入map：判断是否读取到了最后一个着色器
            //如果判断为真则从nextLinePos开始读取到字符串末尾，否则从nextLinePos开始读取到下一个typeToken之前的内容
        }

        return shaderSources;
    }

    void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources) {
        GLuint program = glCreateProgram();
        JFM_CORE_ASSERT(shaderSources.size() <= 2, "We only support 2 shaders for now");
        std::array<GLenum, 2> glShaderIDs;//标准库中的定长数组
        int glShaderIDIndex = 0;
        for (auto& kv : shaderSources) {
            GLenum type = kv.first;
            const std::string& source = kv.second;

            GLuint shader = glCreateShader(type);

            const GLchar* sourceCStr = source.c_str();
            glShaderSource(shader, 1, &sourceCStr, 0);//将cpu中的着色器代码传递给gpu，后续的编译与链接都是由cpu发起在gpu中执行
            // shader:着色器对象ID
            // 1：源码字符串数量，这里只有一个源码字符串。
            // &sourceCStr：源码字符串的指针数组，这里传入源码的C字符串指针地址。
            // 0：每个字符串的长度数组，传0表示每个字符串都以\0结尾。

            glCompileShader(shader);// 编译着色器

            GLint isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);// 检查编译状态
            if (isCompiled == GL_FALSE) {
                GLint maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);// 获取编译错误日志的最大长度

                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);// 获取编译错误日志

                glDeleteShader(shader);// 删除编译失败的着色器

                JFM_CORE_ASSERT(false, "Shader compilation failure!");// 断言编译失败
                break;
            }

            glAttachShader(program, shader);// 将编译好的着色器附加到程序对象上
            glShaderIDs[glShaderIDIndex++] = shader;
        }

        m_RendererID = program;

        // Link our program
        glLinkProgram(program);

        // 请注意此处的不同函数：glGetProgram* 而不是 glGetShader*。
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);// 检查程序链接状态
        if (isLinked == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(program);

            for (auto id : glShaderIDs)
                glDeleteShader(id);

            JFM_CORE_ASSERT(false, "Shader link failure!");
            return;
        }

        for (auto id : glShaderIDs) {
            glDetachShader(program, id);//将着色器对象从程序对象中分离，如果直接删除着色器对象，不会立刻释放资源，而是等待所以程序都分离才会释放
            glDeleteShader(id);//删除着色器对象
        }
    }

    void OpenGLShader::Bind() const {
        glUseProgram(m_RendererID);
    }

    void OpenGLShader::Unbind() const {
        glUseProgram(0);
    }

    void OpenGLShader::SetInt(const std::string& name, int value) {
        UploadUniformInt(name, value);
    }

    void OpenGLShader::SetBool(const std::string& name, bool value) {
        UploadUniformInt(name, value ? 1 : 0);  // 布尔值转换为整数上传
    }

    void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count) {
        UploadUniformIntArray(name, values, count);
    }

    void OpenGLShader::SetFloat(const std::string& name, float value) {
        UploadUniformFloat(name, value);
    }

    void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value) {
        UploadUniformFloat2(name, value);
    }

    void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value) {
        UploadUniformFloat3(name, value);
    }

    void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value) {
        UploadUniformFloat4(name, value);
    }

    void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value) {
        UploadUniformMat4(name, value);
    }

    void OpenGLShader::SetMat3(const std::string& name, const glm::mat3& value) {
        UploadUniformMat3(name, value);
    }
    //用于在着色器中上传uniform变量的值
    void OpenGLShader::UploadUniformInt(const std::string& name, int value) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return;
        }
        glUniform1i(location, value);
    }
    //上传uniform数组
    void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return;
        }
        glUniform1iv(location, count, values);
    }
    //上传uniform浮点数
    void OpenGLShader::UploadUniformFloat(const std::string& name, float value) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return;
        }
        glUniform1f(location, value);
    }
    //上传uniform2维向量
    void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return;
        }
        glUniform2f(location, value.x, value.y);
    }

    void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return;
        }
        glUniform3f(location, value.x, value.y, value.z);
    }

    void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return;
        }
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }
    //上传uniform矩阵
    void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return;
        }
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return;
        }
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

}
