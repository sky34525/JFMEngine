#include "JFMEngine/Renderer/OpenGLTexture.h"
#include <stb_image.h>
#include <iostream>

namespace JFM {

    OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height) {

        m_Specification.Width = width;
        m_Specification.Height = height;
        m_Specification.Format = TextureFormat::RGBA8;

        m_InternalFormat = GL_RGBA8;
        m_DataFormat = GL_RGBA;

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        // 使用配置化的纹理参数设置，而不是硬编码
        SetupTextureParameters();

        glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, nullptr);

        m_IsLoaded = true;
    }

    OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
        : m_Path(path) {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(1);//在使用 stb_image 库加载图片时，将图片在垂直方向（Y 轴）进行翻转。

        stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);//这行代码使用 stb_image 库加载图片文件。

        if (!data) {
            std::cerr << "Failed to load texture: " << path << std::endl;
            return;
        }

        m_Width = width;
        m_Height = height;
    //根据图片的通道数（channels）设置 OpenGL 纹理的内部格式（internalFormat）和数据格式（dataFormat）
    //这样可以确保加载的图片数据与OpenGL纹理格式匹配，保证正确显示。
        GLenum internalFormat = 0, dataFormat = 0;
        if (channels == 4) {
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        } else if (channels == 3) {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        } else if (channels == 1) {
            internalFormat = GL_R8;
            dataFormat = GL_RED;
        }

        if (!(internalFormat & dataFormat)) {
            std::cerr << "Unsupported texture format with " << channels << " channels: " << path << std::endl;
            stbi_image_free(data);
            return;
        }

        m_InternalFormat = internalFormat;
        m_DataFormat = dataFormat;

        //存储纹理规格
        m_Specification.Width = width;
        m_Specification.Height = height;
        m_Specification.Format = channels == 4 ? TextureFormat::RGBA8 : (channels == 3 ? TextureFormat::RGB8 : TextureFormat::RED_INTEGER);

        std::cout << "Loaded texture: " << path << " (" << width << "x" << height << ", " << channels << " channels)" << std::endl;

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        SetupTextureParameters();

        glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, data);//将加载的图像数据上传到OpenGL纹理对象中。

        if (m_Specification.GenerateMips) {
            glGenerateMipmap(GL_TEXTURE_2D);//生成多级渐远纹理（mipmap），提升缩小时的质量。
        }

        stbi_image_free(data);
        m_IsLoaded = true;
    }

    OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification)
        : m_Specification(specification), m_Width(specification.Width), m_Height(specification.Height) {

        m_InternalFormat = TextureFormatToGL(m_Specification.Format);
        m_DataFormat = m_InternalFormat;

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFilterToGL(m_Specification.MinFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFilterToGL(m_Specification.MagFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrapToGL(m_Specification.WrapS));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrapToGL(m_Specification.WrapT));

        glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, nullptr);
    }

    OpenGLTexture2D::OpenGLTexture2D(const std::string& path, const TextureSpecification& specification)
        : m_Specification(specification), m_Path(path) {
        // 基本实现，使用默认构造然后加载
        *this = OpenGLTexture2D(path);
        m_Specification = specification;
    }

    OpenGLTexture2D::~OpenGLTexture2D() {
        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture2D::SetData(void* data, uint32_t size) {
        uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
        if (size != m_Width * m_Height * bpp) {
            std::cerr << "Data size mismatch! Expected: " << (m_Width * m_Height * bpp) << ", Got: " << size << std::endl;
        }
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    void OpenGLTexture2D::Unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void OpenGLTexture2D::SetupTextureParameters() {
        // 使用TextureSpecification中的参数设置纹理参数
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFilterToGL(m_Specification.MinFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFilterToGL(m_Specification.MagFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrapToGL(m_Specification.WrapS));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrapToGL(m_Specification.WrapT));
    }

    GLenum OpenGLTexture2D::TextureFormatToGL(TextureFormat format) {
        switch (format) {
            case TextureFormat::RGB8:        return GL_RGB8;
            case TextureFormat::RGBA8:       return GL_RGBA8;
            case TextureFormat::RED_INTEGER: return GL_R32I;
            case TextureFormat::None:        return GL_RGBA8; // 默认值
        }
        return GL_RGBA8;
    }

    GLenum OpenGLTexture2D::TextureWrapToGL(TextureWrap wrap) {
        switch (wrap) {
            case TextureWrap::Repeat:         return GL_REPEAT;
            case TextureWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
            case TextureWrap::ClampToEdge:    return GL_CLAMP_TO_EDGE;
            case TextureWrap::ClampToBorder:  return GL_CLAMP_TO_BORDER;
            case TextureWrap::None:           return GL_REPEAT; // 默认值
        }
        return GL_REPEAT;
    }

    GLenum OpenGLTexture2D::TextureFilterToGL(TextureFilter filter) {
        switch (filter) {
            case TextureFilter::Linear:                return GL_LINEAR;
            case TextureFilter::Nearest:               return GL_NEAREST;
            case TextureFilter::LinearMipmapLinear:    return GL_LINEAR_MIPMAP_LINEAR;
            case TextureFilter::LinearMipmapNearest:   return GL_LINEAR_MIPMAP_NEAREST;
            case TextureFilter::NearestMipmapLinear:   return GL_NEAREST_MIPMAP_LINEAR;
            case TextureFilter::NearestMipmapNearest:  return GL_NEAREST_MIPMAP_NEAREST;
            case TextureFilter::None:                  return GL_LINEAR; // 默认值
        }
        return GL_LINEAR;
    }

    std::shared_ptr<Texture2D> Texture2D::Create(uint32_t width, uint32_t height) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:      return nullptr;
            case RendererAPI::API::OpenGL:    return std::make_shared<OpenGLTexture2D>(width, height);
            case RendererAPI::API::Vulkan:    return nullptr;
            case RendererAPI::API::DirectX11: return nullptr;
            case RendererAPI::API::DirectX12: return nullptr;
        }
        return nullptr;
    }

    std::shared_ptr<Texture2D> Texture2D::Create(const std::string& path) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:      return nullptr;
            case RendererAPI::API::OpenGL:    return std::make_shared<OpenGLTexture2D>(path);
            case RendererAPI::API::Vulkan:    return nullptr;
            case RendererAPI::API::DirectX11: return nullptr;
            case RendererAPI::API::DirectX12: return nullptr;
        }
        return nullptr;
    }

    std::shared_ptr<Texture2D> Texture2D::Create(const TextureSpecification& specification) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:      return nullptr;
            case RendererAPI::API::OpenGL:    return std::make_shared<OpenGLTexture2D>(specification);
            case RendererAPI::API::Vulkan:    return nullptr;
            case RendererAPI::API::DirectX11: return nullptr;
            case RendererAPI::API::DirectX12: return nullptr;
        }
        return nullptr;
    }

    std::shared_ptr<Texture2D> Texture2D::Create(const std::string& path, const TextureSpecification& specification) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:      return nullptr;
            case RendererAPI::API::OpenGL:    return std::make_shared<OpenGLTexture2D>(path, specification);
            case RendererAPI::API::Vulkan:    return nullptr;
            case RendererAPI::API::DirectX11: return nullptr;
            case RendererAPI::API::DirectX12: return nullptr;
        }
        return nullptr;
    }

}
