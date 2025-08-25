//
// TextureConfig.cpp - 纹理配置管理系统实现
//

#include "JFMEngine/Renderer/TextureConfig.h"
#include "JFMEngine/Utils/Log.h"
#include <fstream>
#include <filesystem>

namespace JFM {

    // 预定义配置常量
    namespace TextureConfigs {
        const char* DEFAULT = "default";
        const char* PIXEL_ART = "pixel_art";
        const char* HIGH_QUALITY = "high_quality";
        const char* UI_TEXTURE = "ui_texture";
        const char* NORMAL_MAP = "normal_map";
        const char* HEIGHTMAP = "heightmap";
        const char* CUBEMAP = "cubemap";
        const char* SHADOW_MAP = "shadow_map";
    }

    bool TextureConfigManager::LoadConfigFromFile(const std::string& configPath) {
        if (!std::filesystem::exists(configPath)) {
            // 如果文件不存在，初始化默认配置并创建示例文件
            InitializeDefaultConfigs();
            SaveConfigToFile(configPath);
            return true;
        }

        try {
            std::ifstream file(configPath);
            if (!file.is_open()) {
                return false;
            }

            // 简单的键值对解析（可以扩展为JSON解析）
            std::string line;
            std::string currentConfig;
            TextureSpecification currentSpec;

            while (std::getline(file, line)) {
                // 跳过空行和注释
                if (line.empty() || line[0] == '#' || line[0] == '/') continue;

                // 解析配置块开头 [config_name]
                if (line[0] == '[' && line.back() == ']') {
                    // 保存之前的配置
                    if (!currentConfig.empty()) {
                        m_Configurations[currentConfig] = currentSpec;
                    }

                    currentConfig = line.substr(1, line.length() - 2);
                    currentSpec = TextureSpecification(); // 重置为默认值
                    continue;
                }

                // 解析键值对
                size_t equalPos = line.find('=');
                if (equalPos != std::string::npos) {
                    std::string key = line.substr(0, equalPos);
                    std::string value = line.substr(equalPos + 1);

                    // 去除空白字符
                    key.erase(0, key.find_first_not_of(" \t"));
                    key.erase(key.find_last_not_of(" \t") + 1);
                    value.erase(0, value.find_first_not_of(" \t"));
                    value.erase(value.find_last_not_of(" \t") + 1);

                    // 解析具体属性
                    if (key == "min_filter") {
                        currentSpec.MinFilter = ParseTextureFilter(value);
                    } else if (key == "mag_filter") {
                        currentSpec.MagFilter = ParseTextureFilter(value);
                    } else if (key == "wrap_s") {
                        currentSpec.WrapS = ParseTextureWrap(value);
                    } else if (key == "wrap_t") {
                        currentSpec.WrapT = ParseTextureWrap(value);
                    } else if (key == "generate_mips") {
                        currentSpec.GenerateMips = (value == "true" || value == "1");
                    }
                }
            }

            // 保存最后一个配置
            if (!currentConfig.empty()) {
                m_Configurations[currentConfig] = currentSpec;
            }

            file.close();
            return true;

        } catch (const std::exception& e) {
            return false;
        }
    }

    void TextureConfigManager::RegisterConfig(const std::string& name, const TextureSpecification& spec) {
        m_Configurations[name] = spec;
    }

    TextureSpecification TextureConfigManager::GetConfig(const std::string& name) const {
        auto it = m_Configurations.find(name);
        if (it != m_Configurations.end()) {
            return it->second;
        }

        // 返回默认配置
        return TextureSpecification();
    }

    bool TextureConfigManager::HasConfig(const std::string& name) const {
        return m_Configurations.find(name) != m_Configurations.end();
    }

    TextureSpecification TextureConfigManager::GetDefaultConfigForType(const std::string& extension) const {
        std::string ext = extension;
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") {
            return GetConfig(TextureConfigs::DEFAULT);
        } else if (ext == ".tga" || ext == ".bmp") {
            return GetConfig(TextureConfigs::PIXEL_ART);
        } else if (ext == ".hdr" || ext == ".exr") {
            return GetConfig(TextureConfigs::HIGH_QUALITY);
        }

