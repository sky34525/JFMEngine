//
// OpenGLTexture.h - OpenGL纹理实现
//

#pragma once

#include "JFMEngine/Renderer/Texture.h"
#include "JFMEngine/Renderer/RendererAPI.h"
#include <glad/glad.h>

namespace JFM {

    class OpenGLTexture2D : public Texture2D {
    public:
        OpenGLTexture2D(uint32_t width, uint32_t height);
        OpenGLTexture2D(const std::string& path);
        OpenGLTexture2D(const TextureSpecification& specification);
        OpenGLTexture2D(const std::string& path, const TextureSpecification& specification);
        virtual ~OpenGLTexture2D();

        virtual uint32_t GetWidth() const override { return m_Width; }
        virtual uint32_t GetHeight() const override { return m_Height; }
        virtual uint32_t GetRendererID() const override { return m_RendererID; }
        virtual const std::string& GetPath() const override { return m_Path; }

        virtual void SetData(void* data, uint32_t size) override;
        virtual void Bind(uint32_t slot = 0) const override;
        virtual void Unbind() const override;

        virtual bool IsLoaded() const override { return m_IsLoaded; }
        virtual bool operator==(const Texture& other) const override {
            return m_RendererID == other.GetRendererID();
        }

        // 添加纹理类型支持
        virtual void SetType(const std::string& type) override { m_Type = type; }
        virtual const std::string& GetType() const override { return m_Type; }

        const TextureSpecification& GetSpecification() const { return m_Specification; }

    private:
        TextureSpecification m_Specification;
        std::string m_Path;
        std::string m_Type = ""; // 纹理类型（如texture_diffuse, texture_specular等）
        bool m_IsLoaded = false;
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID;
        GLenum m_InternalFormat, m_DataFormat;

        void LoadFromFile(const std::string& path);
        void CreateTexture();
        void SetupTextureParameters();

        static GLenum TextureFormatToGL(TextureFormat format);
        static GLenum TextureWrapToGL(TextureWrap wrap);
        static GLenum TextureFilterToGL(TextureFilter filter);
    };

}
