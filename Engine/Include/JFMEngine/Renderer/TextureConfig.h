//
// TextureConfig.h - 纹理配置管理系统
// 支持从外部文件加载纹理参数配置
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "Texture.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace JFM {

    // 纹理配置管理器
    class JFM_API TextureConfigManager {
    public:
        static TextureConfigManager& GetInstance() {
            static TextureConfigManager instance;
            return instance;
        }

        // 从JSON文件加载纹理配置
        bool LoadConfigFromFile(const std::string& configPath);

        // 从代码中注册纹理配置
        void RegisterConfig(const std::string& name, const TextureSpecification& spec);

        // 获取纹理配置
        TextureSpecification GetConfig(const std::string& name) const;

        // 检查配置是否存在
        bool HasConfig(const std::string& name) const;

        // 根据文件扩展名获取默认配置
        TextureSpecification GetDefaultConfigForType(const std::string& extension) const;

        // 创建带配置的纹理
        std::shared_ptr<Texture2D> CreateTextureWithConfig(const std::string& path, const std::string& configName = "");

        // 清除所有配置
        void Clear();

        // 保存当前配置到文件
        bool SaveConfigToFile(const std::string& configPath) const;

    private:
        std::unordered_map<std::string, TextureSpecification> m_Configurations;

        // 初始化默认配置
        void InitializeDefaultConfigs();

        // 解析纹理过滤器字符串
        TextureFilter ParseTextureFilter(const std::string& filterStr) const;

        // 解析纹理包裹方式字符串
        TextureWrap ParseTextureWrap(const std::string& wrapStr) const;

        // 解析纹理格式字符串
        TextureFormat ParseTextureFormat(const std::string& formatStr) const;

        // 转换为字符串（用于保存配置）
        std::string TextureFilterToString(TextureFilter filter) const;
        std::string TextureWrapToString(TextureWrap wrap) const;
        std::string TextureFormatToString(TextureFormat format) const;
    };

    // 预定义的纹理配置常量
    namespace TextureConfigs {
        // 标准配置
        extern JFM_API const char* DEFAULT;
        extern JFM_API const char* PIXEL_ART;      // 像素艺术纹理
        extern JFM_API const char* HIGH_QUALITY;   // 高质量纹理
        extern JFM_API const char* UI_TEXTURE;     // UI纹理
        extern JFM_API const char* NORMAL_MAP;     // 法线贴图
        extern JFM_API const char* HEIGHTMAP;      // 高度贴图
        extern JFM_API const char* CUBEMAP;        // 立方体贴图
        extern JFM_API const char* SHADOW_MAP;     // 阴影贴图
    }

} // namespace JFM
