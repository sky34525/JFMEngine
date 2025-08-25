//
// Texture.h - 纹理系统
// 支持2D纹理加载、绑定和管理
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include <string>
#include <memory>
#include <unordered_map>

namespace JFM {

    //纹理像素格式类型
    enum class TextureFormat {
        None = 0,
        RGB8,//8位每通道的RGB
        RGBA8,//8位每通道的RGBA
        RED_INTEGER//红色整数
    };
    //表示纹理包裹方式
 enum class TextureWrap {
     None = 0,         // 无包裹
     Repeat,           // 重复包裹
     MirroredRepeat,   // 镜像重复
     ClampToEdge,      // 边缘夹取
     ClampToBorder     // 边框夹取
 };
    //纹理过滤方式
    enum class TextureFilter {
        None = 0,
        Linear,//线性插值采样
        Nearest,//最近点采样
        LinearMipmapLinear,//线性插值并结合多级渐远纹理（mipmap），提升缩小时的质量。
        LinearMipmapNearest,//线性插值 + 最近点 mipmap。
        NearestMipmapLinear,// 最近点采样 + 线性 mipmap。
        NearestMipmapNearest// 最近点采样 + 最近点 mipmap。
    };
    // 纹理规格结构体
    struct TextureSpecification {
        uint32_t Width = 1;
        uint32_t Height = 1;
        TextureFormat Format = TextureFormat::RGBA8;
        TextureWrap WrapS = TextureWrap::Repeat;
        TextureWrap WrapT = TextureWrap::Repeat;
        TextureFilter MinFilter = TextureFilter::Linear;
        TextureFilter MagFilter = TextureFilter::Linear;
        bool GenerateMips = true;
    };

    class JFM_API Texture {
    protected:
        Texture(){};
    public:
        virtual ~Texture() = default;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual uint32_t GetRendererID() const = 0;
        virtual const std::string& GetPath() const = 0;

        virtual void SetData(void* data, uint32_t size) = 0;
        virtual void Bind(uint32_t slot = 0) const = 0;
        virtual void Unbind() const = 0;

        virtual bool IsLoaded() const = 0;// 检查纹理是否已加载
        virtual bool operator==(const Texture& other) const = 0;

        // 添加纹理类型支持
        virtual void SetType(const std::string& type) = 0;
        virtual const std::string& GetType() const = 0;
    };

    class JFM_API Texture2D : public Texture {
    public:
        static std::shared_ptr<Texture2D> Create(uint32_t width, uint32_t height);
        static std::shared_ptr<Texture2D> Create(const std::string& path);
        static std::shared_ptr<Texture2D> Create(const TextureSpecification& specification);
        static std::shared_ptr<Texture2D> Create(const std::string& path, const TextureSpecification& specification);
    };

    class JFM_API TextureLibrary {
    public:
        void Add(const std::string& name, const std::shared_ptr<Texture2D>& texture);
        void Add(const std::shared_ptr<Texture2D>& texture);
        std::shared_ptr<Texture2D> Load(const std::string& path);
        std::shared_ptr<Texture2D> Load(const std::string& name, const std::string& path);

        std::shared_ptr<Texture2D> Get(const std::string& name);
        bool Exists(const std::string& name) const;

        void Clear();
        size_t GetCount() const { return m_Textures.size(); }

        // 获取全局纹理库实例
        static TextureLibrary& GetInstance() {
            static TextureLibrary instance;
            return instance;
        }

    private:
        std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_Textures;

        std::string ExtractName(const std::string& filepath);
    };

}
