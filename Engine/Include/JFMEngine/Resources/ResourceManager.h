//
// ResourceManager.h - 统一资源管理系统
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

namespace JFM {

    // 前向声明
    class Texture;
    class Model;
    class AudioClip;

    // 资源类型枚举
    enum class ResourceType {
        TEXTURE,
        MODEL,
        AUDIO,
        SHADER,
        MATERIAL,
        ANIMATION,
        FONT
    };

    // 资源状态
    enum class ResourceState {
        UNLOADED,   // 未加载
        LOADING,    // 加载中
        LOADED,     // 已加载
        ERROR       // 加载失败
    };

    // 资源基类
    class JFM_API Resource {
    public:
        Resource(const std::string& path, ResourceType type);
        virtual ~Resource() = default;

        // 基本属性
        const std::string& GetPath() const { return m_Path; }
        ResourceType GetType() const { return m_Type; }
        ResourceState GetState() const { return m_State; }
        size_t GetRefCount() const { return m_RefCount; }

        // 资源操作
        virtual bool Load() = 0;
        virtual void Unload() = 0;
        virtual size_t GetMemoryUsage() const = 0;

        // 引用计数
        void AddRef() { ++m_RefCount; }
        void Release() { --m_RefCount; }

    protected:
        std::string m_Path;
        ResourceType m_Type;
        ResourceState m_State = ResourceState::UNLOADED;
        size_t m_RefCount = 0;
    };

    // 资源句柄
    template<typename T>
    class ResourceHandle {
    public:
        ResourceHandle() = default;
        ResourceHandle(std::shared_ptr<T> resource) : m_Resource(resource) {
            if (m_Resource) m_Resource->AddRef();
        }

        ResourceHandle(const ResourceHandle& other) : m_Resource(other.m_Resource) {
            if (m_Resource) m_Resource->AddRef();
        }

        ResourceHandle& operator=(const ResourceHandle& other) {
            if (this != &other) {
                if (m_Resource) m_Resource->Release();
                m_Resource = other.m_Resource;
                if (m_Resource) m_Resource->AddRef();
            }
            return *this;
        }

        ~ResourceHandle() {
            if (m_Resource) m_Resource->Release();
        }

        T* Get() const { return m_Resource.get(); }
        T* operator->() const { return m_Resource.get(); }
        T& operator*() const { return *m_Resource; }
        bool IsValid() const { return m_Resource != nullptr; }

    private:
        std::shared_ptr<T> m_Resource;
    };

    // 资源加载任务
    struct ResourceLoadTask {
        std::string Path;
        ResourceType Type;
        std::promise<std::shared_ptr<Resource>> Promise;
        std::function<void(std::shared_ptr<Resource>)> Callback;
    };

    // 资源加载器基类
    class JFM_API ResourceLoader {
    public:
        virtual ~ResourceLoader() = default;

        virtual std::vector<std::string> GetSupportedExtensions() const = 0;
        virtual std::shared_ptr<Resource> LoadResource(const std::string& path,
                                                      const std::unordered_map<std::string, std::string>& params = {}) = 0;
        virtual bool CanLoadAsync() const { return false; }
    };

    // 资源管理器
    class JFM_API ResourceManager {
    public:
        static ResourceManager& GetInstance();

        // 资源加载
        template<typename T>
        ResourceHandle<T> LoadResource(const std::string& path);

        template<typename T>
        std::future<ResourceHandle<T>> LoadResourceAsync(const std::string& path);

        // 资源卸载
        void UnloadResource(const std::string& path);
        void UnloadAllResources();
        void UnloadUnusedResources();

        // 资源查询
        bool IsResourceLoaded(const std::string& path) const;
        ResourceState GetResourceState(const std::string& path) const;
        size_t GetTotalMemoryUsage() const;

        // 缓存管理
        void SetMaxCacheSize(size_t maxSize) { m_MaxCacheSize = maxSize; }
        void ClearCache();

        // 预加载
        void PreloadResources(const std::vector<std::string>& paths);
        void PreloadResourcesFromManifest(const std::string& manifestPath);

        // 热重载
        void EnableHotReload(bool enable) { m_HotReloadEnabled = enable; }
        void CheckForChangedResources();

        // 线程控制
        void StartBackgroundLoading();
        void StopBackgroundLoading();

        // 公共工具方法
        ResourceType GetResourceTypeFromPath(const std::string& path);

    private:
        // 友元声明，允许 std::unique_ptr 访问私有析构函数
        friend class std::default_delete<ResourceManager>;

        ResourceManager() = default;
        ~ResourceManager();

        // 内部加载方法
        std::shared_ptr<Resource> LoadResourceInternal(const std::string& path, ResourceType type);

        // 缓存管理
        void EnforceMemoryLimit();
        void EvictLeastRecentlyUsed();

        // 后台加载线程
        void BackgroundLoadingThread();
        void ProcessLoadTasks();

        // 成员变量
        std::unordered_map<std::string, std::shared_ptr<Resource>> m_Resources;
        std::unordered_map<std::string, uint64_t> m_LastAccessTimes;

        mutable std::mutex m_ResourcesMutex;
        std::mutex m_TaskQueueMutex;

        std::queue<ResourceLoadTask> m_LoadTasks;
        std::thread m_BackgroundThread;
        std::atomic<bool> m_BackgroundThreadRunning{false};

        size_t m_MaxCacheSize = 1024 * 1024 * 1024; // 1GB默认
        bool m_HotReloadEnabled = false;

        static std::unique_ptr<ResourceManager> s_Instance;
    };

    // 模板实现
    template<typename T>
    ResourceHandle<T> ResourceManager::LoadResource(const std::string& path) {
        auto resource = LoadResourceInternal(path, GetResourceTypeFromPath(path));
        return ResourceHandle<T>(std::static_pointer_cast<T>(resource));
    }

    template<typename T>
    std::future<ResourceHandle<T>> ResourceManager::LoadResourceAsync(const std::string& path) {
        auto promise = std::make_shared<std::promise<ResourceHandle<T>>>();
        auto future = promise->get_future();

        ResourceLoadTask task;
        task.Path = path;
        task.Type = GetResourceTypeFromPath(path);
        task.Callback = [promise](std::shared_ptr<Resource> resource) {
            promise->set_value(ResourceHandle<T>(std::static_pointer_cast<T>(resource)));
        };

        {
            std::lock_guard<std::mutex> lock(m_TaskQueueMutex);
            m_LoadTasks.push(std::move(task));
        }

        return future;
    }

}
