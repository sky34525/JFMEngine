//
// ResourcePreloader.h - 资源预加载和配置管理
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "ResourceManager.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
// 暂时注释掉 nlohmann/json，因为项目中没有配置这个依赖
// #include <nlohmann/json.hpp>

namespace JFM {

    // 资源预加载配置
    struct ResourcePreloadConfig {
        std::string Name;
        std::vector<std::string> Paths;
        int Priority = 0;           // 加载优先级
        bool LoadAsync = true;      // 是否异步加载
        bool Required = false;      // 是否必需资源

        // 纹理特定配置
        std::string CompressionFormat = "DXT5";
        bool GenerateMipmaps = true;
        std::string WrapMode = "REPEAT";
        std::string FilterMode = "LINEAR";

        // 模型特定配置
        bool OptimizeMesh = true;
        bool CalculateTangents = true;

        // 音频特定配置
        bool StreamingMode = false;
        float CompressionQuality = 0.8f;
    };

    // 资源预加载器
    class JFM_API ResourcePreloader {
    public:
        // 加载配置文件
        bool LoadManifest(const std::string& manifestPath);

        // 预加载指定组的资源
        void PreloadGroup(const std::string& groupName);
        void PreloadAllGroups();

        // 异步预加载
        std::future<bool> PreloadGroupAsync(const std::string& groupName);

        // 进度查询
        float GetLoadingProgress(const std::string& groupName) const;
        bool IsGroupLoaded(const std::string& groupName) const;

        // 事件回调
        void SetProgressCallback(std::function<void(const std::string&, float)> callback);
        void SetCompletionCallback(std::function<void(const std::string&, bool)> callback);

        // 配置管理
        void AddResourceGroup(const std::string& name, const ResourcePreloadConfig& config);
        void RemoveResourceGroup(const std::string& name);
        const ResourcePreloadConfig* GetGroupConfig(const std::string& name) const;

    private:
        std::unordered_map<std::string, ResourcePreloadConfig> m_ResourceGroups;
        std::unordered_map<std::string, float> m_LoadingProgress;
        std::unordered_map<std::string, bool> m_GroupLoadStatus;

        std::function<void(const std::string&, float)> m_ProgressCallback;
        std::function<void(const std::string&, bool)> m_CompletionCallback;

        bool LoadResourceWithConfig(const std::string& path, const ResourcePreloadConfig& config);
        void UpdateProgress(const std::string& groupName, float progress);
        void NotifyCompletion(const std::string& groupName, bool success);

        // JSON解析辅助方法
        bool ParseManifestJSON(const std::string& jsonContent);
        // 暂时注释掉使用 nlohmann::json 的函数
        // ResourcePreloadConfig ParseGroupConfig(const nlohmann::json& groupData);
    };

}
