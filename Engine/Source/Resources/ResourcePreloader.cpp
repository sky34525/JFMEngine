//
// ResourcePreloader.cpp - 资源预加载器实现
//

#include "JFMEngine/Resources/ResourcePreloader.h"
#include "JFMEngine/Resources/ResourceLoaders.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <thread>
#include <algorithm>
#include <iostream>

// 临时日志宏定义，直到Logger系统完善
#define JFM_CORE_INFO(fmt, ...) std::cout << "[INFO] " << fmt << std::endl
#define JFM_CORE_ERROR(fmt, ...) std::cerr << "[ERROR] " << fmt << std::endl
#define JFM_CORE_WARN(fmt, ...) std::cout << "[WARN] " << fmt << std::endl

namespace JFM {

    bool ResourcePreloader::LoadManifest(const std::string& manifestPath) {
        std::ifstream file(manifestPath);
        if (!file.is_open()) {
            std::cerr << "[ERROR] Failed to open resource manifest: " << manifestPath << std::endl;
            return false;
        }

        std::string jsonContent((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
        file.close();

        return ParseManifestJSON(jsonContent);
    }

    bool ResourcePreloader::ParseManifestJSON(const std::string& jsonContent) {
        try {
            nlohmann::json manifest = nlohmann::json::parse(jsonContent);

            if (!manifest.contains("resourceGroups")) {
                std::cerr << "[ERROR] Invalid manifest format: missing 'resourceGroups'" << std::endl;
                return false;
            }

            for (const auto& [groupName, groupData] : manifest["resourceGroups"].items()) {
                // 暂时注释掉 ParseGroupConfig 的调用，因为函数定义被注释了
                // ResourcePreloadConfig config = ParseGroupConfig(groupData);
                ResourcePreloadConfig config;
                config.Name = groupName;
                m_ResourceGroups[groupName] = config;
            }

                return true;
        }
        catch (const std::exception& e) {
            std::cerr << "[ERROR] Failed to parse resource manifest: " << e.what() << std::endl;
            return false;
        }
    }

    // 暂时注释掉 ParseGroupConfig 函数的实现，因为函数声明在头文件中被注释了
    /*
    ResourcePreloadConfig ResourcePreloader::ParseGroupConfig(const nlohmann::json& groupData) {
        ResourcePreloadConfig config;

        // 基本配置
        if (groupData.contains("paths") && groupData["paths"].is_array()) {
            for (const auto& path : groupData["paths"]) {
                config.Paths.push_back(path.get<std::string>());
            }
        }

        if (groupData.contains("priority")) {
            config.Priority = groupData["priority"].get<int>();
        }

        if (groupData.contains("loadAsync")) {
            config.LoadAsync = groupData["loadAsync"].get<bool>();
        }

        if (groupData.contains("required")) {
            config.Required = groupData["required"].get<bool>();
        }

        // 纹理配置
        if (groupData.contains("textureSettings")) {
            const auto& texSettings = groupData["textureSettings"];

            if (texSettings.contains("compressionFormat")) {
                config.CompressionFormat = texSettings["compressionFormat"].get<std::string>();
            }
            if (texSettings.contains("generateMipmaps")) {
                config.GenerateMipmaps = texSettings["generateMipmaps"].get<bool>();
            }
            if (texSettings.contains("wrapMode")) {
                config.WrapMode = texSettings["wrapMode"].get<std::string>();
            }
            if (texSettings.contains("filterMode")) {
                config.FilterMode = texSettings["filterMode"].get<std::string>();
            }
        }

        // 模型配置
        if (groupData.contains("modelSettings")) {
            const auto& modelSettings = groupData["modelSettings"];

            if (modelSettings.contains("optimizeMesh")) {
                config.OptimizeMesh = modelSettings["optimizeMesh"].get<bool>();
            }
            if (modelSettings.contains("calculateTangents")) {
                config.CalculateTangents = modelSettings["calculateTangents"].get<bool>();
            }
        }

        // 音频配置
        if (groupData.contains("audioSettings")) {
            const auto& audioSettings = groupData["audioSettings"];

            if (audioSettings.contains("streamingMode")) {
                config.StreamingMode = audioSettings["streamingMode"].get<bool>();
            }
            if (audioSettings.contains("compressionQuality")) {
                config.CompressionQuality = audioSettings["compressionQuality"].get<float>();
            }
        }

        return config;
    }
    */

    void ResourcePreloader::PreloadGroup(const std::string& groupName) {
        auto it = m_ResourceGroups.find(groupName);
        if (it == m_ResourceGroups.end()) {
            std::cerr << "[ERROR] Resource group not found: " << groupName << std::endl;
            return;
        }

        const auto& config = it->second;

        m_LoadingProgress[groupName] = 0.0f;
        m_GroupLoadStatus[groupName] = false;

        size_t loadedCount = 0;
        size_t totalCount = config.Paths.size();

        for (const auto& path : config.Paths) {
            bool success = LoadResourceWithConfig(path, config);
            if (success || !config.Required) {
                loadedCount++;
            } else {
                std::cerr << "[ERROR] Failed to load required resource: " << path << std::endl;
            }

            float progress = static_cast<float>(loadedCount) / totalCount;
            UpdateProgress(groupName, progress);
        }

        bool groupSuccess = (loadedCount == totalCount) ||
                           (loadedCount > 0 && !config.Required);
        m_GroupLoadStatus[groupName] = groupSuccess;
        NotifyCompletion(groupName, groupSuccess);

    }

    void ResourcePreloader::PreloadAllGroups() {
        // 按优先级排序
        std::vector<std::pair<std::string, ResourcePreloadConfig*>> sortedGroups;
        for (auto& pair : m_ResourceGroups) {
            sortedGroups.emplace_back(pair.first, &pair.second);
        }

        std::sort(sortedGroups.begin(), sortedGroups.end(),
                  [](const auto& a, const auto& b) {
                      return a.second->Priority > b.second->Priority;
                  });

        // 按优先级加载
        for (const auto& pair : sortedGroups) {
            PreloadGroup(pair.first);
        }
    }

    std::future<bool> ResourcePreloader::PreloadGroupAsync(const std::string& groupName) {
        return std::async(std::launch::async, [this, groupName]() {
            PreloadGroup(groupName);
            return IsGroupLoaded(groupName);
        });
    }

    float ResourcePreloader::GetLoadingProgress(const std::string& groupName) const {
        auto it = m_LoadingProgress.find(groupName);
        return (it != m_LoadingProgress.end()) ? it->second : 0.0f;
    }

    bool ResourcePreloader::IsGroupLoaded(const std::string& groupName) const {
        auto it = m_GroupLoadStatus.find(groupName);
        return (it != m_GroupLoadStatus.end()) ? it->second : false;
    }

    void ResourcePreloader::SetProgressCallback(
        std::function<void(const std::string&, float)> callback) {
        m_ProgressCallback = callback;
    }

    void ResourcePreloader::SetCompletionCallback(
        std::function<void(const std::string&, bool)> callback) {
        m_CompletionCallback = callback;
    }

    void ResourcePreloader::AddResourceGroup(const std::string& name,
                                           const ResourcePreloadConfig& config) {
        m_ResourceGroups[name] = config;
    }

    void ResourcePreloader::RemoveResourceGroup(const std::string& name) {
        m_ResourceGroups.erase(name);
        m_LoadingProgress.erase(name);
        m_GroupLoadStatus.erase(name);
    }

    const ResourcePreloadConfig* ResourcePreloader::GetGroupConfig(
        const std::string& name) const {
        auto it = m_ResourceGroups.find(name);
        return (it != m_ResourceGroups.end()) ? &it->second : nullptr;
    }

    bool ResourcePreloader::LoadResourceWithConfig(const std::string& path,
                                                  const ResourcePreloadConfig& config) {
        try {
            // 直接通过文件扩展名确定资源类型，避免访问私有方法
            std::filesystem::path filePath(path);
            std::string extension = filePath.extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

            ResourceType type = ResourceType::TEXTURE; // 默认
            if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||
                extension == ".tga" || extension == ".bmp" || extension == ".dds") {
                type = ResourceType::TEXTURE;
            } else if (extension == ".obj" || extension == ".fbx" || extension == ".gltf" ||
                      extension == ".glb" || extension == ".dae" || extension == ".3ds") {
                type = ResourceType::MODEL;
            } else if (extension == ".wav" || extension == ".ogg" || extension == ".mp3" ||
                      extension == ".flac") {
                type = ResourceType::AUDIO;
            }

            switch (type) {
                case ResourceType::TEXTURE: {
                    auto textureResource = ResourceManager::GetInstance()
                        .LoadResource<TextureResource>(path);

                    if (textureResource.IsValid()) {
                        // 应用纹理配置
                        textureResource->SetCompressionFormat(config.CompressionFormat);
                        textureResource->SetGenerateMipmaps(config.GenerateMipmaps);
                        textureResource->SetWrapMode(config.WrapMode);
                        textureResource->SetFilterMode(config.FilterMode);
                        return true;
                    }
                    break;
                }

                case ResourceType::MODEL: {
                    auto modelResource = ResourceManager::GetInstance()
                        .LoadResource<ModelResource>(path);

                    if (modelResource.IsValid()) {
                        // 应用模型配置
                        modelResource->SetOptimizeMesh(config.OptimizeMesh);
                        modelResource->SetCalculateTangents(config.CalculateTangents);
                        return true;
                    }
                    break;
                }

                case ResourceType::AUDIO: {
                    auto audioResource = ResourceManager::GetInstance()
                        .LoadResource<AudioResource>(path);

                    if (audioResource.IsValid()) {
                        // 应用音频配置
                        audioResource->SetStreamingMode(config.StreamingMode);
                        audioResource->SetCompressionQuality(config.CompressionQuality);
                        return true;
                    }
                    break;
                }

                default:
                    return false;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "[ERROR] Exception loading resource " << path << ": " << e.what() << std::endl;
        }

        return false;
    }

    void ResourcePreloader::UpdateProgress(const std::string& groupName, float progress) {
        m_LoadingProgress[groupName] = progress;

        if (m_ProgressCallback) {
            m_ProgressCallback(groupName, progress);
        }
    }

    void ResourcePreloader::NotifyCompletion(const std::string& groupName, bool success) {
        if (m_CompletionCallback) {
            m_CompletionCallback(groupName, success);
        }
    }

}