        return TextureSpecification();
    }

    std::shared_ptr<Texture2D> TextureConfigManager::CreateTextureWithConfig(const std::string& path, const std::string& configName) {
        TextureSpecification spec;

        if (!configName.empty() && HasConfig(configName)) {
            spec = GetConfig(configName);
        } else {
            // 根据文件扩展名选择默认配置
            std::filesystem::path filePath(path);
            spec = GetDefaultConfigForType(filePath.extension().string());
        }

        return Texture2D::Create(path, spec);
    }

    void TextureConfigManager::Clear() {
        m_Configurations.clear();
    }

    bool TextureConfigManager::SaveConfigToFile(const std::string& configPath) const {
        try {
            // 确保目录存在
            std::filesystem::path filePath(configPath);
            if (filePath.has_parent_path()) {
                std::filesystem::create_directories(filePath.parent_path());
            }

            std::ofstream file(configPath);
            if (!file.is_open()) {
                return false;
            }

            file << "# JFM Engine Texture Configuration File\n";
            file << "# This file defines texture parameters for different use cases\n\n";

            for (const auto& [name, spec] : m_Configurations) {
                file << "[" << name << "]\n";
                file << "min_filter=" << TextureFilterToString(spec.MinFilter) << "\n";
                file << "mag_filter=" << TextureFilterToString(spec.MagFilter) << "\n";
                file << "wrap_s=" << TextureWrapToString(spec.WrapS) << "\n";
                file << "wrap_t=" << TextureWrapToString(spec.WrapT) << "\n";
                file << "generate_mips=" << (spec.GenerateMips ? "true" : "false") << "\n";
                file << "\n";
            }

            file.close();
            return true;

        } catch (const std::exception& e) {
            return false;
        }
    }

    void TextureConfigManager::InitializeDefaultConfigs() {
        // 默认配置
        TextureSpecification defaultSpec;
        defaultSpec.MinFilter = TextureFilter::Linear;
        defaultSpec.MagFilter = TextureFilter::Linear;
        defaultSpec.WrapS = TextureWrap::Repeat;
        defaultSpec.WrapT = TextureWrap::Repeat;
        defaultSpec.GenerateMips = true;
        RegisterConfig(TextureConfigs::DEFAULT, defaultSpec);

        // 像素艺术配置
        TextureSpecification pixelArtSpec;
        pixelArtSpec.MinFilter = TextureFilter::Nearest;
        pixelArtSpec.MagFilter = TextureFilter::Nearest;
        pixelArtSpec.WrapS = TextureWrap::ClampToEdge;
        pixelArtSpec.WrapT = TextureWrap::ClampToEdge;
        pixelArtSpec.GenerateMips = false;
        RegisterConfig(TextureConfigs::PIXEL_ART, pixelArtSpec);

        // 高质量配置
        TextureSpecification highQualitySpec;
        highQualitySpec.MinFilter = TextureFilter::LinearMipmapLinear;
        highQualitySpec.MagFilter = TextureFilter::Linear;
        highQualitySpec.WrapS = TextureWrap::Repeat;
        highQualitySpec.WrapT = TextureWrap::Repeat;
        highQualitySpec.GenerateMips = true;
        RegisterConfig(TextureConfigs::HIGH_QUALITY, highQualitySpec);

        // UI纹理配置
        TextureSpecification uiSpec;
        uiSpec.MinFilter = TextureFilter::Linear;
        uiSpec.MagFilter = TextureFilter::Linear;
        uiSpec.WrapS = TextureWrap::ClampToEdge;
        uiSpec.WrapT = TextureWrap::ClampToEdge;
        uiSpec.GenerateMips = false;
        RegisterConfig(TextureConfigs::UI_TEXTURE, uiSpec);

        // 法线贴图配置
        TextureSpecification normalMapSpec;
        normalMapSpec.MinFilter = TextureFilter::LinearMipmapLinear;
        normalMapSpec.MagFilter = TextureFilter::Linear;
        normalMapSpec.WrapS = TextureWrap::Repeat;
        normalMapSpec.WrapT = TextureWrap::Repeat;
        normalMapSpec.GenerateMips = true;
        RegisterConfig(TextureConfigs::NORMAL_MAP, normalMapSpec);

    }

    TextureFilter TextureConfigManager::ParseTextureFilter(const std::string& filterStr) const {
        if (filterStr == "linear") return TextureFilter::Linear;
        if (filterStr == "nearest") return TextureFilter::Nearest;
        if (filterStr == "linear_mipmap_linear") return TextureFilter::LinearMipmapLinear;
        if (filterStr == "linear_mipmap_nearest") return TextureFilter::LinearMipmapNearest;
        if (filterStr == "nearest_mipmap_linear") return TextureFilter::NearestMipmapLinear;
        if (filterStr == "nearest_mipmap_nearest") return TextureFilter::NearestMipmapNearest;
        return TextureFilter::Linear;
    }

    TextureWrap TextureConfigManager::ParseTextureWrap(const std::string& wrapStr) const {
        if (wrapStr == "repeat") return TextureWrap::Repeat;
        if (wrapStr == "mirrored_repeat") return TextureWrap::MirroredRepeat;
        if (wrapStr == "clamp_to_edge") return TextureWrap::ClampToEdge;
        if (wrapStr == "clamp_to_border") return TextureWrap::ClampToBorder;
        return TextureWrap::Repeat;
    }

    TextureFormat TextureConfigManager::ParseTextureFormat(const std::string& formatStr) const {
        if (formatStr == "rgb8") return TextureFormat::RGB8;
        if (formatStr == "rgba8") return TextureFormat::RGBA8;
        if (formatStr == "red_integer") return TextureFormat::RED_INTEGER;
        return TextureFormat::RGBA8;
    }

    std::string TextureConfigManager::TextureFilterToString(TextureFilter filter) const {
        switch (filter) {
            case TextureFilter::Linear: return "linear";
            case TextureFilter::Nearest: return "nearest";
            case TextureFilter::LinearMipmapLinear: return "linear_mipmap_linear";
            case TextureFilter::LinearMipmapNearest: return "linear_mipmap_nearest";
            case TextureFilter::NearestMipmapLinear: return "nearest_mipmap_linear";
            case TextureFilter::NearestMipmapNearest: return "nearest_mipmap_nearest";
            default: return "linear";
        }
    }

    std::string TextureConfigManager::TextureWrapToString(TextureWrap wrap) const {
        switch (wrap) {
            case TextureWrap::Repeat: return "repeat";
            case TextureWrap::MirroredRepeat: return "mirrored_repeat";
            case TextureWrap::ClampToEdge: return "clamp_to_edge";
            case TextureWrap::ClampToBorder: return "clamp_to_border";
            default: return "repeat";
        }
    }

    std::string TextureConfigManager::TextureFormatToString(TextureFormat format) const {
        switch (format) {
            case TextureFormat::RGB8: return "rgb8";
            case TextureFormat::RGBA8: return "rgba8";
            case TextureFormat::RED_INTEGER: return "red_integer";
            default: return "rgba8";
        }
    }

} // namespace JFM
