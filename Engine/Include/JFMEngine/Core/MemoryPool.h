//
// Created by kw on 25-7-22.
// 内存池管理器 - 用于高效分配事件对象
//

#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include "JFMEngine/Core/Core.h"
#include <memory>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <queue>
#include <chrono>

namespace JFM {

    // 线程安全的内存池
    //模版参数可带默认值
    template<typename T, size_t PoolSize = 1024>
    class JFM_API MemoryPool {
    public:
        MemoryPool() {
            // 预分配内存块
            m_Pool.reserve(PoolSize);
            for (size_t i = 0; i < PoolSize; ++i) {
                //所有权分离
                m_Pool.emplace_back(std::make_unique<T>());
                m_FreeList.push(m_Pool.back().get());
            }
        }

        // 获取对象（线程安全）
        T* Acquire() {
            std::lock_guard<std::mutex> lock(m_Mutex);
            if (m_FreeList.empty()) {
                // 动态扩展池
                ExpandPool();
            }

            T* obj = m_FreeList.top();
            m_FreeList.pop();
            m_AllocatedCount.fetch_add(1);
            return obj;
        }

        // 释放对象（线程安全）
        void Release(T* obj) {
            if (!obj) return;

            std::lock_guard<std::mutex> lock(m_Mutex);
            m_FreeList.push(obj);
            m_AllocatedCount.fetch_sub(1);
        }

        // 获取统计信息
        size_t GetAllocatedCount() const { return m_AllocatedCount.load(); }
        size_t GetPoolSize() const { return m_Pool.size(); }

    private:
        void ExpandPool() {
            size_t currentSize = m_Pool.size();
            size_t newSize = currentSize + PoolSize / 2; // 扩展50%

            for (size_t i = currentSize; i < newSize; ++i) {
                m_Pool.emplace_back(std::make_unique<T>());
                m_FreeList.push(m_Pool.back().get());
            }
        }

        std::vector<std::unique_ptr<T>> m_Pool;
        std::stack<T*> m_FreeList;
        std::mutex m_Mutex;
        std::atomic<size_t> m_AllocatedCount{0};//用于线程安全地跟踪内存池中已分配对象的数量。
    };

    // 事件内存管理器
    class JFM_API EventMemoryManager {
    public:
        static EventMemoryManager& GetInstance() {
            static EventMemoryManager instance;
            return instance;
        }

        template<typename T, typename... Args>
        T* AllocateEvent(Args&&... args) {
            // 使用placement new在预分配内存上构造对象
            void* memory = AllocateRawMemory(sizeof(T));
            if (!memory) return nullptr;

            // 记录分配信息用于释放时的类型安全检查
            RegisterAllocation(memory, sizeof(T), typeid(T).hash_code());

            return new(memory) T(std::forward<Args>(args)...);
        }

        template<typename T>
        void DeallocateEvent(T* ptr) {
            if (!ptr) return;

            // 先调用析构函数
            ptr->~T();

            // 然后释放内存
            DeallocateRawMemory(ptr, sizeof(T));
        }

        // 获取内存使用统计
        struct MemoryStats {
            size_t totalAllocated{0};      // 总分配内存
            size_t activeAllocations{0};   // 活跃分配数
            size_t totalBlocks{0};         // 总内存块数
            size_t fragmentationRatio{0};  // 碎片化比例(百分比)
        };

        MemoryStats GetStats();

        // 内存压缩整理
        void Compact();

        // 设置释放策略
        enum class ReleasePolicy {
            Immediate,    // 立即释放
            Deferred,     // 延迟释放
            BatchDeferred // 批量延迟释放
        };
        void SetReleasePolicy(ReleasePolicy policy) { m_ReleasePolicy = policy; }

    private:
        void* AllocateRawMemory(size_t size);
        void DeallocateRawMemory(void* ptr, size_t size);

        // 具体释放策略实现
        void DeallocateImmediate(void* ptr, size_t size);
        void DeallocateDeferred(void* ptr, size_t size);
        void DeallocateBatchDeferred(void* ptr, size_t size);

        // 分配信息记录
        void RegisterAllocation(void* ptr, size_t size, size_t typeHash);
        void UnregisterAllocation(void* ptr);

        // 延迟释放处理
        void ProcessDeferredDeallocations();

        std::mutex m_Mutex;
        std::vector<std::unique_ptr<uint8_t[]>> m_MemoryBlocks;
        ReleasePolicy m_ReleasePolicy{ReleasePolicy::Deferred};

        // 分配追踪信息
        struct AllocationInfo {
            size_t size;
            size_t typeHash;
            std::chrono::steady_clock::time_point allocTime;
        };
        std::unordered_map<void*, AllocationInfo> m_Allocations;

        // 延迟释放队列
        struct DeferredDeallocation {
            void* ptr;
            size_t size;
            std::chrono::steady_clock::time_point deallocTime;
        };
        std::queue<DeferredDeallocation> m_DeferredQueue;
    };

}

#endif //MEMORYPOOL_H
