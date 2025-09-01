//
// ResourceManager.cpp - 资源管理器实现
//

#include "JFMEngine/Resources/ResourceManager.h"
#include "JFMEngine/Resources/ResourceLoaders.h"
#include "JFMEngine/Utils//Log.h"
#include <fstream>
#include <algorithm>
#include <chrono>
#include <sys/stat.h>

namespace JFM {

    std::unique_ptr<ResourceManager> ResourceManager::s_Instance = nullptr;

    // Resource基类实现
    Resource::Resource(const std::string& path, ResourceType type)
        : m_Path(path), m_Type(type) {
    }

    // ResourceManager实现
    ResourceManager& ResourceManager::GetInstance() {
        if (!s_Instance) {
            // 使用 new 操作符创建实例，因为构造函数是私有的
            s_Instance.reset(new ResourceManager());
        }
        return *s_Instance;
    }

    ResourceManager::~ResourceManager() {
        StopBackgroundLoading();
        UnloadAllResources();
    }

    std::shared_ptr<Resource> ResourceManager::LoadResourceInternal(const std::string& path, ResourceType type) {
        // 检查资源是否已加载
        {
            std::lock_guard<std::mutex> lock(m_ResourcesMutex);
            auto it = m_Resources.find(path);
            if (it != m_Resources.end()) {
                // 更新访问时间
                m_LastAccessTimes[path] = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
                return it->second;
            }
        }

        // 创建对应类型的资源
        std::shared_ptr<Resource> resource;
        switch (type) {
            case ResourceType::TEXTURE:
                resource = std::make_shared<TextureResource>(path);
                break;
            case ResourceType::MODEL:
                resource = std::make_shared<ModelResource>(path);
                break;
            case ResourceType::AUDIO:
                resource = std::make_shared<AudioResource>(path);
                break;
            default:
                    return nullptr;
        }

        // 加载资源
        if (!resource->Load()) {
            return nullptr;
        }

        // 添加到缓存
        {
            std::lock_guard<std::mutex> lock(m_ResourcesMutex);
            m_Resources[path] = resource;
            m_LastAccessTimes[path] = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        }

        // 检查内存限制
        EnforceMemoryLimit();

        return resource;
    }

    ResourceType ResourceManager::GetResourceTypeFromPath(const std::string& path) {
        size_t dotPos = path.find_last_of('.');
        std::string extension = (dotPos != std::string::npos) ? path.substr(dotPos) : "";
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        // 纹理格式
        if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||
            extension == ".tga" || extension == ".bmp" || extension == ".dds") {
            return ResourceType::TEXTURE;
        }
        // 模型格式
        else if (extension == ".obj" || extension == ".fbx" || extension == ".gltf" ||
                 extension == ".glb" || extension == ".dae" || extension == ".3ds") {
            return ResourceType::MODEL;
        }
        // 音频格式
        else if (extension == ".wav" || extension == ".ogg" || extension == ".mp3" ||
                 extension == ".flac") {
            return ResourceType::AUDIO;
        }
        // 着色器格式
        else if (extension == ".glsl" || extension == ".vert" || extension == ".frag" ||
                 extension == ".geom") {
            return ResourceType::SHADER;
        }

