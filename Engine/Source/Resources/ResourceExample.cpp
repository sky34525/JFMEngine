//
// ResourceExample.cpp - 资源管理系统使用示例
//

#include "JFMEngine/Resources/ResourceManager.h"
#include "JFMEngine/Resources/ResourcePreloader.h"
#include "JFMEngine/Resources/ResourceLoaders.h"
#include "JFMEngine/Utils//Log.h"

namespace JFM {

    class ResourceExample {
    private:
        ResourcePreloader m_Preloader;

    public:
        void RunExample() {

            // 启动后台加载线程
            ResourceManager::GetInstance().StartBackgroundLoading();

            // 设置进度回调
            SetupCallbacks();

            // 演示不同的加载方式
            DemoBasicLoading();
            DemoPreloading();
            DemoAsyncLoading();
            DemoHotReload();
            DemoMemoryManagement();

            // 清理
            ResourceManager::GetInstance().StopBackgroundLoading();
        }

    private:
        void SetupCallbacks() {
            // 设置加载进度回调
            m_Preloader.SetProgressCallback([](const std::string& groupName, float progress) {
                // Progress update
            });

            // 设置完成回调
            m_Preloader.SetCompletionCallback([](const std::string& groupName, bool success) {
                // Resource group loading completion
            });
        }

        void DemoBasicLoading() {

            // 加载单个纹理
            auto textureHandle = ResourceManager::GetInstance()
                .LoadResource<TextureResource>("Assets/Textures/test.png");

            if (textureHandle.IsValid()) {
                // 使用纹理
                auto texture = textureHandle->GetTexture();
            }

            // 加载模型
            auto modelHandle = ResourceManager::GetInstance()
                .LoadResource<ModelResource>("Assets/Models/test.obj");

            if (modelHandle.IsValid()) {
                // Model loaded successfully
            }
        }

        void DemoPreloading() {

            // 加载资源清单
            if (m_Preloader.LoadManifest("Assets/resource_manifest.json")) {
                // 预加载UI资源（高优先级，同步）
                m_Preloader.PreloadGroup("ui");

                // 预加载所有组（按优先级顺序）
                m_Preloader.PreloadAllGroups();
            }
        }

        void DemoAsyncLoading() {

            // 异步加载大型资源
            auto future = ResourceManager::GetInstance()
                .LoadResourceAsync<ModelResource>("Assets/Models/large_scene.fbx");

            // 在加载期间做其他工作

            // 异步预加载资源组
            auto groupFuture = m_Preloader.PreloadGroupAsync("characters");

            // 等待完成
            if (future.wait_for(std::chrono::seconds(5)) == std::future_status::ready) {
                auto modelHandle = future.get();
            }
        }

        void DemoHotReload() {

            // 启用热重载
            ResourceManager::GetInstance().EnableHotReload(true);

            // 加载一个资源
            auto textureHandle = ResourceManager::GetInstance()
                .LoadResource<TextureResource>("Assets/Textures/test.png");

            if (textureHandle.IsValid()) {
                // 模拟文件修改检查
                ResourceManager::GetInstance().CheckForChangedResources();
            }
        }

        void DemoMemoryManagement() {

            // 设置内存限制
            ResourceManager::GetInstance().SetMaxCacheSize(100 * 1024 * 1024); // 100MB

            // 显示当前内存使用
            size_t memoryUsage = ResourceManager::GetInstance().GetTotalMemoryUsage();

            // 卸载未使用的资源
            ResourceManager::GetInstance().UnloadUnusedResources();

            // 显示清理后的内存使用
            memoryUsage = ResourceManager::GetInstance().GetTotalMemoryUsage();

            // 清空缓存
            ResourceManager::GetInstance().ClearCache();
        }
    };

    // 游戏中的实际使用示例
    class GameResourceManager {
    private:
        ResourcePreloader m_Preloader;
        bool m_InitialLoadComplete = false;

    public:
        bool Initialize() {

            // 启动后台加载
            ResourceManager::GetInstance().StartBackgroundLoading();
            ResourceManager::GetInstance().EnableHotReload(true);

            // 设置内存限制（根据平台调整）
            ResourceManager::GetInstance().SetMaxCacheSize(512 * 1024 * 1024); // 512MB

            // 加载资源清单
            if (!m_Preloader.LoadManifest("Assets/resource_manifest.json")) {
                return false;
            }

            // 设置回调
            m_Preloader.SetCompletionCallback([this](const std::string& group, bool success) {
                OnResourceGroupLoaded(group, success);
            });

            // 预加载关键资源（UI, 玩家角色等）
            m_Preloader.PreloadGroup("ui");
            m_Preloader.PreloadGroup("characters");

            return true;
        }

        void Update() {
            // 检查热重载
            static auto lastCheck = std::chrono::steady_clock::now();
            auto now = std::chrono::steady_clock::now();

            if (std::chrono::duration_cast<std::chrono::seconds>(now - lastCheck).count() >= 1) {
                ResourceManager::GetInstance().CheckForChangedResources();
                lastCheck = now;
            }
        }

        void LoadLevel(const std::string& levelName) {

            // 根据关卡加载对应的资源组
            if (levelName == "forest_level") {
                m_Preloader.PreloadGroup("environment");
                m_Preloader.PreloadGroup("sounds");
            }
            // 可以添加更多关卡特定的资源加载逻辑
        }

        void Shutdown() {
            ResourceManager::GetInstance().StopBackgroundLoading();
            ResourceManager::GetInstance().UnloadAllResources();
        }

    private:
        void OnResourceGroupLoaded(const std::string& groupName, bool success) {
            if (success) {
                // 检查是否所有关键资源都已加载
                if (groupName == "ui" || groupName == "characters") {
                    CheckInitialLoadComplete();
                }
            }
        }

        void CheckInitialLoadComplete() {
            if (!m_InitialLoadComplete &&
                m_Preloader.IsGroupLoaded("ui") &&
                m_Preloader.IsGroupLoaded("characters")) {

                m_InitialLoadComplete = true;
            }
        }
    };

}