        return ResourceType::TEXTURE; // 默认为纹理
    }

    void ResourceManager::UnloadResource(const std::string& path) {
        std::lock_guard<std::mutex> lock(m_ResourcesMutex);
        auto it = m_Resources.find(path);
        if (it != m_Resources.end()) {
            it->second->Unload();
            m_Resources.erase(it);
            m_LastAccessTimes.erase(path);
        }
    }

    void ResourceManager::UnloadAllResources() {
        std::lock_guard<std::mutex> lock(m_ResourcesMutex);
        for (auto& pair : m_Resources) {
            pair.second->Unload();
        }
        m_Resources.clear();
        m_LastAccessTimes.clear();
    }

    void ResourceManager::UnloadUnusedResources() {
        std::lock_guard<std::mutex> lock(m_ResourcesMutex);
        auto it = m_Resources.begin();
        while (it != m_Resources.end()) {
            if (it->second->GetRefCount() == 0) {
                it->second->Unload();
                m_LastAccessTimes.erase(it->first);
                it = m_Resources.erase(it);
            } else {
                ++it;
            }
        }
    }

    bool ResourceManager::IsResourceLoaded(const std::string& path) const {
        std::lock_guard<std::mutex> lock(m_ResourcesMutex);
        return m_Resources.find(path) != m_Resources.end();
    }

    ResourceState ResourceManager::GetResourceState(const std::string& path) const {
        std::lock_guard<std::mutex> lock(m_ResourcesMutex);
        auto it = m_Resources.find(path);
        if (it != m_Resources.end()) {
            return it->second->GetState();
        }
        return ResourceState::UNLOADED;
    }

    size_t ResourceManager::GetTotalMemoryUsage() const {
        std::lock_guard<std::mutex> lock(m_ResourcesMutex);
        size_t totalSize = 0;
        for (const auto& pair : m_Resources) {
            totalSize += pair.second->GetMemoryUsage();
        }
        return totalSize;
    }

    void ResourceManager::ClearCache() {
        UnloadAllResources();
    }

    void ResourceManager::PreloadResources(const std::vector<std::string>& paths) {
        for (const auto& path : paths) {
            ResourceType type = GetResourceTypeFromPath(path);
            LoadResourceInternal(path, type);
        }
    }

    void ResourceManager::PreloadResourcesFromManifest(const std::string& manifestPath) {
        std::ifstream file(manifestPath);
        if (!file.is_open()) {
            return;
        }

        std::vector<std::string> paths;
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty() && line[0] != '#') { // 忽略空行和注释
                paths.push_back(line);
            }
        }

        PreloadResources(paths);
    }

    void ResourceManager::CheckForChangedResources() {
        if (!m_HotReloadEnabled) return;

        std::lock_guard<std::mutex> lock(m_ResourcesMutex);
        for (auto& pair : m_Resources) {
            const std::string& path = pair.first;
            auto& resource = pair.second;

            std::ifstream testFile(path);
            if (testFile.good()) {
                testFile.close();
                
                // 获取文件修改时间
                struct stat fileStat;
                if (stat(path.c_str(), &fileStat) == 0) {
                    auto timestamp = static_cast<uint64_t>(fileStat.st_mtime) * 1000; // 转换为毫秒
                    
                    // 检查文件是否被修改
                    if (timestamp > m_LastAccessTimes[path]) {
                        resource->Unload();
                        resource->Load();
                        m_LastAccessTimes[path] = timestamp;
                    }
                }
            }
        }
    }

    void ResourceManager::EnforceMemoryLimit() {
        size_t currentUsage = GetTotalMemoryUsage();
        if (currentUsage <= m_MaxCacheSize) return;


        while (currentUsage > m_MaxCacheSize * 0.8f) { // 保持在80%以下
            EvictLeastRecentlyUsed();
            currentUsage = GetTotalMemoryUsage();
        }
    }

    void ResourceManager::EvictLeastRecentlyUsed() {
        std::lock_guard<std::mutex> lock(m_ResourcesMutex);

        if (m_Resources.empty()) return;

        // 找到最近最少使用的资源
        auto oldestIt = m_LastAccessTimes.begin();
        for (auto it = m_LastAccessTimes.begin(); it != m_LastAccessTimes.end(); ++it) {
            if (it->second < oldestIt->second) {
                oldestIt = it;
            }
        }

        // 只驱逐没有引用的资源
        auto resourceIt = m_Resources.find(oldestIt->first);
        if (resourceIt != m_Resources.end() && resourceIt->second->GetRefCount() == 0) {
            resourceIt->second->Unload();
            m_Resources.erase(resourceIt);
            m_LastAccessTimes.erase(oldestIt);
        }
    }

    void ResourceManager::StartBackgroundLoading() {
        if (m_BackgroundThreadRunning) return;

        m_BackgroundThreadRunning = true;
        m_BackgroundThread = std::thread(&ResourceManager::BackgroundLoadingThread, this);
    }

    void ResourceManager::StopBackgroundLoading() {
        if (!m_BackgroundThreadRunning) return;

        m_BackgroundThreadRunning = false;
        if (m_BackgroundThread.joinable()) {
            m_BackgroundThread.join();
        }
    }

    void ResourceManager::BackgroundLoadingThread() {
        while (m_BackgroundThreadRunning) {
            ProcessLoadTasks();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void ResourceManager::ProcessLoadTasks() {
        std::queue<ResourceLoadTask> tasksToProcess;

        // 获取待处理的任务
        {
            std::lock_guard<std::mutex> lock(m_TaskQueueMutex);
            tasksToProcess = std::move(m_LoadTasks);
            m_LoadTasks = std::queue<ResourceLoadTask>();
        }

        // 处理任务
        while (!tasksToProcess.empty()) {
            auto task = std::move(tasksToProcess.front());
            tasksToProcess.pop();

            try {
                auto resource = LoadResourceInternal(task.Path, task.Type);
                if (task.Callback) {
                    task.Callback(resource);
                }
            } catch (const std::exception& e) {
                // Background loading failed
            }
        }
    }

}
